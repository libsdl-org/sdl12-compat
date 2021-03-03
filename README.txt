
                         Simple DirectMedia Layer

                                  (SDL)

                               sdl12-compat

---
https://www.libsdl.org/

This is the Simple DirectMedia Layer, a general API that provides low
level access to audio, keyboard, mouse, joystick, 3D hardware via OpenGL,
and 2D framebuffer across multiple platforms.

This code is a compatibility layer; it provides a binary-compatible API for
programs written against SDL 1.2, but it uses SDL 2.0 behind the scenes. If
you are writing new code, please target SDL 2.0 directly and do not use this
layer.

If you absolutely must have the real SDL 1.2 ("SDL 1.2 Classic"), please use
the source tree at https://github.com/libsdl-org/SDL-1.2, which occasionally
gets bug fixes but no formal release. But we strongly encourage you not to do
that.
