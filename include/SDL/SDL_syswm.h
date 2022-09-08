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

/* These headers are from sdl12-compat, and are intended to give just enough
functionality to let you build an SDL-1.2-based project without having the
real SDL-1.2 available to you. */

#ifndef _SDL_syswm_h
#define _SDL_syswm_h

#include "SDL_stdinc.h"
#include "SDL_error.h"
#include "SDL_version.h"

#ifndef SDL_PROTOTYPES_ONLY

#   if defined(__WIN32__)
#       ifndef WIN32_LEAN_AND_MEAN
#           define WIN32_LEAN_AND_MEAN
#       endif
#       include <windows.h>

#       include "begin_code.h"

        struct SDL_SysWMmsg
        {
            SDL_version version;
            HWND hwnd;
            UINT msg;
            WPARAM wParam;
            LPARAM lParam;
        };

        typedef struct SDL_SysWMinfo {
            SDL_version version;
            HWND window;
            HGLRC hglrc;
        } SDL_SysWMinfo;

#       include "close_code.h"

#   elif defined(SDL_VIDEO_DRIVER_X11)
#       include <X11/Xlib.h>
#       include <X11/Xatom.h>

#       include "begin_code.h"

        typedef enum SDL_SYSWM_TYPE
        {
            SDL_SYSWM_X11
        } SDL_SYSWM_TYPE;

        struct SDL_SysWMmsg
        {
            SDL_version version;
            SDL_SYSWM_TYPE subsystem;
            union {
                XEvent xevent;
            } event;
        };

        typedef struct SDL_SysWMinfo
        {
            SDL_version version;
            SDL_SYSWM_TYPE subsystem;
            union {
                struct {
                    Display *display;
                    Window window;
                    void (*lock_func)(void);
                    void (*unlock_func)(void);
                    Window fswindow;
                    Window wmwindow;
                    Display *gfxdisplay;
                } x11;
            } info;
        } SDL_SysWMinfo;

#       include "close_code.h"

#   else

#       include "begin_code.h"

        struct SDL_SysWMmsg
        {
            SDL_version version;
            int data;
        };

        typedef struct SDL_SysWMinfo
        {
            SDL_version version;
            int data;
        } SDL_SysWMinfo;

#       include "close_code.h"

#   endif

#endif

#include "begin_code.h"

extern DECLSPEC int SDLCALL SDL_GetWMInfo(SDL_SysWMinfo *info);

#include "close_code.h"

#endif

