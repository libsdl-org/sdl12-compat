/* this isn't a classic SDL 1.2 test program, this tests a little
   compatibility magic that only exists in sdl12-compat. */

#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"
#include "SDL_syswm.h"

/* so this will compile with classic 1.2 headers... */
#ifndef SDL12_COMPAT_HEADERS
#include "begin_code.h"
typedef struct SDL_SysWMinfo2on12
{
    SDL_version version;
    Uint32 subsystem;
    void *data1;
    void *data2;
    void *data3;
    void *data4;
    void *data5;
    void *data6;
    void *data7;
    void *data8;
} SDL_SysWMinfo2on12;
#include "end_code.h"
#endif

typedef enum
{
    SDL20_SYSWM_UNKNOWN,
    SDL20_SYSWM_WINDOWS,
    SDL20_SYSWM_X11,
    SDL20_SYSWM_DIRECTFB,
    SDL20_SYSWM_COCOA,
    SDL20_SYSWM_UIKIT,
    SDL20_SYSWM_WAYLAND,
    SDL20_SYSWM_MIR,
    SDL20_SYSWM_WINRT,
    SDL20_SYSWM_ANDROID,
    SDL20_SYSWM_VIVANTE,
    SDL20_SYSWM_OS2,
    SDL20_SYSWM_HAIKU,
    SDL20_SYSWM_KMSDRM,
    SDL20_SYSWM_RISCOS
} SDL20_SYSWM_TYPE;

static const char *
subsystem_name(const Uint32 typ)
{
    static char unrecognized[64];
    switch ((SDL20_SYSWM_TYPE) typ) {
        case SDL20_SYSWM_UNKNOWN: return "Unknown";
        case SDL20_SYSWM_WINDOWS: return "Windows";
        case SDL20_SYSWM_X11: return "X11";
        case SDL20_SYSWM_DIRECTFB: return "DirectFB";
        case SDL20_SYSWM_COCOA: return "Cocoa";
        case SDL20_SYSWM_UIKIT: return "UIKit";
        case SDL20_SYSWM_WAYLAND: return "Wayland";
        case SDL20_SYSWM_MIR: return "Mir";
        case SDL20_SYSWM_WINRT: return "WinRT";
        case SDL20_SYSWM_ANDROID: return "Android";
        case SDL20_SYSWM_VIVANTE: return "Vivante";
        case SDL20_SYSWM_OS2: return "OS/2";
        case SDL20_SYSWM_HAIKU: return "Haiku";
        case SDL20_SYSWM_KMSDRM: return "KMSDRM";
        case SDL20_SYSWM_RISCOS: return "RiscOS";
        default: break;
    }

    snprintf(unrecognized, sizeof (unrecognized), "Unrecognized (%d)", (int) typ);
    return unrecognized;
}

int main(int argc, char **argv)
{
    SDL_SysWMinfo2on12 syswm_info;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Couldn't initialize SDL: %s\n", SDL_GetError());
		return 1;
	}

    if (SDL_SetVideoMode(640, 480, 32, 0) == NULL) {
		fprintf(stderr, "Couldn't create an SDL window: %s\n", SDL_GetError());
        SDL_Quit();
		return 1;
	}

    SDL_memset(&syswm_info, '\0', sizeof (syswm_info));
    syswm_info.version.major = 2;  /* this triggers magic in sdl12-compat */

    /* obviously this should fail with classic SDL 1.2. */
	if (SDL_GetWMInfo((SDL_SysWMinfo *) &syswm_info) != 1) {
	    fprintf(stderr, "Failed to get syswm 2on12 info: %s\n", SDL_GetError());
	} else {
        printf("SysWM2on12 info:\n");
        printf("Version: %d.%d.%d\n", syswm_info.version.major, syswm_info.version.minor, syswm_info.version.patch);
        printf("Subsystem: %s\n", subsystem_name(syswm_info.subsystem));
        printf("data1: %p\n", syswm_info.data1);
        printf("data2: %p\n", syswm_info.data2);
        printf("data3: %p\n", syswm_info.data3);
        printf("data4: %p\n", syswm_info.data4);
        printf("data5: %p\n", syswm_info.data5);
        printf("data6: %p\n", syswm_info.data6);
        printf("data7: %p\n", syswm_info.data7);
        printf("data8: %p\n", syswm_info.data8);
        printf("sdl2_window: %p\n", syswm_info.sdl2_window);
    }

    SDL_Quit();
    return 0;
}

