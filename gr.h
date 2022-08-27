#pragma once

#ifdef PSP
#include "glib2d.h"
#include "intraFont.h"

#define TEXT_ALIGN_CENTER INTRAFONT_ALIGN_CENTER
#define TEXT_ALIGN_LEFT   INTRAFONT_ALIGN_LEFT
#define TEXT_ALIGN_RIGHT  INTRAFONT_ALIGN_RIGHT

typedef g2dTexture texture;
typedef g2dColor color_t;
#endif

#ifdef RAYLIB
#include <raylib.h>

#define TEXT_ALIGN_CENTER 0
#define TEXT_ALIGN_LEFT   1
#define TEXT_ALIGN_RIGHT  2

typedef Texture2D texture;
typedef Color color_t;
typedef Font font_t;
#endif

texture *load_texture_from_png(char *path);

void draw_rectangle(float x, float y, float width, float height, color_t color);
void draw_image(texture *tex, float x, float y);
void draw_image_ex(texture *tex, float x, float y, float w, float h);
void draw_image_pro(texture *tex, float x, float y, float w, float h, float cx, float cy, float cw, float ch);
float draw_text(float x, float y, char *text, ...);

void font_set_style(float size, color_t color, unsigned int options);
float font_get_height(void);

void font_init(void);
void font_free(void);

