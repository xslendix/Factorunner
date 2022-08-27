#pragma once

#include "utils.h"

#ifdef PSP
#include "glib2d.h"
#endif

#ifdef RAYLIB
#include <raylib.h>
#endif

#include "gr.h"

extern texture *sprites, *tiles;

int tex_load(void);
void tex_free(void);

void draw_tile(float x, float y, float scale, int idx);
void draw_sprite(float x, float y, float scale, int idx);

