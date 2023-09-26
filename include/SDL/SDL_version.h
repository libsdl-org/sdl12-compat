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

#ifndef _SDL_version_h
#define _SDL_version_h

#include "SDL_stdinc.h"

#include "begin_code.h"

/* We bumped the patchlevel to 50 for sdl12-compat */
#define SDL_MAJOR_VERSION 1
#define SDL_MINOR_VERSION 2
#define SDL_PATCHLEVEL 68

typedef struct SDL_version
{
    Uint8 major;
    Uint8 minor;
    Uint8 patch;
} SDL_version;

#define SDL_VERSION(X) { \
    (X)->major = SDL_MAJOR_VERSION; \
    (X)->minor = SDL_MINOR_VERSION; \
    (X)->patch = SDL_PATCHLEVEL; \
}

#define SDL_VERSIONNUM(X, Y, Z) ((X)*1000 + (Y)*100 + (Z))
#define SDL_COMPILEDVERSION SDL_VERSIONNUM(SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL)
#define SDL_VERSION_ATLEAST(X, Y, Z) (SDL_COMPILEDVERSION >= SDL_VERSIONNUM(X, Y, Z))

extern DECLSPEC const SDL_version * SDLCALL SDL_Linked_Version(void);

#include "close_code.h"

#endif
