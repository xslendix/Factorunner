#pragma once

#ifdef PSP
#include <pspctrl.h>
#include <pspkernel.h>
#endif

#ifdef RAYLIB
#include <stdint.h>

typedef uint8_t u8;
#endif
typedef int8_t  i8;

#define SCREEN_WIDTH  (240)
#define SCREEN_HEIGHT (136)

#define FAILED(x)  ((x) < 0)
#define SUCCESS(x) ((x) >= 0)

typedef unsigned int uint;
typedef unsigned char uchar;

typedef struct vec2 {
  float x, y;
} vec2_t;

typedef struct vec2lst {
  uint cnt;
  vec2_t *data;
} vec2lst_t;

void push_vec2(vec2lst_t *lst, vec2_t vec);
void pop_vec2(vec2lst_t *lst);
void clear_vec2lst(vec2lst_t *lst);
u8 in_vec2lst(vec2lst_t *lst, vec2_t vec);

double random_range(double min, double max);

int init(void);
void quit(void);

#ifdef PSP
int setup_callbacks(void);

int is_button_pressed(enum PspCtrlButtons buttons);
#define is_button_down(x) (pad.Buttons & x)
#define get_pad_x() pad.Lx

#endif

#ifdef RAYLIB
#define is_button_pressed(x) IsKeyPressed(x)
#define is_button_down(x) IsKeyDown(x)
// FIXME: Add gamepad support
#define get_pad_x() 127

#define PSP_CTRL_CROSS  KEY_ENTER
#define PSP_CTRL_CIRCLE KEY_ESCAPE
#define PSP_CTRL_LEFT   KEY_LEFT
#define PSP_CTRL_RIGHT  KEY_RIGHT
#define PSP_CTRL_UP     KEY_UP
#define PSP_CTRL_DOWN   KEY_DOWN
#define PSP_CTRL_START  KEY_P

#define G2D_RGBA(a, b, c, d) (Color){a, b, c, d}
#endif

extern int running;
#ifdef PSP
extern SceCtrlData pad, pad_prev;
#endif

extern int GAME_SCALE;

