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

#ifndef _SDL_thread_h
#define _SDL_thread_h

#include "SDL_stdinc.h"
#include "SDL_error.h"
#include "SDL_mutex.h"

#ifdef __OS2__ /* for _beginthread() and _endthread(). */
#include <process.h>
#ifdef __EMX__
#include <stdlib.h>
#endif
#endif

#include "begin_code.h"

typedef struct SDL_Thread SDL_Thread;

/* No _beginthread()/_endthread() magic for windows -- only for os2:
 * Official Windows versions of SDL-1.2 >= 1.2.10 were always built
 * with HAVE_LIBC, i.e.: *without* SDL_PASSED_BEGINTHREAD_ENDTHREAD
 * defined, in order to keep binary compatibility with SDL <= 1.2.9. */
#ifdef __OS2__
typedef int (*pfnSDL_CurrentBeginThread)(void (*func)(void *), void *, unsigned, void *arg);
typedef void (*pfnSDL_CurrentEndThread)(void);
extern DECLSPEC SDL_Thread * SDLCALL SDL_CreateThread(int (SDLCALL *fn)(void *), void *data, pfnSDL_CurrentBeginThread pfnBeginThread, pfnSDL_CurrentEndThread pfnEndThread);
#define SDL_PASSED_BEGINTHREAD_ENDTHREAD
#define SDL_CreateThread(fn, data) SDL_CreateThread(fn, data, _beginthread, _endthread)
#else
extern DECLSPEC SDL_Thread * SDLCALL SDL_CreateThread(int (SDLCALL *fn)(void *), void *data);
#endif
extern DECLSPEC Uint32 SDLCALL SDL_ThreadID(void);
extern DECLSPEC Uint32 SDLCALL SDL_GetThreadID(SDL_Thread *thread);
extern DECLSPEC void SDLCALL SDL_WaitThread(SDL_Thread *thread, int *status);
extern DECLSPEC void SDLCALL SDL_KillThread(SDL_Thread *thread);

#include "close_code.h"

#endif
