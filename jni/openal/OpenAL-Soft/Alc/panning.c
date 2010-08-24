/**
 * OpenAL cross platform audio library
 * Copyright (C) 1999-2010 by authors.
 * This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA  02111-1307, USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */

#include "config.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "alMain.h"
#include "AL/al.h"
#include "AL/alc.h"
#include "alu.h"

static void SetSpeakerArrangement(const char *name, ALfloat SpeakerAngle[OUTPUTCHANNELS],
                                  Channel Speaker2Chan[OUTPUTCHANNELS], ALint chans)
{
    char layout_str[256];
    char *confkey, *next;
    char *sep, *end;
    Channel val;
    int i;

    strncpy(layout_str, GetConfigValue(NULL, name, ""), sizeof(layout_str));
    layout_str[sizeof(layout_str)-1] = 0;

    if(!layout_str[0])
        return;

    next = confkey = layout_str;
    while(next && *next)
    {
        confkey = next;
        next = strchr(confkey, ',');
        if(next)
        {
            *next = 0;
            do {
                next++;
            } while(isspace(*next) || *next == ',');
        }

        sep = strchr(confkey, '=');
        if(!sep || confkey == sep)
            continue;

        end = sep - 1;
        while(isspace(*end) && end != confkey)
            end--;
        *(++end) = 0;

        if(strcmp(confkey, "fl") == 0 || strcmp(confkey, "front-left") == 0)
            val = FRONT_LEFT;
        else if(strcmp(confkey, "fr") == 0 || strcmp(confkey, "front-right") == 0)
            val = FRONT_RIGHT;
        else if(strcmp(confkey, "fc") == 0 || strcmp(confkey, "front-center") == 0)
            val = FRONT_CENTER;
        else if(strcmp(confkey, "bl") == 0 || strcmp(confkey, "back-left") == 0)
            val = BACK_LEFT;
        else if(strcmp(confkey, "br") == 0 || strcmp(confkey, "back-right") == 0)
            val = BACK_RIGHT;
        else if(strcmp(confkey, "bc") == 0 || strcmp(confkey, "back-center") == 0)
            val = BACK_CENTER;
        else if(strcmp(confkey, "sl") == 0 || strcmp(confkey, "side-left") == 0)
            val = SIDE_LEFT;
        else if(strcmp(confkey, "sr") == 0 || strcmp(confkey, "side-right") == 0)
            val = SIDE_RIGHT;
        else
        {
            AL_PRINT("Unknown speaker for %s: \"%s\"\n", name, confkey);
            continue;
        }

        *(sep++) = 0;
        while(isspace(*sep))
            sep++;

        for(i = 0;i < chans;i++)
        {
            if(Speaker2Chan[i] == val)
            {
                long angle = strtol(sep, NULL, 10);
                if(angle >= -180 && angle <= 180)
                    SpeakerAngle[i] = angle * M_PI/180.0f;
                else
                    AL_PRINT("Invalid angle for speaker \"%s\": %ld\n", confkey, angle);
                break;
            }
        }
    }

    for(i = 0;i < chans;i++)
    {
        int min = i;
        int i2;

        for(i2 = i+1;i2 < chans;i2++)
        {
            if(SpeakerAngle[i2] < SpeakerAngle[min])
                min = i2;
        }

        if(min != i)
        {
            ALfloat tmpf;
            Channel tmpc;

            tmpf = SpeakerAngle[i];
            SpeakerAngle[i] = SpeakerAngle[min];
            SpeakerAngle[min] = tmpf;

            tmpc = Speaker2Chan[i];
            Speaker2Chan[i] = Speaker2Chan[min];
            Speaker2Chan[min] = tmpc;
        }
    }
}

static ALfloat aluLUTpos2Angle(ALint pos)
{
    if(pos < QUADRANT_NUM)
        return aluAtan((ALfloat)pos / (ALfloat)(QUADRANT_NUM - pos));
    if(pos < 2 * QUADRANT_NUM)
        return M_PI_2 + aluAtan((ALfloat)(pos - QUADRANT_NUM) / (ALfloat)(2 * QUADRANT_NUM - pos));
    if(pos < 3 * QUADRANT_NUM)
        return aluAtan((ALfloat)(pos - 2 * QUADRANT_NUM) / (ALfloat)(3 * QUADRANT_NUM - pos)) - M_PI;
    return aluAtan((ALfloat)(pos - 3 * QUADRANT_NUM) / (ALfloat)(4 * QUADRANT_NUM - pos)) - M_PI_2;
}

