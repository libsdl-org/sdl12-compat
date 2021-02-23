cmake_minimum_required(VERSION 3.0.0)
project(sdl12_compat)

if(APPLE)
  set(OSX_SRCS "src/SDL12_compat_objc.m")
  set_source_files_properties(${OSX_SRCS} PROPERTIES LANGUAGE C)
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

target_include_directories(SDL PRIVATE "/usr/local/include/SDL2")  # !!! FIXME

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
    target_link_libraries(SDL PRIVATE "-framework AppKit")
endif()

if(APPLE)
    set_target_properties(SDL PROPERTIES
        MACOSX_RPATH 1
        OUTPUT_NAME "SDL-1.2.0"
    )
elseif(UNIX AND NOT ANDROID)
    set_target_properties(SDL PROPERTIES
        VERSION "1.2.50"
        SOVERSION "0"
        OUTPUT_NAME "SDL-1.2")
else()
    set_target_properties(SDL PROPERTIES
        VERSION "1.2.50"
        SOVERSION "0"
      OUTPUT_NAME "SDL")
endif()

if(MINGW)
    set_target_properties(SDL PROPERTIES LINK_FLAGS "-nostdlib")
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

# test programs...
macro(test_program _NAME _SRCS)
    add_executable(${_NAME} ${_SRCS})
    target_include_directories(${_NAME} PUBLIC "/usr/local/include/SDL")  # !!! FIXME
#    target_include_directories(${_NAME} PUBLIC "/usr/X11/include")  # !!! FIXME
    target_link_libraries(${_NAME} SDL)
    target_link_libraries(${_NAME} m)
endmacro()

test_program(testsprite "test/testsprite.c")
