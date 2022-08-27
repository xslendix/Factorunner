#ifdef PSP
#include <pspctrl.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspdisplay.h>
#include <psppower.h>
#include <psprtc.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

#ifdef PSP
#include "glib2d.h"
#include "intraFont.h"
#endif

#include "sys/time.h"
#include "utils.h"
#include "gr.h"
#include "tex.h"
#include "audio.h"
#include "input.h"

#ifdef PSP
PSP_MODULE_INFO("Factorunner", 0, 0, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);
#endif

#define COLS              (30)
#define ROWS              (17)

#define MAP_WIDTH         (240)
#define MAP_HEIGHT        (136)

#define PLAYER_SPEED      (1.3)
#define PLAYER_FALL_SPEED (0.3)
#define PLAYER_JUMP_SPEED (3.5)

#define SCREEN_MENU       (0)
#define SCREEN_GAME       (1)

int soff_x = 0,
    soff_y = 2,
    shake = 0;

uint level_no = 0,
     t = 0;

long diff = 0, best_time = 0;

int least_deaths = -1;

u8 *map = NULL,
    finished_game = 0,
    screen = 0,
    paused = 0;

struct timeval initial_time, now = {0, 0};

void
panic(char *msg) {
  printf("[PANIC] %s\n", msg);
  exit(1);
}

enum player_sprite {
  RUN_LEFT_1 = 258,
  RUN_LEFT_2,
  RUN_RIGHT_1 = 261,
  RUN_RIGHT_2,
  IDLE_1,
  IDLE_2
};

enum player_orientation {
  ORIENTATION_NONE = 0,
  ORIENTATION_LEFT,
  ORIENTATION_RIGHT
};

typedef struct player {
  float x, y, vx, vy;
  int coins;
  uint deaths;
  u8 grounded, cframe, orientation;
} player_t;

player_t p = {
  16,SCREEN_WIDTH-24,0,0,
  0,
  0,
  0,0,0
};

vec2lst_t coin_locs = { 0, NULL };

void update_player(float dt, uchar *map_data);

uint
get_level_coins(uint level)
{
  switch (level)
  {
  case 0: return 3;
  case 1: return 6;
  case 2: return 10;
  case 3: return 11;
  case 4: return 14;
  case 5: return 10;
  case 6: return 10;
  }
  return 0;
}

uchar *
load_map(void)
{
  FILE *fp;
  int res, size;
  u8 *buf;

  fp = fopen("assets/map.map", "rb");
  if (fp == NULL)
    panic("Cannot open map file!");

  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  rewind(fp);

  printf("Map size: %d\n", size);

  buf = calloc(1, sizeof(u8) * size);

  res = fread(buf, 1, size, fp);
  if (res != size)
    panic("Cannot read map file!");

  fclose(fp);
  return buf;
}

u8
get_map(u8 *map_data, uint x, int y)
{
  if (y < 0) return 0;
  return map_data[y * MAP_WIDTH + x];
}

void
set_map(u8 *map_data, uint x, uint y, u8 id)
{
  map_data[y * MAP_WIDTH + x] = id;
}

void
reset_coins(u8 *map_data, vec2lst_t *lst)
{
  int i;
  for (i = 0; i < lst->cnt; i++) {
    set_map(map_data, lst->data[i].x, lst->data[i].y, 59);
  }
  clear_vec2lst(lst);
}

void
draw_map(u8 *map_data, uint offx, uint offy, uint w, uint h)
{
  uint x, y;
  u8 tile;

  if (map_data == NULL) return;

  for (y = offy; y < h+offy; y++) {
    for (x = offx; x < w+offx; x++) {
      tile = get_map(map_data, x, y);
      if (tile == 0) continue;

      if (tile == 59 && t % 40 <= 20 && !paused) {
	tile = 60;
      }
      draw_tile((x-offx)*8*GAME_SCALE+soff_x, (y-offy)*8*GAME_SCALE+soff_y, GAME_SCALE, tile);
    }
  }
}

void
check_finish()
{
  if (!finished_game && level_no == 7) {
    finished_game = true;
  }
}

