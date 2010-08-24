//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_SUPERTUX_VIDEO_SDL_SURFACE_DATA_HPP
#define HEADER_SUPERTUX_VIDEO_SDL_SURFACE_DATA_HPP

#include <config.h>

#include "supertux/gameconfig.hpp"
#include "supertux/globals.hpp"
#include "video/surface.hpp"
#include "video/surface_data.hpp"
#include "video/texture.hpp"

class SDLSurfaceData : public SurfaceData
{
private:
  const Surface &surface;
  SDL_Rect src_rects[NUM_EFFECTS];

public:
  SDLSurfaceData(const Surface &surface) :
    surface(surface)
  {
    int numerator   = 1;
    int denominator = 1;
    //float xfactor = 1.0f; // FIXME: (float) config->screenwidth  / SCREEN_WIDTH;
    //float yfactor = 1.0f; // FIXME: (float) config->screenheight / SCREEN_HEIGHT;

    /* FIXME: 
       if(xfactor < yfactor)
       {
       numerator = config->screenwidth;
       denominator = SCREEN_WIDTH;
       }
       else
       {
       numerator = config->screenheight;
       denominator = SCREEN_HEIGHT;
       }
    */

    src_rects[NO_EFFECT].x = surface.get_x() * numerator / denominator;
    src_rects[NO_EFFECT].y = surface.get_y() * numerator / denominator;
    src_rects[NO_EFFECT].w = surface.get_width() * numerator / denominator;
    src_rects[NO_EFFECT].h = surface.get_height() * numerator / denominator;

    int flipped_x = surface.get_texture()->get_texture_width() - surface.get_x() - surface.get_width();
    src_rects[HORIZONTAL_FLIP].x = flipped_x * numerator / denominator;
    src_rects[HORIZONTAL_FLIP].y = surface.get_y() * numerator / denominator;
    src_rects[HORIZONTAL_FLIP].w = surface.get_width() * numerator / denominator;
    src_rects[HORIZONTAL_FLIP].h = surface.get_height() * numerator / denominator;

    int flipped_y = surface.get_texture()->get_texture_height() - surface.get_y() - surface.get_height();
    src_rects[VERTICAL_FLIP].x = flipped_y * numerator / denominator;
    src_rects[VERTICAL_FLIP].y = surface.get_y() * numerator / denominator;
    src_rects[VERTICAL_FLIP].w = surface.get_width() * numerator / denominator;
    src_rects[VERTICAL_FLIP].h = surface.get_height() * numerator / denominator;
  }

  SDL_Rect *get_src_rect(DrawingEffect effect)
  {
    return src_rects + effect;
  }
};

#endif

/* EOF */
