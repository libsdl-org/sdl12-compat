# Simple DirectMedia Layer (SDL) sdl12-compat

https://www.libsdl.org/

This is the Simple DirectMedia Layer, a general API that provides low
level access to audio, keyboard, mouse, joystick, 3D hardware via OpenGL,
and 2D framebuffer across multiple platforms.

This code is a compatibility layer; it provides a binary and source
compatible API for programs written against SDL 1.2, but it uses SDL 2.0
behind the scenes. If you are writing new code, please target SDL 2.0
directly and do not use this layer.

If you absolutely must have the real SDL 1.2 ("SDL 1.2 Classic"), please use
the source tree at https://github.com/libsdl-org/SDL-1.2, which occasionally
gets bug fixes but no formal release. But we strongly encourage you not to do
that.

# How to use:

- Build the library. This will need access to SDL2's headers (v2.0.7 or newer),
[CMake](https://cmake.org/) and the build tools of your choice. Once built, you
will have a drop-in replacement that can be used with any existing binary
that relies on SDL 1.2. You can copy this library over the existing 1.2 build,
or force it to take priority over a system copy with LD_LIBRARY_PATH, etc.
At runtime, sdl12-compat needs to be able to find a copy of SDL2 (v2.0.7 or
newer -- v2.0.12 or newer for Windows), so plan to include it with the library
if necessary.

- If you want to build an SDL 1.2 program from source code, we have included
compatibility headers, so that sdl12-compat can completely replace SDL 1.2
at all points. These headers are just the bare minimum needed for source-level
compatibility and don't have a lot of documentation or fanciness at all. The
new headers are also under the zlib license. Note that sdl12-compat itself
does not use these headers, so if you just want the library, you don't need
them.
