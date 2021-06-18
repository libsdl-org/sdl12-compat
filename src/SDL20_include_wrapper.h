/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2021 Sam Lantinga <slouken@libsdl.org>

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

/*
 * This file #includes the proper SDL 2.0 headers, but #defines a whole
 *  bunch of stuff so we don't conflict with symbols we plan to offer with
 *  the 1.2 signatures. Ugly business.
 */

#ifndef _INCL_SDL20_INCLUDE_WRAPPER_H_
#define _INCL_SDL20_INCLUDE_WRAPPER_H_

#define SDL_GetVersion IGNORE_THIS_VERSION_OF_SDL_GetVersion
#define SDL_Log IGNORE_THIS_VERSION_OF_SDL_Log
#define SDL_ReportAssertion IGNORE_THIS_VERSION_OF_SDL_ReportAssertion
#define SDL_Error IGNORE_THIS_VERSION_OF_SDL_Error
#define SDL_SetError IGNORE_THIS_VERSION_OF_SDL_SetError
#define SDL_PollEvent IGNORE_THIS_VERSION_OF_SDL_PollEvent
#define SDL_PushEvent IGNORE_THIS_VERSION_OF_SDL_PushEvent
#define SDL_EventState IGNORE_THIS_VERSION_OF_SDL_EventState
#define SDL_PeepEvents IGNORE_THIS_VERSION_OF_SDL_PeepEvents
#define SDL_WaitEvent IGNORE_THIS_VERSION_OF_SDL_WaitEvent
#define SDL_SetEventFilter IGNORE_THIS_VERSION_OF_SDL_SetEventFilter
#define SDL_GetEventFilter IGNORE_THIS_VERSION_OF_SDL_GetEventFilter
#define SDL_CreateRGBSurface IGNORE_THIS_VERSION_OF_SDL_CreateRGBSurface
#define SDL_CreateRGBSurfaceFrom IGNORE_THIS_VERSION_OF_SDL_CreateRGBSurfaceFrom
#define SDL_FreeSurface IGNORE_THIS_VERSION_OF_SDL_FreeSurface
#define SDL_SetClipRect IGNORE_THIS_VERSION_OF_SDL_SetClipRect
#define SDL_GetClipRect IGNORE_THIS_VERSION_OF_SDL_GetClipRect
#define SDL_FillRect IGNORE_THIS_VERSION_OF_SDL_FillRect
#define SDL_GetRGB IGNORE_THIS_VERSION_OF_SDL_GetRGB
#define SDL_GetRGBA IGNORE_THIS_VERSION_OF_SDL_GetRGBA
#define SDL_MapRGB IGNORE_THIS_VERSION_OF_SDL_MapRGB
#define SDL_MapRGBA IGNORE_THIS_VERSION_OF_SDL_MapRGBA
#define SDL_CreateCursor IGNORE_THIS_VERSION_OF_SDL_CreateCursor
#define SDL_SetCursor IGNORE_THIS_VERSION_OF_SDL_SetCursor
#define SDL_GetCursor IGNORE_THIS_VERSION_OF_SDL_GetCursor
#define SDL_FreeCursor IGNORE_THIS_VERSION_OF_SDL_FreeCursor
#define SDL_UpdateRect IGNORE_THIS_VERSION_OF_SDL_UpdateRect
#define SDL_UpdateRects IGNORE_THIS_VERSION_OF_SDL_UpdateRects
#define SDL_GetMouseState IGNORE_THIS_VERSION_OF_SDL_GetMouseState
#define SDL_GetRelativeMouseState IGNORE_THIS_VERSION_OF_SDL_GetRelativeMouseState
#define SDL_GL_SetAttribute IGNORE_THIS_VERSION_OF_SDL_GL_SetAttribute
#define SDL_GL_GetAttribute IGNORE_THIS_VERSION_OF_SDL_GL_GetAttribute
#define SDL_CreateThread IGNORE_THIS_VERSION_OF_SDL_CreateThread
#define SDL_AddTimer IGNORE_THIS_VERSION_OF_SDL_AddTimer
#define SDL_RemoveTimer IGNORE_THIS_VERSION_OF_SDL_RemoveTimer
#define SDL_AllocRW IGNORE_THIS_VERSION_OF_SDL_AllocRW
#define SDL_FreeRW IGNORE_THIS_VERSION_OF_SDL_FreeRW
#define SDL_RWFromFile IGNORE_THIS_VERSION_OF_SDL_RWFromFile
#define SDL_RWFromFP IGNORE_THIS_VERSION_OF_SDL_RWFromFP
#define SDL_RWFromMem IGNORE_THIS_VERSION_OF_SDL_RWFromMem
#define SDL_RWFromConstMem IGNORE_THIS_VERSION_OF_SDL_RWFromConstMem
#define SDL_ReadLE16 IGNORE_THIS_VERSION_OF_SDL_ReadLE16
#define SDL_ReadBE16 IGNORE_THIS_VERSION_OF_SDL_ReadBE16
#define SDL_ReadLE32 IGNORE_THIS_VERSION_OF_SDL_ReadLE32
#define SDL_ReadBE32 IGNORE_THIS_VERSION_OF_SDL_ReadBE32
#define SDL_ReadLE64 IGNORE_THIS_VERSION_OF_SDL_ReadLE64
#define SDL_ReadBE64 IGNORE_THIS_VERSION_OF_SDL_ReadBE64
#define SDL_WriteLE16 IGNORE_THIS_VERSION_OF_SDL_WriteLE16
#define SDL_WriteBE16 IGNORE_THIS_VERSION_OF_SDL_WriteBE16
#define SDL_WriteLE32 IGNORE_THIS_VERSION_OF_SDL_WriteLE32
#define SDL_WriteBE32 IGNORE_THIS_VERSION_OF_SDL_WriteBE32
#define SDL_WriteLE64 IGNORE_THIS_VERSION_OF_SDL_WriteLE64
#define SDL_WriteBE64 IGNORE_THIS_VERSION_OF_SDL_WriteBE64
#define SDL_GetThreadID IGNORE_THIS_VERSION_OF_SDL_GetThreadID
#define SDL_ThreadID IGNORE_THIS_VERSION_OF_SDL_ThreadID
#define SDL_JoystickName IGNORE_THIS_VERSION_OF_SDL_JoystickName
#define SDL_LoadBMP_RW IGNORE_THIS_VERSION_OF_SDL_LoadBMP_RW
#define SDL_SaveBMP_RW IGNORE_THIS_VERSION_OF_SDL_SaveBMP_RW
#define SDL_LoadWAV_RW IGNORE_THIS_VERSION_OF_SDL_LoadWAV_RW
#define SDL_UpperBlit IGNORE_THIS_VERSION_OF_SDL_UpperBlit
#define SDL_LowerBlit IGNORE_THIS_VERSION_OF_SDL_LowerBlit
#define SDL_SoftStretch IGNORE_THIS_VERSION_OF_SDL_SoftStretch
#define SDL_ConvertSurface IGNORE_THIS_VERSION_OF_SDL_ConvertSurface
#define SDL_SetColorKey IGNORE_THIS_VERSION_OF_SDL_SetColorKey
#define SDL_LockSurface IGNORE_THIS_VERSION_OF_SDL_LockSurface
#define SDL_UnlockSurface IGNORE_THIS_VERSION_OF_SDL_UnlockSurface
#define SDL_GetKeyName IGNORE_THIS_VERSION_OF_SDL_GetKeyName
#define SDL_VideoInit IGNORE_THIS_VERSION_OF_SDL_VideoInit
#define SDL_BuildAudioCVT IGNORE_THIS_VERSION_OF_SDL_BuildAudioCVT
#define SDL_ConvertAudio IGNORE_THIS_VERSION_OF_SDL_ConvertAudio

