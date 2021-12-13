# Compatibility notes

This is a list of quirks and known-issues for specific games, with possible
workarounds.

We are currently building and maintaining a list of all known SDL 1.2 games
and their current state with sdl12-compat over here:

https://docs.google.com/spreadsheets/d/1u8Rq3LVQYYgu28sBuxrZ371QolbiZu5z_LjENc4ddZs/edit?usp=sharing



## Dynamite Jack (Linux)

More modern builds are ported to SDL2, but the older 1.2 binaries will work
on X11 if you turn off OpenGL Scaling

    export SDL12COMPAT_OPENGL_SCALING=0

Note that this game calls glXGetProcAddress() directly and likely will not
work as a native Wayland app (XWayland works fine, however).


## Hammerfight (Linux)

You probably aren't getting usable mouse input, as the game talks directly to
XInput2 on X11 (and tries to use /dev/input if that fails). This is not an
sdl12-compat bug, as this is going outside of SDL to get multiple mice
input. The game is statically linked to a copy of [ManyMouse](https://icculus.org/manymouse/),
which provides multiple mice access.


The simplest way to deal with this is change this line in the game's
Config.ini file:

    CORE_INIT_RI=true

Make that false and it won't even try to initialize ManyMouse, and will use
standard SDL 1.2 mouse events for single-player input. (The "RI" stands for
"RawInput," which Hammerfight uses on Windows for multi-mice support).

Of course, with this change, you can't have multiple players on the same
machine using separate mice.

That game dlopen()'s the XInput2 libraries instead of linking to them
directly, so this does not prevent the game from working on Wayland. No
XWayland needed!


