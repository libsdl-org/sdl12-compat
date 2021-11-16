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

# Configuration options:

sdl12-compat has a number of configuration options which can be used to work
around issues with individual applications, or to better fit your system or
preferences.

These options are all specified as environment variables, and can be set by
running your application with them set on the command-line, for example:
```
SDL12COMPAT_HIGHDPI=1 SDL12COMPAT_OPENGL_SCALING=0 %command%
```
will run `%command%` with high-dpi monitor support enabled, but OpenGL
scaling support disabled.

The available options are:

- SDL12COMPAT_DEBUG_LOGGING:
  If enabled, print debugging messages to stderr.  These messages are
  mostly useful to developers, or when trying to track down a specific
  bug.

- SDL12COMPAT_FAKE_CDROM_PATH:
  A path to a directory containing MP3 files (named trackXX.mp3, where
  XX is a two-digit track number) to be used by applications which play
  CD audio.

- SDL12COMPAT_OPENGL_SCALING:
  Enables scaling of OpenGL applications to the current desktop resolution.
  If disabled, applications can change the real screen resolution.  This
  option is enabled by default, but not all applications are compatible
  with it: try changing this if you can only see a black screen.

- SDL12COMPAT_SYNC_TO_VBLANK:
  Force the application to sync (or not sync) to the vertical blanking
  interval (VSync).  When enabled, this will cap the application's
  framerate to the screen's refresh rate (and may resolve issues with
  screen tearing).

- SDL12COMPAT_SCALE_METHOD:
  Choose the scaling method used when applications render at a non-native
  resolution.  The options are `nearest`, for nearest-neighbour sampling
  (more pixelated) and `linear` for bilinear sampling (blurrier).

- SDL12COMPAT_HIGHDPI:
  Advertise the application as supporting high-DPI displays.  Enabling
  this will usually result in sharper graphics, but on some applications
  text and other elements may become very small.

- SDL12COMPAT_USE_KEYBOARD_LAYOUT:
  Make all keyboard input take the current keyboard layout into account.
  This may need to be disabled for applications which provide their own
  keyboard layout support, or if the position of the keys on the keyboard
  is more important than the character they produce.  Note that text input
  (in most applications) will take the keyboard layout into account
  regardless of this option.

- SDL_MOUSE_RELATIVE_SCALING:
  If enabled, relative mouse motion is scaled when the application is
  running at a non-native resolution.  This may be required with some
  applications which use their own mouse cursors. See also:
  https://wiki.libsdl.org/SDL_HINT_MOUSE_RELATIVE_SCALING