ALvoid aluInitPanning(ALCdevice *Device)
{
    ALfloat SpeakerAngle[OUTPUTCHANNELS];
    Channel *Speaker2Chan;
    ALfloat Alpha, Theta;
    ALint pos, offset;
    ALuint s, s2;

    for(s = 0;s < OUTPUTCHANNELS;s++)
    {
        for(s2 = 0;s2 < OUTPUTCHANNELS;s2++)
            Device->ChannelMatrix[s][s2] = ((s==s2) ? 1.0f : 0.0f);
    }

    Speaker2Chan = Device->Speaker2Chan;
    switch(Device->Format)
    {
        case AL_FORMAT_MONO8:
        case AL_FORMAT_MONO16:
        case AL_FORMAT_MONO_FLOAT32:
            Device->DuplicateStereo = AL_FALSE;
            Device->ChannelMatrix[FRONT_LEFT][FRONT_CENTER]  = aluSqrt(0.5);
            Device->ChannelMatrix[FRONT_RIGHT][FRONT_CENTER] = aluSqrt(0.5);
            Device->ChannelMatrix[SIDE_LEFT][FRONT_CENTER]   = aluSqrt(0.5);
            Device->ChannelMatrix[SIDE_RIGHT][FRONT_CENTER]  = aluSqrt(0.5);
            Device->ChannelMatrix[BACK_LEFT][FRONT_CENTER]   = aluSqrt(0.5);
            Device->ChannelMatrix[BACK_RIGHT][FRONT_CENTER]  = aluSqrt(0.5);
            Device->ChannelMatrix[BACK_CENTER][FRONT_CENTER] = 1.0f;
            Device->NumChan = 1;
            Speaker2Chan[0] = FRONT_CENTER;
            SpeakerAngle[0] = 0.0f * M_PI/180.0f;
            break;

        case AL_FORMAT_STEREO8:
        case AL_FORMAT_STEREO16:
        case AL_FORMAT_STEREO_FLOAT32:
            Device->DuplicateStereo = AL_FALSE;
            Device->ChannelMatrix[FRONT_CENTER][FRONT_LEFT]  = aluSqrt(0.5);
            Device->ChannelMatrix[FRONT_CENTER][FRONT_RIGHT] = aluSqrt(0.5);
            Device->ChannelMatrix[SIDE_LEFT][FRONT_LEFT]     = 1.0f;
            Device->ChannelMatrix[SIDE_RIGHT][FRONT_RIGHT]   = 1.0f;
            Device->ChannelMatrix[BACK_LEFT][FRONT_LEFT]     = 1.0f;
            Device->ChannelMatrix[BACK_RIGHT][FRONT_RIGHT]   = 1.0f;
            Device->ChannelMatrix[BACK_CENTER][FRONT_LEFT]   = aluSqrt(0.5);
            Device->ChannelMatrix[BACK_CENTER][FRONT_RIGHT]  = aluSqrt(0.5);
            Device->NumChan = 2;
            Speaker2Chan[0] = FRONT_LEFT;
            Speaker2Chan[1] = FRONT_RIGHT;
            SpeakerAngle[0] = -90.0f * M_PI/180.0f;
            SpeakerAngle[1] =  90.0f * M_PI/180.0f;
            SetSpeakerArrangement("layout", SpeakerAngle, Speaker2Chan, Device->NumChan);
            break;

        case AL_FORMAT_QUAD8:
        case AL_FORMAT_QUAD16:
        case AL_FORMAT_QUAD32:
            Device->DuplicateStereo = GetConfigValueBool(NULL, "stereodup", 0);
            Device->ChannelMatrix[FRONT_CENTER][FRONT_LEFT]  = aluSqrt(0.5);
            Device->ChannelMatrix[FRONT_CENTER][FRONT_RIGHT] = aluSqrt(0.5);
            Device->ChannelMatrix[SIDE_LEFT][FRONT_LEFT]     = aluSqrt(0.5);
            Device->ChannelMatrix[SIDE_LEFT][BACK_LEFT]      = aluSqrt(0.5);
            Device->ChannelMatrix[SIDE_RIGHT][FRONT_RIGHT]   = aluSqrt(0.5);
            Device->ChannelMatrix[SIDE_RIGHT][BACK_RIGHT]    = aluSqrt(0.5);
            Device->ChannelMatrix[BACK_CENTER][BACK_LEFT]    = aluSqrt(0.5);
            Device->ChannelMatrix[BACK_CENTER][BACK_RIGHT]   = aluSqrt(0.5);
            Device->NumChan = 4;
            Speaker2Chan[0] = BACK_LEFT;
            Speaker2Chan[1] = FRONT_LEFT;
            Speaker2Chan[2] = FRONT_RIGHT;
            Speaker2Chan[3] = BACK_RIGHT;
            SpeakerAngle[0] = -135.0f * M_PI/180.0f;
            SpeakerAngle[1] =  -45.0f * M_PI/180.0f;
            SpeakerAngle[2] =   45.0f * M_PI/180.0f;
            SpeakerAngle[3] =  135.0f * M_PI/180.0f;
            SetSpeakerArrangement("layout", SpeakerAngle, Speaker2Chan, Device->NumChan);
            break;

        case AL_FORMAT_51CHN8:
        case AL_FORMAT_51CHN16:
        case AL_FORMAT_51CHN32:
            Device->DuplicateStereo = GetConfigValueBool(NULL, "stereodup", 0);
            Device->ChannelMatrix[SIDE_LEFT][FRONT_LEFT]   = aluSqrt(0.5);
            Device->ChannelMatrix[SIDE_LEFT][BACK_LEFT]    = aluSqrt(0.5);
            Device->ChannelMatrix[SIDE_RIGHT][FRONT_RIGHT] = aluSqrt(0.5);
            Device->ChannelMatrix[SIDE_RIGHT][BACK_RIGHT]  = aluSqrt(0.5);
            Device->ChannelMatrix[BACK_CENTER][BACK_LEFT]  = aluSqrt(0.5);
            Device->ChannelMatrix[BACK_CENTER][BACK_RIGHT] = aluSqrt(0.5);
            Device->NumChan = 5;
            Speaker2Chan[0] = BACK_LEFT;
            Speaker2Chan[1] = FRONT_LEFT;
            Speaker2Chan[2] = FRONT_CENTER;
            Speaker2Chan[3] = FRONT_RIGHT;
            Speaker2Chan[4] = BACK_RIGHT;
            SpeakerAngle[0] = -110.0f * M_PI/180.0f;
            SpeakerAngle[1] =  -30.0f * M_PI/180.0f;
            SpeakerAngle[2] =    0.0f * M_PI/180.0f;
            SpeakerAngle[3] =   30.0f * M_PI/180.0f;
            SpeakerAngle[4] =  110.0f * M_PI/180.0f;
            SetSpeakerArrangement("layout", SpeakerAngle, Speaker2Chan, Device->NumChan);
            break;

        case AL_FORMAT_61CHN8:
        case AL_FORMAT_61CHN16:
        case AL_FORMAT_61CHN32:
            Device->DuplicateStereo = GetConfigValueBool(NULL, "stereodup", 0);
            Device->ChannelMatrix[BACK_LEFT][BACK_CENTER]  = aluSqrt(0.5);
            Device->ChannelMatrix[BACK_LEFT][SIDE_LEFT]    = aluSqrt(0.5);
            Device->ChannelMatrix[BACK_RIGHT][BACK_CENTER] = aluSqrt(0.5);
            Device->ChannelMatrix[BACK_RIGHT][SIDE_RIGHT]  = aluSqrt(0.5);
            Device->NumChan = 6;
            Speaker2Chan[0] = SIDE_LEFT;
            Speaker2Chan[1] = FRONT_LEFT;
            Speaker2Chan[2] = FRONT_CENTER;
            Speaker2Chan[3] = FRONT_RIGHT;
            Speaker2Chan[4] = SIDE_RIGHT;
            Speaker2Chan[5] = BACK_CENTER;
            SpeakerAngle[0] = -90.0f * M_PI/180.0f;
            SpeakerAngle[1] = -30.0f * M_PI/180.0f;
            SpeakerAngle[2] =   0.0f * M_PI/180.0f;
            SpeakerAngle[3] =  30.0f * M_PI/180.0f;
            SpeakerAngle[4] =  90.0f * M_PI/180.0f;
            SpeakerAngle[5] = 180.0f * M_PI/180.0f;
            SetSpeakerArrangement("layout", SpeakerAngle, Speaker2Chan, Device->NumChan);
            break;

        case AL_FORMAT_71CHN8:
        case AL_FORMAT_71CHN16:
        case AL_FORMAT_71CHN32:
            Device->DuplicateStereo = GetConfigValueBool(NULL, "stereodup", 0);
            Device->ChannelMatrix[BACK_CENTER][BACK_LEFT]  = aluSqrt(0.5);
            Device->ChannelMatrix[BACK_CENTER][BACK_RIGHT] = aluSqrt(0.5);
            Device->NumChan = 7;
            Speaker2Chan[0] = BACK_LEFT;
            Speaker2Chan[1] = SIDE_LEFT;
            Speaker2Chan[2] = FRONT_LEFT;
            Speaker2Chan[3] = FRONT_CENTER;
            Speaker2Chan[4] = FRONT_RIGHT;
            Speaker2Chan[5] = SIDE_RIGHT;
            Speaker2Chan[6] = BACK_RIGHT;
            SpeakerAngle[0] = -150.0f * M_PI/180.0f;
            SpeakerAngle[1] =  -90.0f * M_PI/180.0f;
            SpeakerAngle[2] =  -30.0f * M_PI/180.0f;
            SpeakerAngle[3] =    0.0f * M_PI/180.0f;
            SpeakerAngle[4] =   30.0f * M_PI/180.0f;
            SpeakerAngle[5] =   90.0f * M_PI/180.0f;
            SpeakerAngle[6] =  150.0f * M_PI/180.0f;
            SetSpeakerArrangement("layout", SpeakerAngle, Speaker2Chan, Device->NumChan);
            break;

        default:
            assert(0);
    }

    if(GetConfigValueBool(NULL, "scalemix", 0))
    {
        ALfloat maxout = 1.0f;
        for(s = 0;s < OUTPUTCHANNELS;s++)
        {
            ALfloat out = 0.0f;
            for(s2 = 0;s2 < OUTPUTCHANNELS;s2++)
                out += Device->ChannelMatrix[s2][s];
            maxout = __max(maxout, out);
        }

        maxout = 1.0f/maxout;
        for(s = 0;s < OUTPUTCHANNELS;s++)
        {
            for(s2 = 0;s2 < OUTPUTCHANNELS;s2++)
                Device->ChannelMatrix[s2][s] *= maxout;
        }
    }

    for(pos = 0; pos < LUT_NUM; pos++)
    {
        /* clear all values */
        offset = OUTPUTCHANNELS * pos;
        for(s = 0; s < OUTPUTCHANNELS; s++)
            Device->PanningLUT[offset+s] = 0.0f;

        if(Device->NumChan == 1)
        {
            Device->PanningLUT[offset + Speaker2Chan[0]] = 1.0f;
            continue;
        }

        /* source angle */
        Theta = aluLUTpos2Angle(pos);

        /* set panning values */
        for(s = 0; s < Device->NumChan - 1; s++)
        {
            if(Theta >= SpeakerAngle[s] && Theta < SpeakerAngle[s+1])
            {
                /* source between speaker s and speaker s+1 */
                Alpha = M_PI_2 * (Theta-SpeakerAngle[s]) /
                                 (SpeakerAngle[s+1]-SpeakerAngle[s]);
                Device->PanningLUT[offset + Speaker2Chan[s]]   = cos(Alpha);
                Device->PanningLUT[offset + Speaker2Chan[s+1]] = sin(Alpha);
                break;
            }
        }
        if(s == Device->NumChan - 1)
        {
            /* source between last and first speaker */
            if(Theta < SpeakerAngle[0])
                Theta += 2.0f * M_PI;
            Alpha = M_PI_2 * (Theta-SpeakerAngle[s]) /
                             (2.0f * M_PI + SpeakerAngle[0]-SpeakerAngle[s]);
            Device->PanningLUT[offset + Speaker2Chan[s]] = cos(Alpha);
            Device->PanningLUT[offset + Speaker2Chan[0]] = sin(Alpha);
        }
    }
}
