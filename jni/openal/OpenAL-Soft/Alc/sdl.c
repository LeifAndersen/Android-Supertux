/** 
 * OpenAL cross platform audio library 
 * Copyright (C) 1999-2007 by authors. 
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
 
#include "alMain.h" 
 
#ifdef HAVE_DLFCN_H 
#include <dlfcn.h> 
#endif 
 
#include "AL/al.h" 
#include "AL/alc.h" 
 
/* SDL.h may redefine this. */ 
#ifdef HAVE_STDINT_H 
#undef HAVE_STDINT_H 
#endif 
 
#include "SDL.h" 
 
static const ALCchar sdl_device[] = "Simple Directmedia Layer"; 
 
/* dynamic library loading code. */ 
static void *sdl_handle; 
static volatile ALuint load_count; 
 
#define MAKE_FUNC(x) static typeof(x) * p##x 
MAKE_FUNC(SDL_InitSubSystem); 
MAKE_FUNC(SDL_OpenAudio); 
MAKE_FUNC(SDL_CloseAudio); 
MAKE_FUNC(SDL_PauseAudio); 
MAKE_FUNC(SDL_QuitSubSystem); 
MAKE_FUNC(SDL_WasInit); 
MAKE_FUNC(SDL_GetError); 
#undef MAKE_FUNC 
 
