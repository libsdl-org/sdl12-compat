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

# Building the library:

These are quick-start instructions; there isn't anything out of the ordinary
here if you're used to using CMake. 

You'll need to use CMake to build sdl12-compat. Download at
[cmake.org](https://cmake.org/) or install from your package manager
(`sudo apt-get install cmake` on Ubuntu, etc).

Please refer to the [CMake documentation](https://cmake.org/documentation/)
for complete details, as platform and build tool details vary.

You'll need a copy of SDL 2.0.x to build sdl12-compat, because we need the
SDL2 headers. You can build this from source or install from a package
manager. Windows and Mac users can download prebuilt binaries from
[SDL's download page](https://libsdl.org/download-2.0.php); make sure you
get the "development libraries" and not "runtime binaries" there.

Now just point CMake at sdl12-compat's directory. Here's a command-line
example:

```bash
cd sdl12-compat
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release .
cmake --build build
```

On Windows or macOS, you might prefer to use CMake's GUI, but it's the same
idea: give it the directory where sdl12-compat is located, click "Configure,"
choose your favorite compiler, then click "Generate." Now you have project
files! Click "Open Project" to launch your development environment. Then you
can build however you like with Visual Studio, Xcode, etc.

If necessary, you might have to fill in the location of the SDL2 headers
when using CMake. sdl12-compat does not need SDL2's library to _build_,
just its headers (although it may complain about the missing library,
you can ignore that). From the command line, add
`-DSDL2_INCLUDE_DIR=/path/to/SDL2/include`, or find this in the CMake
GUI and set it appropriately, click "Configure" again, and then "Generate."

When the build is complete, you'll have a shared library you can drop in
as a replacement for an existing SDL 1.2 build. This will also build
the original SDL 1.2 test apps, so you can verify the library is working.

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

- SDL12COMPAT_SCALE_METHOD:
  Choose the scaling method used when applications render at a non-native
  resolution.  The options are `nearest`, for nearest-neighbour sampling
  (more pixelated) and `linear` for bilinear sampling (blurrier).

- SDL12COMPAT_HIGHDPI:
  Advertise the application as supporting high-DPI displays.  Enabling
  this will usually result in sharper graphics, but on some applications
  text and other elements may become very small.

- SDL12COMPAT_SYNC_TO_VBLANK:
  Force the application to sync (or not sync) to the vertical blanking
  interval (VSync).  When enabled, this will cap the application's
  framerate to the screen's refresh rate (and may resolve issues with
  screen tearing).

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

# Compatibility issues with OpenGL scaling

The OpenGL scaling feature of sdl12-compat allows applications which wish to
run at a non-native screen resolution to do so without changing the system
resolution. It does this by redirecting OpenGL rendering calls to a "fake"
backbuffer which is scaled when rendering.

This works well for simple applications, but for more complicated applications
which use Frame Buffer Objects, sdl12-compat needs to intercept and redirect
some OpenGL calls. Applications which access these functions without going
though SDL (even if via a library) may not successfully render anything, or
may render incorrectly if OpenGL scaling is enabled.

In these cases, you can disable OpenGL scaling by setting the environment
variable:
```
SDL12COMPAT_OPENGL_SCALING=0
```

# Compatibility issues with applications directly accessing underlying APIs

Some applications combine the use of SDL with direct access to the underlying
OS or window system. When running these applications on the same OS and SDL
video driver (e.g. a program written for X11 on Linux is run on X11 on Linux),
sdl12-compat is usually compatible.

However, if you wish to run an application on a different video driver, the
application will be unable to access the underlying API it is expecting, and
may fail. This often occurs trying to run applications written for X11 under
Wayland, and particularly affects a number of popular OpenGL extension loaders.

In this case, the best workaround is to run under a compatibility layer like
XWayland, and set the SDL_VIDEODRIVER environment variable to the driver the
program is expecting:
```
SDL_VIDEODRIVER=x11
```
