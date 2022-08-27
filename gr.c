#include "gr.h"

#ifdef PSP
#include <pspkernel.h>
#include <pspgu.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "utils.h"

#ifdef PSP
#include "glib2d.h"
#include "png.h"
#include "intraFont.h"

intraFont *font;
#endif

#ifdef RAYLIB
struct {
  float size;
  Color color;
  u8 direction;
} font_options;

Font *font;
#endif

void
draw_rectangle(float x, float y, float width, float height, color_t color)
{
#ifdef PSP
  g2dBeginRects(NULL);
  {
    g2dSetCoordXY(x, y);
    g2dSetScaleWH(width, height);
    g2dSetColor(color);
    g2dAdd();
  }
  g2dEnd();
#endif

#ifdef RAYLIB
  DrawRectangle(x, y, width, height, color);
#endif
}

void
draw_image(texture *tex, float x, float y)
{
#ifdef PSP
  g2dBeginRects(tex);
  {
    g2dSetCoordXY(x, y);
    g2dAdd();
  }
  g2dEnd();
#endif

#ifdef RAYLIB
  DrawTexture(*tex, x, y, WHITE);
#endif
}

void
draw_image_ex(texture *tex, float x, float y, float w, float h)
{
#ifdef PSP
  g2dBeginRects(tex);
  {
    g2dSetCoordXY(x, y);
    g2dSetScaleWH(w, h);
    g2dAdd();
  }
  g2dEnd();
#endif

#ifdef RAYLIB
  // FIXME: Add implementation
#endif
}

inline void
draw_image_pro(texture *tex, float x, float y, float w, float h, float cx, float cy, float cw, float ch)
{
#ifdef PSP
  g2dBeginRects(tex);
  {
    g2dSetCoordXY(x, y);
    g2dSetScaleWH(w, h);
    g2dSetCropXY(cx, cy);
    g2dSetCropWH(cw, ch);
    g2dAdd();
  }
  g2dEnd();
#endif

#ifdef RAYLIB
  DrawTexturePro(*tex, (Rectangle){ cx, cy, cw, ch }, (Rectangle){ x, y, w, h }, (Vector2){ 0, 0 }, 0, WHITE);
#endif
}

float
draw_text(float x, float y, char *text, ...)
{
  float ret;
  int cnt;
  char *buf;
  va_list va;

  va_start(va, text);
  cnt = strlen(text) + 50;
  buf = malloc(sizeof(char) * cnt);
  vsprintf(buf, text, va);
#ifdef PSP
  ret = intraFontPrint(font, x, y, buf);
#endif

#ifdef RAYLIB
  ret = MeasureTextEx(*font, buf, font_options.size, 0).x;
  int xoff = x;
  switch (font_options.direction) {
  case TEXT_ALIGN_RIGHT:
    xoff -= ret;
    break;
  case TEXT_ALIGN_CENTER:
    xoff -= ret/2;
    break;
  }
  DrawTextEx(*font, buf, (Vector2){ xoff, y-font_options.size }, font_options.size, 0, font_options.color);
#endif
  free(buf);
  va_end(va);

  return ret;
}

void
font_init(void)
{
#ifdef PSP
  intraFontInit();
  font = intraFontLoad("assets/kongtext.pgf", 0);
#endif

#ifdef RAYLIB
  font_t *fnt = malloc(sizeof(font_t));
  *fnt = LoadFont("assets/kongtext.ttf");
  font = fnt;
#endif
}

void
font_free(void)
{
#ifdef PSP
  intraFontUnload(font);
#endif

#ifdef RAYLIB
  UnloadFont(*font);
  free(font);
#endif
}

void
font_set_style(float size, color_t color, unsigned int options)
{
#ifdef PSP
  intraFontSetStyle(font, size, color, 0, 0, options);
#endif

#ifdef RAYLIB
  font_options.size = size*10*GAME_SCALE;
  font_options.color = color;
  font_options.direction = options;
#endif
}

float
font_get_height(void)
{
  // FIXME: This may not work properly.
  //        Not used anyways, so doesn't matter that much.
#ifdef PSP
  return 0;
#endif

#ifdef RAYLIB
  return font_options.size;
#endif
}

texture *
load_texture_from_png(char *path)
{
#ifdef PSP
  FILE *fp;
  png_structp png_ptr;
  png_infop info_ptr;
  unsigned int sig_read = 0;
  png_uint_32 width, height;
  int bit_depth, color_type, interlace_type;
  u32 x, y;
  g2dColor *line;
  texture *tex;

  fp = fopen(path, "rb");

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  png_set_error_fn(png_ptr, NULL, NULL, NULL);
  info_ptr = png_create_info_struct(png_ptr);
  png_init_io(png_ptr, fp);
  png_set_sig_bytes(png_ptr, sig_read);
  png_read_info(png_ptr, info_ptr);
  png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, 
               &interlace_type, NULL, NULL);
  png_set_strip_16(png_ptr);
  png_set_packing(png_ptr);

  if (color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png_ptr);
  if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(png_ptr);

  png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
  
  tex = (texture *) g2dTexCreate(width, height);
  line = malloc(width * 4);

  for (y = 0; y < height; y++)
  {
    png_read_row(png_ptr, (u8*) line, NULL);

    for (x = 0; x < width; x++)
      tex->data[x + y*tex->tw] = line[x];
  }

  free(line);
  png_read_end(png_ptr, info_ptr);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

  fclose(fp);
#endif

#ifdef RAYLIB
  Texture2D *tex = malloc(sizeof(Texture2D));
  *tex = LoadTexture(path);
#endif

  return tex;
}

