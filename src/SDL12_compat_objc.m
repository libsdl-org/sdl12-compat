/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2019 Sam Lantinga <slouken@libsdl.org>

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

/* This file contains some macOS-specific support code */

#include "SDL.h"

#ifdef __MACOSX__
#include <Cocoa/Cocoa.h>

/* This has to be in a separate, Objective-C source file because it calls
   into Cocoa. The issue is that SDL 1.2 apps on macOS are statically linked
   with SDLmain, which does something mac-specific that conflicts with SDL2
   before the app's main() is even called, and we have to counteract that. */

void sdl12_compat_macos_init(void)
{
    [[NSApp sharedApplication] setActivationPolicy:NSApplicationActivationPolicyRegular];
}
#endif

/* vi: set ts=4 sw=4 expandtab: */

