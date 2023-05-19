# Compatibility notes

This is a list of quirks and known-issues for specific games, with possible
workarounds.

We are currently building and maintaining a list of all known SDL 1.2 games
and their current state with sdl12-compat over here:

https://docs.google.com/spreadsheets/d/1u8Rq3LVQYYgu28sBuxrZ371QolbiZu5z_LjENc4ddZs/edit?usp=sharing



## Dynamite Jack (Linux)

More modern builds are ported to SDL2, but the older 1.2 binaries will work
on X11 if you turn off OpenGL Scaling.

    export SDL12COMPAT_OPENGL_SCALING=0

Note that this game calls glXGetProcAddress() directly and likely will not
work as a native Wayland app (XWayland works fine, however).

You have to set this environment variable yourself. Dynamic Jack's binary
is called "main" so we can't reasonably set up an automatic entry for it in
our quirks table.


## Hammerfight (Linux)

You probably aren't getting usable mouse input, as the game talks directly to
XInput2 on X11 (and tries to use /dev/input if that fails). This is not an
sdl12-compat bug, as this is going outside of SDL to get multiple mice
input. The game is statically linked to a copy of
[ManyMouse](https://icculus.org/manymouse/), which provides multiple mice
access.


The simplest way to deal with this is change this line in the game's
Config.ini file:

    CORE_INIT_RI=true

Make that `false` and it won't even try to initialize ManyMouse, and will use
standard SDL 1.2 mouse events for single-player input. (The "RI" stands for
"RawInput," which Hammerfight uses on Windows for multi-mice support).

Of course, with this change, you can't have multiple players on the same
machine using separate mice.

Hammerfight dlopen()'s the XInput2 libraries instead of linking to them
directly, so this does not prevent the game from working on Wayland. No
XWayland needed!


## Tucnak (Linux)

There is an SDL2 target for libzia (which tucnak uses for rendering), and you
should use that in modern times. But if you're on the SDL 1.2 target,
tucnak tries to render from a background thread, and does its event loop on
another, so we can't cleanly hook in to make it draw from the main thread.

sdl12-compat will force SDL2 to use X11, software rendering, and no texture
framebuffer, to avoid using OpenGL, to avoid the threading problems this
will cause. The app is perfectly usable in this configuration (and largely
matches how they expected you to use it with SDL 1.2 anyhow).


## Awesomenauts (Linux)

Awesomenauts requires X11 because it talks directly to glX, can't use our
scaling code because it uses framebuffer objects without going through
SDL_GL_GetProcAddress() to get entry points, and does something weird with
OpenGL contexts on multiple threads. sdl12-compat detects this and forces
on the correct hints to make this work, but it limits how one can use the
game.

This game will work in a Wayland environment, but only as an X11 app through
XWayland.


## Braid (Linux)

Braid requires X11 because it talks directly to glX, and can't use our
scaling code because it uses framebuffer objects without going through
SDL_GL_GetProcAddress() to get entry points. sdl12-compat detects this
and forces on the correct hints to make this work, but it limits how
one can use the game.

This game will work in a Wayland environment, but only as an X11 app through
XWayland.


## DOSBox (Linux)

DOSBox has some pretty strict requirements for keyboard input. We detect
some common names for DOSBox binaries ("dosbox", "dosbox_i686", etc), and
force the correct hints to make it work, but if you have an uncommon binary
name and keyboard input isn't working as you expect, try exporting this
environment variable:

    export SDL12COMPAT_USE_KEYBOARD_LAYOUT=0


## Multiwinia (Linux)

Multiwinia calls SDL_Quit() when changing video modes but doesn't
reinitialize SDL before using it further. We detect this binary and force
the correct hints to make it work, but if you have an uncommon binary
name and the game isn't working, try this environment variable:

    export SDL12COMPAT_NO_QUIT_VIDEO=1

## Civilization: Call to Power (Linux)

The Linux port of Civilization: Call to Power is very old, and has a number
of issues running on modern systems. The relevant ones for sdl12-compat are
that the game uses SDL 1.1 (not SDL 1.2), and that it uses CD audio.

To fix the former, you'll need to either rename the sdl12-compat files, or
patch the game binary to look for libSDL-1.2.so.0.

For the latter, you'll need to rip the game's CD audio, and set the
SDL12COMPAT_FAKE_CDROM_PATH environment variable. Make sure you set this to an
absolute path, as the game changes directories before initializing SDL.

A more complete guide for getting the game to work on modern systems with
sdl12-compat is available [here](https://davidgow.net/hacks/civctp.html).

(Some of these tricks may also work for other old Loki Entertainment ports, too.)
