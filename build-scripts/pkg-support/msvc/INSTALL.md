
# Using this package

This package contains sdl12-compat built for Visual Studio.

To use this package, edit your project properties:
- Add the include directory to "VC++ Directories" -> "Include Directories"
- Add the lib/_arch_ directory to "VC++ Directories" -> "Library Directories"
- Add SDL.lib and SDLmain.lib to Linker -> Input -> "Additional Dependencies"
- Copy lib/_arch_/SDL.dll, lib/_arch_/SDL2.dll, and lib/_arch_/SDL3.dll to your project directory.