#define BUILD_SDL 1

/* *** HACK HACK HACK:
 * *** Avoid including SDL_thread.h: it defines SDL_CreateThread() as a macro */
#if defined(_WIN32) || defined(__OS2__)
#define _SDL_thread_h
#define SDL_thread_h_
#define SDL_PASSED_BEGINTHREAD_ENDTHREAD
#endif
#ifdef __OS2__
#define INCL_DOSMODULEMGR /* for Dos_LoadModule() & co. */
#endif

#define __BUILDING_SDL12_COMPAT__ 1
#include "SDL.h"
#include "SDL_syswm.h"    /* includes windows.h for _WIN32, os2.h for __OS2__ */

/* Missing SDL_thread.h stuff (see above): */
#if defined(_WIN32) || defined(__OS2__)
typedef struct SDL_Thread SDL_Thread;
typedef int (SDLCALL *SDL_ThreadFunction) (void*);
#endif
#ifdef __OS2__
typedef int (*pfnSDL_CurrentBeginThread) (void (*func)(void*), void*, unsigned, void*);
typedef void (*pfnSDL_CurrentEndThread) (void);
#endif
#ifdef _WIN32
typedef UINT_PTR (__cdecl *pfnSDL_CurrentBeginThread)
                   (void*, unsigned, unsigned (__stdcall *func)(void*), void*, unsigned, unsigned*);
typedef void (__cdecl *pfnSDL_CurrentEndThread) (unsigned);
/* the following macros from Win32 SDK headers are harmful here: */
#undef CreateThread
#undef CreateSemaphore
#undef CreateMutex
#endif /* _WIN32 */

