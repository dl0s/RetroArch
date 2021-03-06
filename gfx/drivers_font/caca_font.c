/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2014 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2016 - Daniel De Matteis
 *  Copyright (C) 2016 - Brad Parker
 * 
 *  RetroArch is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with RetroArch.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string/stdstring.h>
#include <caca.h>

#ifdef HAVE_CONFIG_H
#include "../../config.h"
#endif

#include "../font_driver.h"
#include "../../configuration.h"
#include "../../verbosity.h"
#include "../common/caca_common.h"

typedef struct
{
   const font_renderer_driver_t *caca_font_driver;
   void *caca_font_data;
   caca_t *caca;
} caca_raster_t;

static void *caca_init_font(void *data,
      const char *font_path, float font_size)
{
   caca_raster_t *font  = (caca_raster_t*)calloc(1, sizeof(*font));

   if (!font)
      return NULL;

   font->caca = (caca_t*)data;

   font_size = 1;

   if (!font_renderer_create_default((const void**)&font->caca_font_driver,
            &font->caca_font_data, font_path, font_size))
   {
      RARCH_WARN("Couldn't initialize font renderer.\n");
      return NULL;
   }

   return font;
}

static void caca_render_free_font(void *data)
{

}

static int caca_get_message_width(void *data, const char *msg,
      unsigned msg_len, float scale)
{
   return 0;
}

static const struct font_glyph *caca_font_get_glyph(
      void *data, uint32_t code)
{
   return NULL;
}

static void caca_render_msg(void *data, const char *msg,
      const void *userdata)
{
   caca_raster_t *font = (caca_raster_t*)data;
   float x, y;
   unsigned width, height;
   unsigned newX, newY;
   settings_t *settings = config_get_ptr();
   const struct font_params *params = (const struct font_params*)userdata;

   if (!font || string_is_empty(msg))
      return;

   if (params)
   {
      x = params->x;
      y = params->y;
   }
   else
   {
      x = settings->video.msg_pos_x;
      y = settings->video.msg_pos_y;
   }

   if (!font->caca || !font->caca->caca_cv || !font->caca->caca_display ||
       !*font->caca->caca_cv || !*font->caca->caca_display)
      return;

   width = caca_get_canvas_width(*font->caca->caca_cv);
   height = caca_get_canvas_height(*font->caca->caca_cv);

   newX = x * width;
   newY = height - (y * height);

   if (strlen(msg) + newX > width)
      newX -= strlen(msg) + newX - width;

   caca_put_str(*font->caca->caca_cv, newX, newY, msg);

   caca_refresh_display(*font->caca->caca_display);
}

static void caca_font_flush_block(void* data)
{
   (void)data;
}

static void caca_font_bind_block(void* data, void* userdata)
{
   (void)data;
}

font_renderer_t caca_font = {
   caca_init_font,
   caca_render_free_font,
   caca_render_msg,
   "caca font",
   caca_font_get_glyph,       /* get_glyph */
   caca_font_bind_block,      /* bind_block */
   caca_font_flush_block,     /* flush */
   caca_get_message_width     /* get_message_width */
};
