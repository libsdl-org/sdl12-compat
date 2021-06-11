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

/* This file contains some macOS-specific support code */

#define __BUILDING_SDL12_COMPAT__ 1
#include "SDL.h"

#ifdef __MACOSX__
#include <Cocoa/Cocoa.h>

#if __GNUC__ >= 4
#define SDL12_PRIVATE __attribute__((visibility("hidden")))
#else
#define SDL12_PRIVATE __private_extern__
#endif

/* This has to be in a separate, Objective-C source file because it calls
   into Cocoa. The issue is that SDL 1.2 apps on macOS are statically linked
   with SDLmain, which does something mac-specific that conflicts with SDL2
   before the app's main() is even called, and we have to counteract that. */

SDL12_PRIVATE void sdl12_compat_macos_init(void)
{
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
}

#ifndef MAC_OS_X_VERSION_10_12
#define NSAlertStyleCritical NSCriticalAlertStyle
#endif

SDL12_PRIVATE void error_dialog(const char *errorMsg)
{
    NSAlert *alert;

    if (NSApp == nil) {
        ProcessSerialNumber psn = { 0, kCurrentProcess };
        TransformProcessType(&psn, kProcessTransformToForegroundApplication);
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        [NSApp finishLaunching];
    }

    [NSApp activateIgnoringOtherApps:YES];
    alert = [[[NSAlert alloc] init] autorelease];
    alert.alertStyle = NSAlertStyleCritical;
    alert.messageText = @"Fatal error! Cannot continue!";
    alert.informativeText = [NSString stringWithCString:errorMsg encoding:NSASCIIStringEncoding];
    [alert runModal];
}
#endif

/* vi: set ts=4 sw=4 expandtab: */

