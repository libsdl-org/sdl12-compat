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

#ifndef _SDL_timer_h
#define _SDL_timer_h

#include "SDL_stdinc.h"
#include "SDL_error.h"

#include "begin_code.h"

typedef struct _SDL_TimerID *SDL_TimerID;
typedef Uint32 (SDLCALL *SDL_TimerCallback)(Uint32 interval);
typedef Uint32 (SDLCALL *SDL_NewTimerCallback)(Uint32 interval, void *param);

extern DECLSPEC Uint32 SDLCALL SDL_GetTicks(void);
extern DECLSPEC void SDLCALL SDL_Delay(Uint32 ms);

extern DECLSPEC int SDLCALL SDL_SetTimer(Uint32 interval, SDL_TimerCallback callback);
extern DECLSPEC SDL_TimerID SDLCALL SDL_AddTimer(Uint32 interval, SDL_NewTimerCallback callback, void *param);
extern DECLSPEC SDL_bool SDLCALL SDL_RemoveTimer(SDL_TimerID t);

#define SDL_TIMESLICE 10
#define TIMER_RESOLUTION 10

#include "close_code.h"

#endif

