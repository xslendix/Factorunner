#include "tex.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef PSP
#include "glib2d.h"
#include "libpng16/png.h"
#endif

#include "gr.h"

texture *sprites = NULL, *tiles = NULL;

int
tex_load(void)
{
  int ret = 0;
  
  sprites = load_texture_from_png("assets/sprites.png");
  if (sprites == NULL) {
    puts("[ERROR] Couldn't load sprites!");
    ret = -1;
  }

  tiles = load_texture_from_png("assets/tiles.png");
  if (tiles == NULL) {
    puts("[ERROR] Couldn't load tiles!");
    ret = -1;
  }

  return ret;
}

void
tex_free(void)
{
#ifdef PSP
  g2dTexFree(&sprites);
  g2dTexFree(&tiles);
#endif

#ifdef RAYLIB
  UnloadTexture(*sprites);
  UnloadTexture(*tiles);
  free(sprites);
  free(tiles);
#endif
}

void
draw_image_seg(float x, float y, float scale, int idx, texture *tex)
{
  int cx = idx%16,
      cy = idx/16;

  draw_image_pro(tex, x, y, 8*scale, 8*scale, 8*cx, 8*cy, 8, 8);
}

inline void
draw_tile(float x, float y, float scale, int idx)
{
  draw_image_seg(x, y, scale, idx, tiles);
}

inline void
draw_sprite(float x, float y, float scale, int idx)
{
  idx -= 256;
  draw_image_seg(x, y, scale, idx, sprites);
}

