#include "input.h"

#include <stdlib.h>
#include <string.h>

#include "utils.h"

keybind_t keybindings[] = {
  { "jump",   AXIS_NULL,   GAMEPAD_A    | GAMEPAD_CR,    { K_SPACE,  K_UP, K_Z, K_NULL } },
  { "move",   AXIS_LEFT_X, GAMEPAD_LEFT | GAMEPAD_RIGHT, { K_LEFT,   K_RIGHT,   K_NULL } },
  { "select", AXIS_NULL,   GAMEPAD_A    | GAMEPAD_CR,    { K_ENTER,             K_NULL } },
  { "back",   AXIS_NULL,   GAMEPAD_B    | GAMEPAD_CI,    { K_ESCAPE, K_P,       K_NULL } },
  { "menu_y", AXIS_LEFT_Y, GAMEPAD_UP   | GAMEPAD_DOWN,  { K_UP,     K_DOWN,    K_NULL } },
  { NULL,     AXIS_NULL,   GAMEPAD_NULL,                 { K_NULL                      } }
};

float
get_input(char *id)
{
  float ret = 0;

  keybind_t *keybind = keybindings;
  if (keybind == NULL)
    return ret;

  while (strcmp(keybind->id, id) != 0) {
    keybind++;
    if (keybind == NULL)
      return 0;
  }

  if ((keybind->gamepad_button & GAMEPAD_LEFT) > 0) {
#ifdef PSP
    if (pad.Buttons & PSP_CTRL_LEFT)
      ret = -1;
#endif

#ifdef RAYLIB
    ret = IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT);
#endif
  }

  if ((keybind->gamepad_button & GAMEPAD_RIGHT) > 0) {
#ifdef PSP
    if (pad.Buttons & PSP_CTRL_RIGHT)
      ret = 1;
#endif

#ifdef RAYLIB
    ret = IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT);
#endif
  }

  if ((keybind->gamepad_button & GAMEPAD_UP) > 0) {
#ifdef PSP
    if (pad.Buttons & PSP_CTRL_UP)
      ret = -1;
#endif

#ifdef RAYLIB
    ret = IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP);
#endif
  }

  if ((keybind->gamepad_button & GAMEPAD_DOWN) > 0) {
#ifdef PSP
    if (pad.Buttons & PSP_CTRL_DOWN)
      ret = 1;
#endif

#ifdef RAYLIB
    ret = IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN);
#endif
  }

  if ((keybind->gamepad_button & GAMEPAD_A) > 0 || (keybind->gamepad_button & GAMEPAD_CR) > 0) {
#ifdef PSP
    if (pad.Buttons & PSP_CTRL_CROSS)
      ret = 1;
#endif

#ifdef RAYLIB
    ret = IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
#endif
  }

  if ((keybind->gamepad_button & GAMEPAD_B) > 0 || (keybind->gamepad_button & GAMEPAD_CI) > 0) {
#ifdef PSP
    if (pad.Buttons & PSP_CTRL_CIRCLE)
      ret = 1;
#endif

#ifdef RAYLIB
    ret = IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT);
#endif
  }

#ifdef RAYLIB
  if (ret == 0) {
    for (keyboard_key_t *key = keybind->key_codes; *key != K_NULL; key++) {
      if (ret == 0) {
	ret = IsKeyDown(*key);
	if (*key == K_LEFT) ret = -ret;
	if (*key == K_UP) ret = -ret;
      }
    }
  }
#endif

  if (ret == 0) {
    switch (keybind->axis_id) {
    case AXIS_LEFT_X:
      {
#ifdef PSP
	float val = (pad.Lx - 127)/254.;
        if (val > PAD_SENSITIVITY || val < -PAD_SENSITIVITY) {
	  ret = val;
        }
#endif

#ifdef RAYLIB
	int val = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        if (val > PAD_SENSITIVITY || val < -PAD_SENSITIVITY) {
	  ret = val;
        }
#endif
        break;
      }
    case AXIS_LEFT_Y:
      {
#ifdef PSP
	float val = (pad.Ly - 127)/254.;
        if (val > PAD_SENSITIVITY || val < -PAD_SENSITIVITY) {
	  ret = val;
        }
#endif

#ifdef RAYLIB
	int val = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
        if (val > PAD_SENSITIVITY || val < -PAD_SENSITIVITY) {
	  ret = val;
        }
#endif
        break;
      }
    }
  }

  return ret;
}

i8
get_input_pressed(char *id)
{
  i8 ret = 0;

  // FIXME: Add this to a separate function called `find_keybind`
  keybind_t *keybind = keybindings;
  if (keybind == NULL)
    return ret;

  while (strcmp(keybind->id, id) != 0) {
    keybind++;
    if (keybind == NULL)
      return 0;
  }

  if ((keybind->gamepad_button & GAMEPAD_UP) > 0) {
#ifdef PSP
    if (is_button_pressed(PSP_CTRL_UP))
      ret = -1;
#endif

#ifdef RAYLIB
    ret = -IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_UP);
#endif
  }

  if ((keybind->gamepad_button & GAMEPAD_DOWN) > 0) {
#ifdef PSP
    if (is_button_pressed(PSP_CTRL_DOWN))
      ret = 1;
#endif

#ifdef RAYLIB
    ret = IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN);
#endif
  }

  if ((keybind->gamepad_button & GAMEPAD_A) > 0 || (keybind->gamepad_button & GAMEPAD_CR) > 0) {
#ifdef PSP
    if (is_button_pressed(PSP_CTRL_CROSS))
      ret = 1;
#endif

#ifdef RAYLIB
    ret = IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN);
#endif
  }

  if ((keybind->gamepad_button & GAMEPAD_B) > 0 || (keybind->gamepad_button & GAMEPAD_CI) > 0) {
#ifdef PSP
    if (is_button_pressed(PSP_CTRL_CIRCLE))
      ret = 1;
#endif

#ifdef RAYLIB
    ret = IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT);
#endif
  }

#ifdef RAYLIB
  if (ret == 0) {
    for (keyboard_key_t *key = keybind->key_codes; *key != K_NULL; key++) {
      if (ret == 0) {
	ret = IsKeyPressed(*key);
	if (*key == K_LEFT) ret = -ret;
	if (*key == K_UP)   ret = -ret;
      }
    }
  }
#endif

  return ret;
}

