# NOTE:  If you installed SDL2 at an unusual place and FindSDL2 fails,
#        you can define SDL2_INCLUDE_DIR on the cmdline.  For example:
#        cmake -DSDL2_INCLUDE_DIR=/opt/SDL2/include/SDL2 [other stuff]

cmake_minimum_required(VERSION 3.0.0)
project(sdl12_compat
        VERSION 0.0.1
        LANGUAGES C)

set(SDL12_COMPAT_VERSION_STR "1.2.50")

option(SDL12TESTS "Enable to build SDL-1.2 test programs" ON)
option(SDL12DEVEL "Enable installing SDL-1.2 development headers" ON)
option(STATICDEVEL "Enable installing static link library" OFF)

set(CMAKE_SKIP_RPATH TRUE)

if(APPLE)
  set(OSX_SRCS "src/SDL12_compat_objc.m")
  set_source_files_properties(${OSX_SRCS} PROPERTIES LANGUAGE C)
  # the following matches SDL-1.2 Xcode project file
  set(DYLIB_COMPAT_VERSION 1.0.0 CACHE STRING "library compatibility version")
  set(DYLIB_CURRENT_VERSION 12.50.0 CACHE STRING "library current version")
  include_directories("/opt/X11/include")  # hack.
endif()
if(WIN32)
  set(WIN32_SRCS "src/version.rc")
endif()

set(SDL12COMPAT_SRCS
  src/SDL12_compat.c
  ${OSX_SRCS}
  ${WIN32_SRCS}
)
add_library(SDL SHARED ${SDL12COMPAT_SRCS})

include(GNUInstallDirs)
include("cmake/modules/FindSDL2.cmake")
target_include_directories(SDL PRIVATE ${SDL2_INCLUDE_DIRS})

