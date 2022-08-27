#include "utils.h"

#ifdef PSP
#include <pspctrl.h>
#include <pspkernel.h>
#include <pspgu.h>
#endif

#ifdef RAYLIB
#include <raylib.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef PSP
#include "glib2d.h"
#include "intraFont.h"
#endif

#include "gr.h"
#include "tex.h"
#include "audio.h"

int running = 1;
#ifdef PSP
SceCtrlData pad, pad_prev;

int GAME_SCALE = 2;
#else
int GAME_SCALE = 4;
#endif

#ifdef RAYLIB
Image icon;
#endif

#ifdef PSP
static int
exit_callback(int arg1, int arg2, void *common)
{
  running = 0;
  return 0;
}

static int
callback_thread(SceSize args, void *argp)
{
  int callback = sceKernelCreateCallback("exit_callback", exit_callback, NULL);
  sceKernelRegisterExitCallback(callback);
  sceKernelSleepThreadCB();
  return 0;
}
#endif

int
init(void)
{
#ifdef PSP
  sceCtrlSetSamplingCycle(0);
  sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

  g2dInit();

  // FIXME: Set tex filter to linear somehow
#endif

#ifdef RAYLIB
  InitWindow(SCREEN_WIDTH*GAME_SCALE, SCREEN_HEIGHT*GAME_SCALE, "Factorunner");

  icon = LoadImage("assets/icon.png");
  SetWindowIcon(icon);

  SetTargetFPS(60);
  SetExitKey(0);
#endif

  tex_load();
  font_init();
  audio_init();

  return 0;
}

void
quit(void)
{
#ifdef PSP
  g2dTerm();
#endif

#ifdef RAYLIB
  CloseWindow();
  UnloadImage(icon);
#endif

  tex_free();
  font_free();
  audio_free();

#ifdef PSP
  intraFontShutdown();
  sceKernelExitGame();
#endif
}

#ifdef PSP
int
setup_callbacks(void)
{
  int thread = 0;

  if (SUCCESS(thread = sceKernelCreateThread("callback_thread", callback_thread, 0x11, 0xfa0, 0, NULL)))
    sceKernelStartThread(thread, 0, 0);

  return thread;
}
#endif

void
push_vec2(vec2lst_t *lst, vec2_t vec)
{
  if (lst == NULL) return;

  lst->cnt++;

  if (lst->data == NULL) {
    lst->data = malloc(sizeof(vec2_t));
  } else {
    lst->data = realloc(lst->data, lst->cnt * sizeof(vec2_t));
  }

  lst->data[lst->cnt-1].x = vec.x;
  lst->data[lst->cnt-1].y = vec.y;
}

void
pop_vec2(vec2lst_t *lst)
{
  if (lst == NULL) return;
  if (lst->data == NULL) return;
  if (lst->cnt == 0) return;

  memset(&lst->data[--lst->cnt], 0, sizeof(vec2_t));
  lst->data = realloc(lst->data, sizeof(vec2_t)*lst->cnt);
}

void
clear_vec2lst(vec2lst_t *lst)
{
  if (lst == NULL) return;
  if (lst->data != NULL) {
    free(lst->data);
    lst->data = NULL;
  }
  lst->cnt = 0;
}

u8
in_vec2lst(vec2lst_t *lst, vec2_t vec)
{
  int i;
  for (i = 0; i < lst->cnt; i++) {
    if (lst->data[i].x == vec.x && lst->data[i].y == vec.y)
      return 1;
  }
  return 0;
}

double
random_range(double min, double max)
{
  double f = (double)rand() / RAND_MAX;
  return min + f * (max - min);
}

#ifdef PSP
int
is_button_pressed(enum PspCtrlButtons buttons)
{
  return ((pad.Buttons & buttons) == buttons) && ((pad_prev.Buttons & buttons) != buttons);
}
#endif