void
shake_screen(int amount)
{
  shake = amount;
}

void
die(int flag, int silent)
{
  if (flag) {
    // TODO: Implement
    // make_blood_ps(p.x, p.y);
    p.deaths++;
  }

  p.x = 16;
  p.y = SCREEN_HEIGHT-24;
  reset_coins(map, &coin_locs);
  p.coins = 0;

  if (flag) {
    shake_screen(8);

    if (!silent) audio_sfx_play(0);
  } else {
    if (!silent) audio_sfx_play(1);
  }
}

void
next_level()
{
  reset_coins(map, &coin_locs);
  level_no++;
  p.coins = 0;
  die(1, 0);
}


char
solid(u8 *map_data, float x, float y)
{
  return get_map(map_data, floor(x/8)+level_no*30, floor(y/8)) < 3;
}

void
collect_coin(uint x, uint y)
{
  p.coins++;
  push_vec2(&coin_locs, (vec2_t){ x, y });
  set_map(map, x, y, 4);
}

char const *pause_options[] = {
  "Resume",
  "Restart",
  "Back to menu"
};

int pause_option = 0;

void
update_pause()
{
  // FIXME: When resumed, add to initial time the amount of time spent in the pause menu!
  i8 inp_pause = get_input_pressed("menu_y");
  if (inp_pause < 0)
    if (pause_option > 0) pause_option--;

  if (inp_pause > 0)
    if (pause_option < 2) pause_option++;

  if (get_input_pressed("select")) {
    audio_sfx_play(3);

    switch (pause_option) {
    case 0:
      paused = !paused;
      break;
    // FIXME: Repeating, repeating, repeating...
    case 1:
      screen = 1;
      p.deaths = 0;
      diff = 0;
      paused = 0;
      level_no = 0;
      finished_game = 0;
      now.tv_sec = 0;
      die(0, 1);
      gettimeofday(&initial_time, NULL);
      break;
    case 2:
      screen = 0;
      p.deaths = 0;
      diff = 0;
      paused = 0;
      level_no = 0;
      finished_game = 0;
      now.tv_sec = 0;
      die(0, 1);
      gettimeofday(&initial_time, NULL);
      break;
    }
  }
}

void
draw_pause()
{
  int i;
  struct timeval now2;
  
#ifdef PSP
  g2dBeginRects(NULL);
  {
    g2dSetScaleWH(SCREEN_WIDTH*GAME_SCALE, SCREEN_HEIGHT*GAME_SCALE);
    g2dSetCoordXY(0, 0);
    g2dSetColor(BLACK);
    g2dSetAlpha(210);
    g2dAdd();
  }
  g2dEnd();
#endif

#ifdef RAYLIB
  DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0, 0, 0, 210});
#endif

  font_set_style(.5, WHITE, TEXT_ALIGN_CENTER);
  draw_text((SCREEN_WIDTH*GAME_SCALE)/2., (SCREEN_HEIGHT*GAME_SCALE)/6., "GAME PAUSED");

  for (i = 0; i < 3; i++) {
    font_set_style(0.5, pause_option == i ? WHITE : GRAY, TEXT_ALIGN_CENTER);
    draw_text((SCREEN_WIDTH*GAME_SCALE)/2., i * 10 * GAME_SCALE + (SCREEN_HEIGHT*GAME_SCALE)/2.25, (char *) pause_options[i]);
  }

  font_set_style(0.5, WHITE, TEXT_ALIGN_LEFT);
  draw_text(2*GAME_SCALE, (SCREEN_HEIGHT-2)*GAME_SCALE, " Select");

  font_set_style(0.5, GREEN, TEXT_ALIGN_LEFT);
  draw_text(2*GAME_SCALE, (SCREEN_HEIGHT-2)*GAME_SCALE, "X");

  // TODO: Remove this
  gettimeofday(&now2, NULL);
  diff = (((long long)now2.tv_sec)*1000)+(now2.tv_usec/1000) - (((long long)initial_time.tv_sec)*1000)+(initial_time.tv_usec/1000);
  char *txt = "%d:%d.%d";
  font_set_style(0.5, WHITE, TEXT_ALIGN_CENTER);
  draw_text((SCREEN_WIDTH*GAME_SCALE)/2., (SCREEN_HEIGHT*GAME_SCALE) - (SCREEN_HEIGHT*GAME_SCALE)/6. , txt, diff/60000, (diff / 1000)%60, diff % 1000);
}