static void sdl_load(void) 
{ 
    if(load_count == 0) 
    { 
#ifdef _WIN32 
        sdl_handle = LoadLibrary("SDL.dll"); 
#define LOAD_FUNC(x) do { \ 
    p##x = GetProcAddress(sdl_handle, #x); \ 
    if(!(p##x)) { \ 
        AL_PRINT("Could not load %s from SDL.dll\n", #x); \ 
        FreeLibrary(sdl_handle); \ 
        sdl_handle = NULL; \ 
        return; \ 
    } \ 
} while(0) 
 
#elif defined (HAVE_DLFCN_H) 
 
        const char *err; 
#if defined(__APPLE__) && defined(__MACH__) 
        sdl_handle = dlopen("libSDL-1.2.0.dylib", RTLD_NOW); 
#else 
        sdl_handle = dlopen("libSDL-1.2.so.0", RTLD_NOW); 
#endif 
        dlerror(); 
 
#define LOAD_FUNC(x) do { \ 
    p##x = dlsym(sdl_handle, #x); \ 
    if((err=dlerror()) != NULL) { \ 
        AL_PRINT("Could not load %s from libSDL-1.2: %s\n", #x, err); \ 
        dlclose(sdl_handle); \ 
        sdl_handle = NULL; \ 
        return; \ 
    } \ 
} while(0) 
 
#else 
 
        sdl_handle = (void*)0xDEADBEEF; 
#define LOAD_FUNC(x) p##x = (x) 
 
#endif 
        if(!sdl_handle) 
            return; 
 
        LOAD_FUNC(SDL_InitSubSystem); 
        LOAD_FUNC(SDL_OpenAudio); 
        LOAD_FUNC(SDL_CloseAudio); 
        LOAD_FUNC(SDL_PauseAudio); 
        LOAD_FUNC(SDL_QuitSubSystem); 
        LOAD_FUNC(SDL_WasInit); 
        LOAD_FUNC(SDL_GetError); 
 
#undef LOAD_FUNC 
    } 
    ++load_count; 
} 
 
static void sdl_unload(void) 
{ 
    if(load_count == 0 || --load_count > 0) 
        return; 
 
#ifdef _WIN32 
    FreeLibrary(sdl_handle); 
#elif defined (HAVE_DLFCN_H) 
    dlclose(sdl_handle); 
#endif 
    sdl_handle = NULL; 
} 
 
 
static void SDLCALL sdl_callback(void *userdata, Uint8 *stream, int len) 
{ 
    ALCdevice *pDevice = (ALCdevice*)userdata; 
    const ALint frameSize = aluChannelsFromFormat(pDevice->Format) * 
                aluBytesFromFormat(pDevice->Format); 
    aluMixData(pDevice, stream, len/frameSize); 
} 
 
static ALCboolean sdl_open_playback(ALCdevice *device, const ALCchar *deviceName) 
{ 
    (void) device; 
    (void) deviceName; 
 
    sdl_load(); 
    if(!sdl_handle) 
        return ALC_FALSE; 
 
    return ALC_TRUE; 
} 
 
static void sdl_close_playback(ALCdevice *device) 
{ 
    (void) device; 
    pSDL_PauseAudio(1); 
    pSDL_CloseAudio(); 
    pSDL_QuitSubSystem(SDL_INIT_AUDIO); 
    sdl_unload(); 
} 
 
static ALCboolean sdl_reset_playback(ALCdevice *device) 
{ 
    SDL_AudioSpec desired; 
    memset(&desired, '\0', sizeof (desired)); 
 
    if (pSDL_WasInit(SDL_INIT_AUDIO)) 
    { 
        pSDL_PauseAudio(1); 
        pSDL_CloseAudio(); 
        pSDL_QuitSubSystem(SDL_INIT_AUDIO); 
    } 
 
    switch(aluBytesFromFormat(device->Format)) 
    { 
        case 1: 
            desired.format = AUDIO_U8; 
            break; 
        case 4: 
            switch(aluChannelsFromFormat(device->Format)) 
            { 
                case 1: device->Format = AL_FORMAT_MONO16; break; 
                case 2: device->Format = AL_FORMAT_STEREO16; break; 
                case 4: device->Format = AL_FORMAT_QUAD16; break; 
                case 6: device->Format = AL_FORMAT_51CHN16; break; 
                case 7: device->Format = AL_FORMAT_61CHN16; break; 
                case 8: device->Format = AL_FORMAT_71CHN16; break; 
            } 
            /* fall-through */ 
        case 2: 
            desired.format = AUDIO_S16; 
            break; 
        default: 
            AL_PRINT("Unknown format: 0x%x\n", device->Format); 
            return ALC_FALSE; 
    } 
 
	desired.freq = device->Frequency; 
	desired.channels = aluChannelsFromFormat(device->Format); 
	desired.samples = device->UpdateSize * desired.channels; 
	desired.callback = sdl_callback; 
	desired.userdata = device; 
 
    device->NumUpdates = 2; 
 
    if (pSDL_InitSubSystem(SDL_INIT_AUDIO) == -1) 
    { 
        AL_PRINT("SDL_InitSubSystem(SDL_INIT_AUDIO) failed: %s\n", pSDL_GetError()); 
        return ALC_FALSE; 
    } 
 
    if (pSDL_OpenAudio(&desired, NULL) == -1) 
    { 
        AL_PRINT("SDL_OpenAudio failed: %s\n", pSDL_GetError()); 
        pSDL_QuitSubSystem(SDL_INIT_AUDIO); 
        return ALC_FALSE; 
    } 
 
    pSDL_PauseAudio(0); 
    return ALC_TRUE; 
} 
 
static void sdl_stop_playback(ALCdevice *device) 
{ 
    (void) device; 
    pSDL_PauseAudio(1); 
} 
 
static void sdl_warn_nocapture(void) 
{ 
    static int warned = 0; 
    if (!warned) 
    { 
        AL_PRINT("No OpenAL capture support in the SDL backend at this time.\n"); 
        warned = 1; 
    } 
} 
 
static ALCboolean sdl_open_capture(ALCdevice *device, const ALCchar *deviceName) 
{ 
    (void) device; 
    (void) deviceName; 
    sdl_warn_nocapture(); 
    return ALC_FALSE; 
} 
 
static void sdl_close_capture(ALCdevice *device) 
{ 
    (void) device; 
    sdl_warn_nocapture(); 
} 
 
static void sdl_start_capture(ALCdevice *pDevice) 
{ 
    (void) pDevice; 
    sdl_warn_nocapture(); 
} 
 
static void sdl_stop_capture(ALCdevice *pDevice) 
{ 
    (void) pDevice; 
    sdl_warn_nocapture(); 
} 
 
static void sdl_capture_samples(ALCdevice *pDevice, ALCvoid *pBuffer, ALCuint lSamples) 
{ 
    (void) pDevice; 
    (void) pBuffer; 
    (void) lSamples; 
    sdl_warn_nocapture(); 
} 
 
static ALCuint sdl_available_samples(ALCdevice *pDevice) 
{ 
    (void) pDevice; 
    sdl_warn_nocapture(); 
    return 0; 
} 
 
 
BackendFuncs sdl_funcs = { 
    sdl_open_playback, 
    sdl_close_playback, 
    sdl_reset_playback, 
    sdl_stop_playback, 
    sdl_open_capture, 
    sdl_close_capture, 
    sdl_start_capture, 
    sdl_stop_capture, 
    sdl_capture_samples, 
    sdl_available_samples 
}; 
 
void alc_sdl_init(BackendFuncs *func_list) 
{ 
    *func_list = sdl_funcs; 
} 
 
void alc_sdl_deinit(void) 
{ 
    // no-op. 
} 
 
void alc_sdl_probe(int type) 
{ 
    if(type == DEVICE_PROBE) 
        AppendDeviceList(sdl_device); 
    else if(type == ALL_DEVICE_PROBE) 
        AppendAllDeviceList(sdl_device); 
    /* No SDL capture support until SDL 1.3! */ 
} 
 
