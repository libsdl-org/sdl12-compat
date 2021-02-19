cmake_minimum_required(VERSION 3.0.0)
project(sdl12_compat)

add_library(SDL SHARED "src/SDL12_compat.c")

target_include_directories(SDL PRIVATE "/usr/local/include/SDL2")  # !!! FIXME
#target_include_directories(SDL PRIVATE "/usr/X11/include")  # !!! FIXME
add_definitions("-D_THREAD_SAFE")  # !!! FIXME

# avoid DLL having 'lib' prefix with Windows MinGW builds
if(WIN32)
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
endif()

if(UNIX OR APPLE)
    target_link_libraries(SDL PRIVATE dl)
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

if(MSVC)
    # Don't try to link with the default set of libraries.
    set_target_properties(SDL PROPERTIES LINK_FLAGS_RELEASE "/NODEFAULTLIB")
    set_target_properties(SDL PROPERTIES LINK_FLAGS_DEBUG "/NODEFAULTLIB")
    set_target_properties(SDL PROPERTIES STATIC_LIBRARY_FLAGS "/NODEFAULTLIB")
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
