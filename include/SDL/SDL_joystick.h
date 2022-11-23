/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/* These headers are from sdl12-compat, and are intended to give just enough
functionality to let you build an SDL-1.2-based project without having the
real SDL-1.2 available to you. */

#ifndef _SDL_joystick_h
#define _SDL_joystick_h

#include "SDL_stdinc.h"
#include "SDL_error.h"

#include "begin_code.h"

#define SDL_HAT_CENTERED 0x00
#define SDL_HAT_UP 0x01
#define SDL_HAT_RIGHT 0x02
#define SDL_HAT_DOWN 0x04
#define SDL_HAT_LEFT 0x08
#define SDL_HAT_RIGHTUP 0x03
#define SDL_HAT_RIGHTDOWN 0x06
#define SDL_HAT_LEFTUP 0x09
#define SDL_HAT_LEFTDOWN 0x0C

typedef struct _SDL_Joystick SDL_Joystick;

extern DECLSPEC int SDLCALL SDL_NumJoysticks(void);
extern DECLSPEC const char * SDLCALL SDL_JoystickName(int device_index);
extern DECLSPEC SDL_Joystick * SDLCALL SDL_JoystickOpen(int device_index);
extern DECLSPEC int SDLCALL SDL_JoystickOpened(int device_index);
extern DECLSPEC int SDLCALL SDL_JoystickIndex(SDL_Joystick *joystick);
extern DECLSPEC int SDLCALL SDL_JoystickNumAxes(SDL_Joystick *joystick);
extern DECLSPEC int SDLCALL SDL_JoystickNumBalls(SDL_Joystick *joystick);
extern DECLSPEC int SDLCALL SDL_JoystickNumHats(SDL_Joystick *joystick);
extern DECLSPEC int SDLCALL SDL_JoystickNumButtons(SDL_Joystick *joystick);
extern DECLSPEC void SDLCALL SDL_JoystickUpdate(void);
extern DECLSPEC int SDLCALL SDL_JoystickEventState(int state);
extern DECLSPEC Sint16 SDLCALL SDL_JoystickGetAxis(SDL_Joystick *joystick, int axis);
extern DECLSPEC Uint8 SDLCALL SDL_JoystickGetHat(SDL_Joystick *joystick, int hat);
extern DECLSPEC int SDLCALL SDL_JoystickGetBall(SDL_Joystick *joystick, int ball, int *dx, int *dy);
extern DECLSPEC Uint8 SDLCALL SDL_JoystickGetButton(SDL_Joystick *joystick, int button);
extern DECLSPEC void SDLCALL SDL_JoystickClose(SDL_Joystick *joystick);

#include "close_code.h"

#endif