if (CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror=declaration-after-statement") # force these universally.
endif()

# avoid DLL having 'lib' prefix with Windows MinGW builds
if(WIN32)
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
  set_target_properties(SDL PROPERTIES COMPILE_DEFINITIONS "DLL_EXPORT")
endif()

if(UNIX AND NOT APPLE)
    set_target_properties(SDL PROPERTIES COMPILE_DEFINITIONS "_REENTRANT")
    target_link_libraries(SDL PRIVATE dl)
endif()
if(APPLE)
    set_target_properties(SDL PROPERTIES COMPILE_DEFINITIONS "_THREAD_SAFE")
    set_target_properties(SDL PROPERTIES LINK_FLAGS
            "-Wl,-compatibility_version,${DYLIB_COMPAT_VERSION} -Wl,-current_version,${DYLIB_CURRENT_VERSION}")
    target_link_libraries(SDL PRIVATE "-framework AppKit")
    set_target_properties(SDL PROPERTIES
        MACOSX_RPATH 1
        OUTPUT_NAME "SDL-1.2.0"
    )
elseif(UNIX AND NOT ANDROID)
    set_target_properties(SDL PROPERTIES
        VERSION "${SDL12_COMPAT_VERSION_STR}"
        SOVERSION "0"
        OUTPUT_NAME "SDL-1.2")
else()
    set_target_properties(SDL PROPERTIES
        VERSION "${SDL12_COMPAT_VERSION_STR}"
        SOVERSION "0"
      OUTPUT_NAME "SDL")
endif()

if(MINGW)
    set_target_properties(SDL PROPERTIES LINK_FLAGS "-nostdlib")
    target_link_libraries(SDL PRIVATE "-static-libgcc -lgcc") # libgcc is needed for 32 bit (x86) builds
endif()
if(MSVC)
    # Don't try to link with the default set of libraries.
    set_target_properties(SDL PROPERTIES COMPILE_FLAGS "/GS-")
    set_target_properties(SDL PROPERTIES LINK_FLAGS "/NODEFAULTLIB")
    # Make sure /RTC1 is disabled: (from SDL2 CMake)
    foreach(flag_var
      CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
      CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
      string(REGEX REPLACE "/RTC(su|[1su])" "" ${flag_var} "${${flag_var}}")
    endforeach(flag_var)
endif()


if(SDL12TESTS)
    if(NOT (WIN32 OR APPLE OR CYGWIN OR HAIKU OR BEOS))
      find_library(MATH_LIBRARY m)
    endif()

    find_package(OpenGL COMPONENTS OpenGL)
    if(OPENGL_FOUND)
      set(HAVE_OPENGL_DEFINE "HAVE_OPENGL")
      if(WIN32)
        set(OPENGL_gl_LIBRARY "opengl32")
        set(OPENGL_opengl_LIBRARY "opengl32")
      elseif(APPLE)
        set(OPENGL_gl_LIBRARY "-Wl,-framework,OpenGL")
        set(OPENGL_opengl_LIBRARY "-Wl,-framework,OpenGL")
      endif()
    endif()

    macro(test_program _NAME _SRCS)
        add_executable(${_NAME} ${_SRCS})
        target_include_directories(${_NAME} PRIVATE "include/SDL")
        target_link_libraries(${_NAME} SDL)
        # Turn off MSVC's aggressive C runtime warnings for the old test programs.
        if(MSVC)
          set_target_properties(${_NAME} PROPERTIES COMPILE_DEFINITIONS "${HAVE_OPENGL_DEFINE};_CRT_SECURE_NO_WARNINGS;_CRT_SECURE_NO_DEPRECATE;_CRT_NONSTDC_NO_DEPRECATE")
        elseif(APPLE)
          set_target_properties(${_NAME} PROPERTIES COMPILE_DEFINITIONS "${HAVE_OPENGL_DEFINE};GL_SILENCE_DEPRECATION=1")
        else()
          set_target_properties(${_NAME} PROPERTIES COMPILE_DEFINITIONS "${HAVE_OPENGL_DEFINE}")
        endif()
        if(MATH_LIBRARY)
          target_link_libraries(${_NAME} ${MATH_LIBRARY})
        endif()
    endmacro()

    test_program(checkkeys "test/checkkeys.c")
    test_program(graywin "test/graywin.c")
    test_program(loopwave "test/loopwave.c")
    test_program(testalpha "test/testalpha.c")
    test_program(testbitmap "test/testbitmap.c")
    test_program(testblitspeed "test/testblitspeed.c")
    test_program(testcdrom "test/testcdrom.c")
    test_program(testcursor "test/testcursor.c")
    test_program(testerror "test/testerror.c")
    test_program(testfile "test/testfile.c")
    test_program(testgamma "test/testgamma.c")
    test_program(testthread "test/testhread.c")
    test_program(testiconv "test/testiconv.c")
    test_program(testjoystick "test/testjoystick.c")
    test_program(testkeys "test/testkeys.c")
    test_program(testloadso "test/testloadso.c")
    test_program(testlock "test/testlock.c")
    test_program(testoverlay "test/testoverlay.c")
    test_program(testoverlay2 "test/testoverlay2.c")
    test_program(testpalette "test/testpalette.c")
    test_program(testplatform "test/testplatform.c")
    test_program(testsem "test/testsem.c")
    test_program(testsprite "test/testsprite.c")
    test_program(testtimer "test/testtimer.c")
    test_program(testver "test/testver.c")
    test_program(testvidinfo "test/testvidinfo.c")
    test_program(testwin "test/testwin.c")
    test_program(testwm "test/testwm.c")
    test_program(threadwin "test/threadwin.c")
    test_program(torturethread "test/torturethread.c")
    test_program(testdyngl "test/testdyngl.c")
    test_program(testgl "test/testgl.c")
    if(OPENGL_FOUND)
      if(CMAKE_VERSION VERSION_LESS 3.10)
        target_link_libraries(testgl ${OPENGL_gl_LIBRARY})
      else()
        target_link_libraries(testgl ${OPENGL_opengl_LIBRARY})
      endif()
    endif()

    foreach(fname "icon.bmp" "moose.dat" "picture.xbm" "sail.bmp" "sample.bmp" "sample.wav" "utf8.txt")
        file(COPY "${CMAKE_SOURCE_DIR}/test/${fname}" DESTINATION "${CMAKE_BINARY_DIR}")
    endforeach(fname)
endif()

install(TARGETS SDL
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(SDL12DEVEL)
  set(SDL_VERSION "${SDL12_COMPAT_VERSION_STR}")

  install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  if(NOT MSVC)
    if(WIN32)
      # FIXME: no SDL_main for Windows!
      set(SDL_CFLAGS "")                          # -Dmain=SDL_main
      set(SDL_RLD_FLAGS "")
      set(SDL_LIBS "-mwindows -lSDL")             # -mwindows -lmingw32 -lSDLmain -lSDL
      set(SDL_STATIC_LIBS "-lkernel32 -luser32")  # -mwindows -lmingw32 -lSDLmain -lSDL -lkernel32 -luser32
    elseif(APPLE)
      # FIXME: no SDL_main for Mac!
      set(SDL_CFLAGS "-D_THREAD_SAFE")            # -Dmain=SDL_main
      set(SDL_LIBS "-lSDL")                       #       -lSDLmain
      set(SDL_STATIC_LIBS "-Wl,-framework,AppKit -lobjc")
      set(SDL_RLD_FLAGS "")  # !!! FIXME: this forces rpath, which we might want?
    else() # unix
      set(SDL_CFLAGS "-D_GNU_SOURCE=1 -D_REENTRANT")
      set(SDL_RLD_FLAGS "")  # !!! FIXME: this forces rpath, which we might want?
      set(SDL_LIBS "-lSDL")
      set(SDL_STATIC_LIBS "-lm -ldl -lpthread")
    endif()

    configure_file(sdl12_compat.pc.in sdl12_compat.pc @ONLY)
    install(FILES ${CMAKE_BINARY_DIR}/sdl12_compat.pc
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )

    # !!! FIXME: do we _want_ static builds?
    set(ENABLE_STATIC_TRUE "")
    set(ENABLE_STATIC_FALSE "#")
    set(ENABLE_SHARED_TRUE "")
    set(ENABLE_SHARED_FALSE "#")

    configure_file("${CMAKE_SOURCE_DIR}/sdl-config.in" "${CMAKE_BINARY_DIR}/sdl-config" @ONLY)
    install(PROGRAMS "${CMAKE_BINARY_DIR}/sdl-config" DESTINATION bin)
  endif()

  set(SOEXT ${CMAKE_SHARED_LIBRARY_SUFFIX})
  get_target_property(SONAME SDL OUTPUT_NAME)
  if(UNIX AND NOT ANDROID)
    install(CODE "
      execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
        \"lib${SONAME}${SOPOSTFIX}${SOEXT}\" \"libSDL${SOPOSTFIX}${SOEXT}\"
        WORKING_DIRECTORY \"${CMAKE_BINARY_DIR}\")")
    install(FILES ${CMAKE_BINARY_DIR}/libSDL${SOPOSTFIX}${SOEXT} DESTINATION "${CMAKE_INSTALL_LIBDIR}")
  endif()

  install(FILES "${CMAKE_SOURCE_DIR}/sdl.m4" DESTINATION "${CMAKE_INSTALL_FULL_DATAROOTDIR}/aclocal")
endif()

if(STATICDEVEL AND SDL12DEVEL)
  add_library(SDL-static STATIC ${SDL12COMPAT_SRCS})
  target_include_directories(SDL-static PRIVATE ${SDL2_INCLUDE_DIRS})
  if(UNIX AND NOT APPLE)
      set_target_properties(SDL-static PROPERTIES COMPILE_DEFINITIONS "_REENTRANT")
      target_link_libraries(SDL-static PRIVATE dl)
  endif()
  if(APPLE)
      set_target_properties(SDL-static PROPERTIES COMPILE_DEFINITIONS "_THREAD_SAFE")
      target_link_libraries(SDL-static PRIVATE "-framework AppKit")
      set_target_properties(SDL-static PROPERTIES
          MACOSX_RPATH 1
          OUTPUT_NAME "SDL"
      )
  else()
      set_target_properties(SDL-static PROPERTIES
          VERSION "${SDL12_COMPAT_VERSION_STR}"
        OUTPUT_NAME "SDL")
  endif()

  if(MINGW)
      set_target_properties(SDL-static PROPERTIES LINK_FLAGS "-nostdlib")
  endif()
  if(MSVC)
      # Don't try to link with the default set of libraries.
      set_target_properties(SDL-static PROPERTIES COMPILE_FLAGS "/GS-")
      set_target_properties(SDL-static PROPERTIES LINK_FLAGS "/NODEFAULTLIB")
      # Make sure /RTC1 is disabled: (from SDL2 CMake)
      foreach(flag_var
        CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
        string(REGEX REPLACE "/RTC(su|[1su])" "" ${flag_var} "${${flag_var}}")
      endforeach(flag_var)
  endif()

  install(TARGETS SDL-static
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )
endif()