#undef SDL_Log
#undef SDL_GetVersion
#undef SDL_ReportAssertion
#undef SDL_Error
#undef SDL_SetError
#undef SDL_PollEvent
#undef SDL_PushEvent
#undef SDL_EventState
#undef SDL_PeepEvents
#undef SDL_WaitEvent
#undef SDL_SetEventFilter
#undef SDL_GetEventFilter
#undef SDL_CreateRGBSurface
#undef SDL_CreateRGBSurfaceFrom
#undef SDL_FreeSurface
#undef SDL_SetClipRect
#undef SDL_GetClipRect
#undef SDL_FillRect
#undef SDL_GetRGB
#undef SDL_GetRGBA
#undef SDL_MapRGB
#undef SDL_MapRGBA
#undef SDL_CreateCursor
#undef SDL_SetCursor
#undef SDL_GetCursor
#undef SDL_FreeCursor
#undef SDL_UpdateRect
#undef SDL_UpdateRects
#undef SDL_GetMouseState
#undef SDL_GetRelativeMouseState
#undef SDL_GL_SetAttribute
#undef SDL_GL_GetAttribute
#undef SDL_CreateThread
#undef SDL_AddTimer
#undef SDL_RemoveTimer
#undef SDL_AllocRW
#undef SDL_FreeRW
#undef SDL_RWFromFile
#undef SDL_RWFromFP
#undef SDL_RWFromMem
#undef SDL_RWFromConstMem
#undef SDL_ReadLE16
#undef SDL_ReadBE16
#undef SDL_ReadLE32
#undef SDL_ReadBE32
#undef SDL_ReadLE64
#undef SDL_ReadBE64
#undef SDL_WriteLE16
#undef SDL_WriteBE16
#undef SDL_WriteLE32
#undef SDL_WriteBE32
#undef SDL_WriteLE64
#undef SDL_WriteBE64
#undef SDL_GetThreadID
#undef SDL_ThreadID
#undef SDL_JoystickName
#undef SDL_LoadBMP_RW
#undef SDL_SaveBMP_RW
#undef SDL_LoadWAV_RW
#undef SDL_UpperBlit
#undef SDL_LowerBlit
#undef SDL_SoftStretch
#undef SDL_ConvertSurface
#undef SDL_SetColorKey
#undef SDL_LockSurface
#undef SDL_UnlockSurface
#undef SDL_GetKeyName
#undef SDL_VideoInit
#undef SDL_BuildAudioCVT
#undef SDL_ConvertAudio

#ifdef SDL_mutexP
#undef SDL_mutexP
#endif

#ifdef SDL_mutexV
#undef SDL_mutexV
#endif

#ifdef SDL_BlitSurface
#undef SDL_BlitSurface
#endif

#ifdef SDL_malloc
#undef SDL_malloc
#endif

#ifdef SDL_calloc
#undef SDL_calloc
#endif

#ifdef SDL_realloc
#undef SDL_realloc
#endif

#ifdef SDL_free
#undef SDL_free
#endif

#ifdef SDL_getenv
#undef SDL_getenv
#endif

#ifdef SDL_qsort
#undef SDL_qsort
#endif

#ifdef SDL_memset
#undef SDL_memset
#endif

#ifdef SDL_memcpy
#undef SDL_memcpy
#endif

#ifdef SDL_revcpy
#undef SDL_revcpy
#endif

#ifdef SDL_memcmp
#undef SDL_memcmp
#endif

#ifdef SDL_strlen
#undef SDL_strlen
#endif

#ifdef SDL_strlcpy
#undef SDL_strlcpy
#endif

#ifdef SDL_strlcat
#undef SDL_strlcat
#endif

#ifdef SDL_strdup
#undef SDL_strdup
#endif

#ifdef SDL_strrev
#undef SDL_strrev
#endif

#ifdef SDL_strupr
#undef SDL_strupr
#endif

#ifdef SDL_strlwr
#undef SDL_strlwr
#endif

#ifdef SDL_strchr
#undef SDL_strchr
#endif

#ifdef SDL_strrchr
#undef SDL_strrchr
#endif

#ifdef SDL_strstr
#undef SDL_strstr
#endif

#ifdef SDL_ltoa
#undef SDL_ltoa
#endif

#ifdef SDL_ultoa
#undef SDL_ultoa
#endif

#ifdef SDL_strtol
#undef SDL_strtol
#endif

#ifdef SDL_strtoul
#undef SDL_strtoul
#endif

#ifdef SDL_lltoa
#undef SDL_lltoa
#endif

#ifdef SDL_ulltoa
#undef SDL_ulltoa
#endif

#ifdef SDL_strtoll
#undef SDL_strtoll
#endif

#ifdef SDL_strtoull
#undef SDL_strtoull
#endif

#ifdef SDL_strtod
#undef SDL_strtod
#endif

#ifdef SDL_strcmp
#undef SDL_strcmp
#endif

#ifdef SDL_strncmp
#undef SDL_strncmp
#endif

#ifdef SDL_strcasecmp
#undef SDL_strcasecmp
#endif

#ifdef SDL_strncasecmp
#undef SDL_strncasecmp
#endif

#ifdef SDL_sscanf
#undef SDL_sscanf
#endif

#ifdef SDL_snprintf
#undef SDL_snprintf
#endif

#ifdef SDL_vsnprintf
#undef SDL_vsnprintf
#endif

#ifdef SDL_iconv_open
#undef SDL_iconv_open
#endif

#ifdef SDL_iconv_close
#undef SDL_iconv_close
#endif

#ifdef SDL_iconv
#undef SDL_iconv
#endif

#ifdef SDL_iconv_string
#undef SDL_iconv_string
#endif

#ifdef SDL_atoi
#undef SDL_atoi
#endif

#ifdef SDL_setenv
#undef SDL_setenv
#endif

#endif

/* vi: set ts=4 sw=4 expandtab: */