void
update_game(float dt)
{
  if (get_input_pressed("back") && level_no != 7) {
    paused = !paused;

    if (paused)
      pause_option = 0;
  }

  if (paused) {
    update_pause();
    return;
  }

  // TODO: Add particles
  update_player(dt, map);
  check_finish();

  uint x = round((double)(p.x/8)) + level_no*30,
       y = round((double)(p.y/8)),
       tile_id = get_map(map, x, y);

  switch (tile_id) {
  case 57:
    if (p.coins == get_level_coins(level_no))
	next_level();
    break;
  case 59:
  case 60:
    collect_coin(x, y);
    sfx_level = 0.5;
    audio_sfx_play(2);
    sfx_level = 1;
    break;
  case 65:
  case 66:
  case 67:
  case 68:
  case 71:
  case 72:
  case 73:
  case 74:
    // nice
    if (tile_id < 69)
      set_map(map, x, y, tile_id+6);
    p.vx = p.vy = 0;
    die(1, 0);
  }

  if (finished_game) {
    if (is_button_pressed(PSP_CTRL_CIRCLE)) {
      if (diff < best_time || best_time == 0)
	best_time = diff;
      if (p.deaths < least_deaths || least_deaths == -1)
	least_deaths = p.deaths;

      FILE *fp = fopen("save.dat", "w+");
      fprintf(fp, "%ld,%ld,%d,%d", diff, best_time, p.deaths, least_deaths);
      fclose(fp);

      screen = 0;
      level_no = 0;
      finished_game = 0;
      now.tv_sec = 0;
      die(0, 1);
    }
  }
}

void
update_player(float dt, u8 *map_data)
{
  if (p.y > 136 || p.x > 240 || p.x < 0) {
    die(0, 0);
  }

  if (get_input("move") < 0.) {
    p.vx = -PLAYER_SPEED;
    p.orientation = ORIENTATION_LEFT;
  } else if (get_input("move") > 0.) {
    p.vx = PLAYER_SPEED;
    p.orientation = ORIENTATION_RIGHT;
  } else {
    p.vx = 0;
    p.orientation = ORIENTATION_NONE;
  }

  if (solid(map_data, p.x+p.vx,   p.y+p.vy  ) ||
      solid(map_data, p.x+p.vx+7, p.y+p.vy  ) ||
      solid(map_data, p.x+p.vx,   p.y+p.vy+7) ||
      solid(map_data, p.x+p.vx+7, p.y+p.vy+7)) {
    p.vx=0;
    p.orientation = ORIENTATION_NONE;
  }

  if (solid(map_data, p.x, p.y+p.vy+8) || solid(map_data, p.x+7,p.y+p.vy+8)) {
    p.vy = 0;
    p.grounded = 1;
  } else {
    p.vy += PLAYER_FALL_SPEED;
    p.grounded = 0;
  }

  if (p.vy == 0 && get_input("jump")) {
    p.vy -= PLAYER_JUMP_SPEED;
  }

  if (p.vy < 0 && (solid(map_data, p.x+p.vx, p.y+p.vy) || solid(map_data, p.x+p.vy+7, p.y+p.vy))) {
    p.vy = 0;
    p.grounded = 1;
  }

  p.x += p.vx*dt;
  p.y += p.vy*dt;
}

int
get_sprite_player()
{
  int ret;

  switch (p.orientation) {
  case ORIENTATION_LEFT:
    ret = p.cframe == 1 && p.grounded && !paused ? RUN_LEFT_2  : RUN_LEFT_1;
    break;
  case ORIENTATION_RIGHT:
    ret = p.cframe == 1 && p.grounded && !paused ? RUN_RIGHT_2 : RUN_RIGHT_1;
    break;
  case ORIENTATION_NONE:
  default:
    ret = p.cframe == 1 && p.grounded && !paused ? IDLE_2      : IDLE_1;
  }

  if (t % 20 == 0) {
    p.cframe = !p.cframe;
  }

  return ret;
}

