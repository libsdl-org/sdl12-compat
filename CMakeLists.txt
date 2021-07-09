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

if(STATICDEVEL AND NOT (CMAKE_SYSTEM_NAME MATCHES "Linux"))
  MESSAGE(FATAL_ERROR "Static builds are only supported on Linux.")
endif()

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

include(CheckCSourceCompiles)
include(CheckIncludeFile)
include(CheckCCompilerFlag)

include(GNUInstallDirs)
include("cmake/modules/FindSDL2.cmake")
target_include_directories(SDL PRIVATE ${SDL2_INCLUDE_DIRS})

set(EXTRA_CFLAGS )
if (CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
  set(EXTRA_CFLAGS "${EXTRA_CFLAGS} -Wall")
  check_c_compiler_flag(-Wdeclaration-after-statement HAVE_WDECLARATION_AFTER_STATEMENT)
  if(HAVE_WDECLARATION_AFTER_STATEMENT)
    set(EXTRA_CFLAGS "${EXTRA_CFLAGS} -Wdeclaration-after-statement")
  endif()
  check_c_compiler_flag(-Werror=declaration-after-statement HAVE_WERROR_DECLARATION_AFTER_STATEMENT)
  if(HAVE_WERROR_DECLARATION_AFTER_STATEMENT)
    set(EXTRA_CFLAGS "${EXTRA_CFLAGS} -Werror=declaration-after-statement")
  endif()
endif()
# just in case:
check_include_file("immintrin.h" HAVE_IMMINTRIN_H)
if(NOT HAVE_IMMINTRIN_H)
  set(EXTRA_CFLAGS "${EXTRA_CFLAGS} -DSDL_DISABLE_IMMINTRIN_H")
endif()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS}")
string(STRIP "${CMAKE_C_FLAGS}" CMAKE_C_FLAGS)

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
    set(MSVC_FLAGS "/GS-")
    check_c_source_compiles("int main(void) {
#ifndef _M_IX86
#error not x86
#endif
return 0; }" IS_X86)
    if(IS_X86)  # don't emit SSE2 in x86 builds
      set(MSVC_FLAGS "${MSVC_FLAGS} /arch:SSE")
    endif()
    set_target_properties(SDL PROPERTIES COMPILE_FLAGS ${MSVC_FLAGS})
    set_target_properties(SDL PROPERTIES LINK_FLAGS "/NODEFAULTLIB")
    # Make sure /RTC1 is disabled: (from SDL2 CMake)
    foreach(flag_var
      CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE
      CMAKE_C_FLAGS_MINSIZEREL CMAKE_C_FLAGS_RELWITHDEBINFO)
      string(REGEX REPLACE "/RTC(su|[1su])" "" ${flag_var} "${${flag_var}}")
    endforeach(flag_var)
endif()


# SDLmain library...
if(APPLE)
    add_library(SDLmain STATIC src/SDLmain/macosx/SDLMain.m)
    set_source_files_properties(src/SDLmain/macosx/SDLMain.m PROPERTIES LANGUAGE C)
elseif(WIN32)
    add_library(SDLmain STATIC src/SDLmain/win32/SDL_win32_main.c)
    set_target_properties(SDLmain PROPERTIES COMPILE_DEFINITIONS "_CRT_SECURE_NO_WARNINGS;_CRT_SECURE_NO_DEPRECATE;_CRT_NONSTDC_NO_DEPRECATE")  # !!! FIXME: don't use C runtime? We fixed this in SDL2.
else()
    add_library(SDLmain STATIC src/SDLmain/dummy/SDL_dummy_main.c)
endif()
target_include_directories(SDLmain PRIVATE "include/SDL")

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
        if(MINGW)
          target_link_libraries(${_NAME} mingw32 SDLmain SDL)
        else()
          target_link_libraries(${_NAME} SDLmain SDL)
        endif()
        # Turn off MSVC's aggressive C runtime warnings for the old test programs.
        if(MSVC)
          set_target_properties(${_NAME} PROPERTIES COMPILE_DEFINITIONS "${HAVE_OPENGL_DEFINE};_CRT_SECURE_NO_WARNINGS;_CRT_SECURE_NO_DEPRECATE;_CRT_NONSTDC_NO_DEPRECATE")
        elseif(APPLE)
          set_target_properties(${_NAME} PROPERTIES COMPILE_DEFINITIONS "${HAVE_OPENGL_DEFINE};GL_SILENCE_DEPRECATION=1")
          target_link_libraries(${_NAME} "-Wl,-framework,Cocoa")
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

install(TARGETS SDL SDLmain
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(SDL12DEVEL)
  set(SDL_VERSION "${SDL12_COMPAT_VERSION_STR}")

  install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

  if(NOT MSVC)
    if(WIN32)
      set(SDL_CFLAGS "")
      set(SDL_RLD_FLAGS "")
      set(SDL_LIBS "-lSDLmain -lSDL -mwindows")             # -lmingw32 -lSDL -mwindows
      set(SDL_STATIC_LIBS "")
    elseif(APPLE)
      set(SDL_CFLAGS "-D_THREAD_SAFE")
      set(SDL_LIBS "-lSDLmain -lSDL -Wl,-framework,Cocoa")
      set(SDL_STATIC_LIBS "")
      set(SDL_RLD_FLAGS "")  # !!! FIXME: this forces rpath, which we might want?
    else() # unix
      set(SDL_CFLAGS "-D_GNU_SOURCE=1 -D_REENTRANT")
      set(SDL_RLD_FLAGS "")  # !!! FIXME: this forces rpath, which we might want?
      set(SDL_LIBS "-lSDL")
      set(SDL_STATIC_LIBS "-ldl")
      if(NOT STATICDEVEL)
        set(SDL_STATIC_LIBS "")
      endif()
    endif()

    # !!! FIXME: do we _want_ static builds?
    if(STATICDEVEL)
      set(ENABLE_STATIC_TRUE "")
      set(ENABLE_STATIC_FALSE "#")
    else()
      set(ENABLE_STATIC_TRUE "#")
      set(ENABLE_STATIC_FALSE "")
    endif()
    set(ENABLE_SHARED_TRUE "")
    set(ENABLE_SHARED_FALSE "#")

    configure_file(sdl12_compat.pc.in sdl12_compat.pc @ONLY)
    install(FILES ${CMAKE_BINARY_DIR}/sdl12_compat.pc
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
    )

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
  set_target_properties(SDL-static PROPERTIES COMPILE_DEFINITIONS "_REENTRANT")
  target_link_libraries(SDL-static PRIVATE dl)
  set_target_properties(SDL-static PROPERTIES
          VERSION "${SDL12_COMPAT_VERSION_STR}"
        OUTPUT_NAME "SDL")

  install(TARGETS SDL-static
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  )
endif()
