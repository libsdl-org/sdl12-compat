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


#ifndef _SDL_keyboard_h
#define _SDL_keyboard_h

#include "SDL_stdinc.h"
#include "SDL_error.h"
#include "SDL_keysym.h"

#include "begin_code.h"

typedef struct SDL_keysym
{
    Uint8 scancode;
    SDLKey sym;
    SDLMod mod;
    Uint16 unicode;
} SDL_keysym;

#define SDL_ALL_HOTKEYS 0xFFFFFFFF
#define SDL_DEFAULT_REPEAT_DELAY 500
#define SDL_DEFAULT_REPEAT_INTERVAL 30

extern DECLSPEC Uint8 * SDLCALL SDL_GetKeyState(int *numkeys);
extern DECLSPEC SDLMod SDLCALL SDL_GetModState(void);
extern DECLSPEC void SDLCALL SDL_SetModState(SDLMod modstate);
extern DECLSPEC char * SDLCALL SDL_GetKeyName(SDLKey key);
extern DECLSPEC int SDLCALL SDL_EnableKeyRepeat(int delay, int interval);
extern DECLSPEC void SDLCALL SDL_GetKeyRepeat(int *delay, int *interval);
extern DECLSPEC int SDLCALL SDL_EnableUNICODE(int enable);

#include "close_code.h"

#endif

