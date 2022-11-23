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

#ifndef _SDL_mutex_h
#define _SDL_mutex_h

#include "SDL_stdinc.h"
#include "SDL_error.h"

#include "begin_code.h"

typedef struct SDL_mutex SDL_mutex;
typedef struct SDL_semaphore SDL_sem;
typedef struct SDL_cond SDL_cond;

extern DECLSPEC SDL_mutex * SDLCALL SDL_CreateMutex(void);
extern DECLSPEC int SDLCALL SDL_mutexP(SDL_mutex *mutex);  /* lock */
extern DECLSPEC int SDLCALL SDL_mutexV(SDL_mutex *mutex);  /* unlock */
extern DECLSPEC void SDLCALL SDL_DestroyMutex(SDL_mutex *mutex);

extern DECLSPEC SDL_sem * SDLCALL SDL_CreateSemaphore(Uint32 initial_value);
extern DECLSPEC int SDLCALL SDL_SemPost(SDL_sem *sem);
extern DECLSPEC int SDLCALL SDL_SemWait(SDL_sem *sem);
extern DECLSPEC int SDLCALL SDL_SemWaitTimeout(SDL_sem *sem, Uint32 ms);
extern DECLSPEC int SDLCALL SDL_SemTryWait(SDL_sem *sem);
extern DECLSPEC Uint32 SDLCALL SDL_SemValue(SDL_sem *sem);
extern DECLSPEC void SDLCALL SDL_DestroySemaphore(SDL_sem *sem);

extern DECLSPEC SDL_cond * SDLCALL SDL_CreateCond(void);
extern DECLSPEC int SDLCALL SDL_CondSignal(SDL_cond *cond);
extern DECLSPEC int SDLCALL SDL_CondBroadcast(SDL_cond *cond);
extern DECLSPEC int SDLCALL SDL_CondWait(SDL_cond *cond, SDL_mutex *mut);
extern DECLSPEC int SDLCALL SDL_CondWaitTimeout(SDL_cond *cond, SDL_mutex *mutex, Uint32 ms);
extern DECLSPEC void SDLCALL SDL_DestroyCond(SDL_cond *cond);

#define SDL_MUTEX_TIMEDOUT 1
#define SDL_MUTEX_MAXWAIT (~(Uint32)0)
#define SDL_LockMutex(m) SDL_mutexP(m)
#define SDL_UnlockMutex(m) SDL_mutexV(m)

#include "close_code.h"

#endif

