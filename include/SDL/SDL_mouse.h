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

#ifndef _SDL_mouse_h
#define _SDL_mouse_h

#include "SDL_stdinc.h"
#include "SDL_error.h"
#include "SDL_video.h"

#include "begin_code.h"

typedef struct WMcursor WMcursor;
typedef struct SDL_Cursor
{
    SDL_Rect area;
    Sint16 hot_x, hot_y;
    Uint8 *data;
    Uint8 *mask;
    Uint8 *save[2];
    WMcursor *wm_cursor;
} SDL_Cursor;

extern DECLSPEC Uint8 SDLCALL SDL_GetMouseState(int *x, int *y);
extern DECLSPEC Uint8 SDLCALL SDL_GetRelativeMouseState(int *x, int *y);
extern DECLSPEC void SDLCALL SDL_WarpMouse(Uint16 x, Uint16 y);
extern DECLSPEC SDL_Cursor * SDLCALL SDL_CreateCursor(Uint8 *data, Uint8 *mask, int w, int h, int hot_x, int hot_y);
extern DECLSPEC void SDLCALL SDL_SetCursor(SDL_Cursor *cursor);
extern DECLSPEC SDL_Cursor * SDLCALL SDL_GetCursor(void);
extern DECLSPEC void SDLCALL SDL_FreeCursor(SDL_Cursor *cursor);
extern DECLSPEC int SDLCALL SDL_ShowCursor(int toggle);

#define SDL_BUTTON(X) (1 << ((X)-1))
#define SDL_BUTTON_LEFT 1
#define SDL_BUTTON_MIDDLE 2
#define SDL_BUTTON_RIGHT 3
#define SDL_BUTTON_WHEELUP 4
#define SDL_BUTTON_WHEELDOWN 5
#define SDL_BUTTON_X1 6
#define SDL_BUTTON_X2 7
#define SDL_BUTTON_LMASK SDL_BUTTON(SDL_BUTTON_LEFT)
#define SDL_BUTTON_MMASK SDL_BUTTON(SDL_BUTTON_MIDDLE)
#define SDL_BUTTON_RMASK SDL_BUTTON(SDL_BUTTON_RIGHT)
#define SDL_BUTTON_X1MASK SDL_BUTTON(SDL_BUTTON_X1)
#define SDL_BUTTON_X2MASK SDL_BUTTON(SDL_BUTTON_X2)

#include "close_code.h"

#endif