void
render_player(void)
{
  draw_sprite(p.x*GAME_SCALE, p.y*GAME_SCALE, GAME_SCALE, get_sprite_player());
}

void
draw_game(void)
{
  if (shake) {
    soff_x = random_range(-4, 4);
    soff_y = random_range(-4, 4);
    shake--;
    draw_rectangle(0, 0, 480, 16, G2D_RGBA(0x18, 0x18, 0x18, 0xff));
    draw_rectangle(0, 16, 16, 272, G2D_RGBA(0x18, 0x18, 0x18, 0xff));
  } else {
    soff_x = soff_y = 0;
  }

  draw_map(map, level_no*30, 0, 30, 17);

  check_finish();

  render_player();

  // TODO: Implement
  // draw_psystem();

  if (finished_game) {
    if (now.tv_sec == 0)
	gettimeofday(&now, NULL);
    diff = (((long long)now.tv_sec)*1000)+(now.tv_usec/1000) - (((long long)initial_time.tv_sec)*1000)+(initial_time.tv_usec/1000);
#ifdef PSP
    char *txt = "GAME COMPLETE!\n\n\nYour time is: %d:%d.%d\n\nDeaths: %d\n\nPress O to save and exit";
#else
    char *txt = "     GAME COMPLETE!\n\n  Your time is: %d:%d.%d\n        Deaths: %d\nPress ESC to save and exit";
#endif
    font_set_style(0.5, WHITE, TEXT_ALIGN_CENTER);
    draw_text((SCREEN_WIDTH*GAME_SCALE)/2., ((SCREEN_HEIGHT-10)*GAME_SCALE)/2., txt,
	      diff/60000, (diff / 1000)%60, diff % 1000, p.deaths);
  }

  if (paused) {
    draw_pause();
  }
}

char const *menu_items[] = {
  "Play Game",
  "Scores",
  "Exit"
};
uint menu_item = 0;

void
update_menu(float dt)
{
  gettimeofday(&initial_time, NULL);

  if (get_input_pressed("select")) {
    audio_sfx_play(3);

    switch (menu_item) {
    case 0:
      screen = 1;
      p.deaths = 0;
      diff = 0;
      paused = 0;
      break;
    case 1:
      {
        FILE *fp = fopen("save.dat", "r");
        if (fp == NULL) break;
        fscanf(fp, "%ld,%ld,%d,%d", &diff, &best_time, &p.deaths, &least_deaths);
        fclose(fp);
	screen=2;
        break;
      }
    case 2:
      running = 0;
      break;
    }
  }

  i8 inp_menu = get_input_pressed("menu_y");
  if (inp_menu < 0)
    if (menu_item > 0) menu_item--;

  if (inp_menu > 0)
    if (menu_item < 2) menu_item++;
}

void
draw_menu(float dt)
{
  int i;
#ifdef PSP
  g2dClear(0xFF1C0C14);
#endif

#ifdef RAYLIB
  ClearBackground(GetColor(0x140C1CFF));
#endif

  draw_map(map, 210, 119, 30, 17);

  for (i = 0; i < 3; i++) {
    font_set_style(0.5, menu_item == i ? WHITE : GRAY, TEXT_ALIGN_CENTER);
    draw_text((SCREEN_WIDTH*GAME_SCALE)/2., i * 10 * GAME_SCALE + (SCREEN_HEIGHT*GAME_SCALE)/2., (char *) menu_items[i]);
  }

  font_set_style(0.5, WHITE, TEXT_ALIGN_LEFT);
  draw_text(2*GAME_SCALE, (SCREEN_HEIGHT-2)*GAME_SCALE, " Select");

  font_set_style(0.5, GREEN, TEXT_ALIGN_LEFT);
  draw_text(2*GAME_SCALE, (SCREEN_HEIGHT-2)*GAME_SCALE, "X");

  font_set_style(0.5, WHITE, TEXT_ALIGN_RIGHT);
  draw_text((SCREEN_WIDTH-2)*GAME_SCALE, (SCREEN_HEIGHT-2)*GAME_SCALE, "Made by xSlendiX");
}

void
update_stats(float dt)
{
  if (get_input_pressed("back")) {
    audio_sfx_play(3);
    screen = 0;
  }

  if (get_input_pressed("select")) {
#ifdef PSP
    sceIoRemove("./save.dat");
#else
    remove("./save.dat");
#endif

    best_time = least_deaths = p.deaths = diff = 0;
  }
}

void
draw_stats(void)
{
#ifdef PSP
  g2dClear(0xFF1C0C14);
#endif

#ifdef RAYLIB
  ClearBackground(GetColor(0x140C1CFF));
#endif

  font_set_style(0.5, WHITE, TEXT_ALIGN_CENTER);
  draw_text((SCREEN_WIDTH*GAME_SCALE)/2., (SCREEN_HEIGHT*GAME_SCALE)/2.-40,
#ifdef PSP
	    "Statistics\n\n\nBest time: %d:%d.%d\n\nLatest time: %d:%d.%d\n\nLatest deaths: %d\n\nLeast deaths: %d",
#else
	    "Statistics\n\nBest time: %d:%d.%d\nLatest time: %d:%d.%d\nLatest deaths: %d\nLeast deaths: %d",
#endif
	    best_time/60000, (best_time / 1000)%60, best_time % 1000,
	    diff/60000, (diff / 1000)%60, diff % 1000,
	    p.deaths, least_deaths);

  font_set_style(0.5, WHITE, TEXT_ALIGN_LEFT);
  draw_text(2*GAME_SCALE, (SCREEN_HEIGHT-2)*GAME_SCALE, " Reset  Go back");

  font_set_style(0.5, RED, TEXT_ALIGN_LEFT);
  draw_text(2*GAME_SCALE, (SCREEN_HEIGHT-2)*GAME_SCALE, "X");

  font_set_style(0.5, GREEN, TEXT_ALIGN_LEFT);
  draw_text(2*GAME_SCALE, (SCREEN_HEIGHT-2)*GAME_SCALE, "       O");
}

int
render_loop(void)
{
  int ret = 0;
  float delta = 0;

#ifdef PSP
  float res = sceRtcGetTickResolution() / 60.f;
  u64 ltick = 0, ctick = 0;
  sceRtcGetCurrentTick(&ltick);
#endif

  gettimeofday(&initial_time, NULL);

  FILE *fp = fopen("save.dat", "r");
  if (fp != NULL) {
    fscanf(fp, "%ld,%ld,%d,%d", &diff, &best_time, &p.deaths, &least_deaths);
    fclose(fp);
  }

  while (running)
  {
    #ifdef PSP
    sceRtcGetCurrentTick(&ctick);

    delta = (ctick - ltick) / res;
    ltick = ctick;

    if (delta < 0.001f)
      delta = 0x001f;

    sceCtrlReadBufferPositive(&pad, 1);

    g2dClear(BLACK);
    #endif

    #ifdef RAYLIB
    delta = GetFrameTime() * 60;
    if (WindowShouldClose()) {
      running = 0;
    }
    #endif

    switch (screen) {
    case SCREEN_MENU: update_menu(delta); break;
    case SCREEN_GAME: update_game(delta); break;
    case 2:           update_stats(delta); break;
    }

    #ifdef RAYLIB
    BeginDrawing();
    #endif

    switch (screen) {
    case SCREEN_MENU: draw_menu(delta); break;
    case SCREEN_GAME: draw_game(); break;
    case 2:           draw_stats(); break;
    }

#ifdef PSP
    pad_prev = pad;

    g2dFlip(G2D_VSYNC);
#endif

#if RAYLIB
    EndDrawing();
#endif

    t++;
  }

  return ret;
}

int
main(void)
{
#ifdef PSP
  setup_callbacks();

  scePowerUnlock(0);
  scePowerSetClockFrequency(333, 333, 166);
#endif

  init();

  map = load_map();

  render_loop();
  free(map);

  quit();

  return 0;
}

