/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2019 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/* This file contains functions for backwards compatibility with SDL 1.2 */

// !!! FIXME: clean up code conventions
// !!! FIXME: grep for VideoWindow20 places that might care if it's NULL

#include "SDL20_include_wrapper.h"

#if !SDL_VERSION_ATLEAST(2,0,0)
#error You need to compile against SDL 2.0 headers.
#endif

/*
 * We report the library version as 1.2.$(SDL12_COMPAT_VERSION). This number
 *  should be way ahead of what SDL-1.2 Classic would report, so apps can
 *  decide if they're running under the compat layer, if they really care.
 */
#define SDL12_COMPAT_VERSION 50

#include <stdarg.h>

// !!! IMPLEMENT_ME SDL_CDClose
// !!! IMPLEMENT_ME SDL_CDEject
// !!! IMPLEMENT_ME SDL_CDName
// !!! IMPLEMENT_ME SDL_CDNumDrives
// !!! IMPLEMENT_ME SDL_CDOpen
// !!! IMPLEMENT_ME SDL_CDPause
// !!! IMPLEMENT_ME SDL_CDPlay
// !!! IMPLEMENT_ME SDL_CDPlayTracks
// !!! IMPLEMENT_ME SDL_CDResume
// !!! IMPLEMENT_ME SDL_CDStatus
// !!! IMPLEMENT_ME SDL_CDStop
// !!! IMPLEMENT_ME SDL_CreateYUVOverlay

// !!! IMPLEMENT_ME SDL_DisplayFormatAlpha
// !!! IMPLEMENT_ME SDL_DisplayYUVOverlay
// !!! IMPLEMENT_ME SDL_EnableKeyRepeat
// !!! IMPLEMENT_ME SDL_EnableUNICODE
// !!! IMPLEMENT_ME SDL_FreeYUVOverlay

// !!! IMPLEMENT_ME SDL_GL_Lock
// !!! IMPLEMENT_ME SDL_GL_Unlock
// !!! IMPLEMENT_ME SDL_GL_UpdateRects

// !!! IMPLEMENT_ME SDL_GetKeyName
// !!! IMPLEMENT_ME SDL_GetKeyState
// !!! IMPLEMENT_ME SDL_GetModState
// !!! IMPLEMENT_ME SDL_GetRelativeMouseState

// !!! IMPLEMENT_ME SDL_GetVideoSurface
// !!! IMPLEMENT_ME SDL_GetWMInfo

// !!! IMPLEMENT_ME SDL_LockSurface
// !!! IMPLEMENT_ME SDL_LockYUVOverlay
// !!! IMPLEMENT_ME SDL_LowerBlit

// !!! IMPLEMENT_ME SDL_SetAlpha
// !!! IMPLEMENT_ME SDL_SetColorKey
// !!! IMPLEMENT_ME SDL_SetColors

// !!! IMPLEMENT_ME SDL_SetModState
// !!! IMPLEMENT_ME SDL_SetPalette
// !!! IMPLEMENT_ME SDL_SetVideoMode
// !!! IMPLEMENT_ME SDL_SoftStretch
// !!! IMPLEMENT_ME SDL_UnlockSurface
// !!! IMPLEMENT_ME SDL_UnlockYUVOverlay
// !!! IMPLEMENT_ME SDL_UpdateRects
// !!! IMPLEMENT_ME SDL_UpperBlit

// !!! FIXME: should SDL_VideoInit really be a passthrough?
// !!! FIXME: should SDL_VideoQuit really be a passthrough?

// !!! IMPLEMENT_ME SDL_WM_SetIcon
// !!! IMPLEMENT_ME SDL_WM_ToggleFullScreen

// !!! IMPLEMENT_ME X11_KeyToUnicode

#define SDL_BlitSurface SDL_UpperBlit


#if 0
#define FIXME(x) do {} while (0)
#else
#define FIXME(x) \
    do { \
        static SDL_bool seen = SDL_FALSE; \
        if (!seen) { \
            fprintf(stderr, "FIXME: %s (%s:%d)\n", x, __FILE__, __LINE__); \
            seen = SDL_TRUE; \
        } \
    } while (0)
#endif

#define SDL20_SYM(rc,fn,params,args,ret) \
    typedef rc (SDLCALL *SDL20_##fn##_t) params; \
    static SDL20_##fn##_t SDL20_##fn = NULL;
#include "SDL20_syms.h"

/* Things that _should_ be binary compatible pass right through... */
#define SDL20_SYM_PASSTHROUGH(rc,fn,params,args,ret) \
    DECLSPEC rc SDLCALL SDL_##fn params { ret SDL20_##fn args; }
#include "SDL20_syms.h"


/* these are macros (etc) in the SDL headers, so make our own. */
#define SDL20_OutOfMemory() SDL20_Error(SDL_ENOMEM)
#define SDL20_Unsupported() SDL20_Error(SDL_UNSUPPORTED)
#define SDL20_InvalidParamError(param) SDL20_SetError("Parameter '%s' is invalid", (param))
#define SDL20_zero(x) SDL20_memset(&(x), 0, sizeof((x)))
#define SDL20_zerop(x) SDL20_memset((x), 0, sizeof(*(x)))
#define SDL_ReportAssertion SDL20_ReportAssertion

#define SDL12_DEFAULT_REPEAT_DELAY 500
#define SDL12_DEFAULT_REPEAT_INTERVAL 30

#define SDL12_INIT_TIMER       0x00000001
#define SDL12_INIT_AUDIO       0x00000010
#define SDL12_INIT_VIDEO       0x00000020
#define SDL12_INIT_CDROM       0x00000100
#define SDL12_INIT_JOYSTICK    0x00000200
#define SDL12_INIT_NOPARACHUTE 0x00100000
#define SDL12_INIT_EVENTTHREAD 0x01000000
#define SDL12_INIT_EVERYTHING  0x0000FFFF

typedef struct SDL12_Palette
{
    int       ncolors;
    SDL_Color *colors;
} SDL12_Palette;

typedef struct SDL12_PixelFormat
{
    SDL12_Palette *palette;
    Uint8 BitsPerPixel;
    Uint8 BytesPerPixel;
    Uint8 Rloss;
    Uint8 Gloss;
    Uint8 Bloss;
    Uint8 Aloss;
    Uint8 Rshift;
    Uint8 Gshift;
    Uint8 Bshift;
    Uint8 Ashift;
    Uint32 Rmask;
    Uint32 Gmask;
    Uint32 Bmask;
    Uint32 Amask;
    Uint32 colorkey;
    Uint8 alpha;
} SDL12_PixelFormat;

typedef struct SDL12_Surface
{
    Uint32 flags;
    SDL12_PixelFormat *format;
    int w;
    int h;
    Uint16 pitch;
    void *pixels;
    int offset;
    SDL_Surface *surface20; /* the real SDL 1.2 has an opaque pointer to a platform-specific thing here named "hwdata". */
    SDL_Rect clip_rect;
    Uint32 unused1;
    Uint32 locked;
    void *blitmap;
    unsigned int format_version;
    int refcount;
} SDL12_Surface;

typedef struct SDL12_Overlay
{
    Uint32 format;
    int w;
    int h;
    int planes;
    Uint16 *pitches;
    Uint8 **pixels;
    void *hwfuncs;
    void *hwdata;
    Uint32 hw_overlay :1;
    Uint32 UnusedBits :31;
} SDL12_Overlay;

typedef struct
{
    Uint32 hw_available :1;
    Uint32 wm_available :1;
    Uint32 UnusedBits1  :6;
    Uint32 UnusedBits2  :1;
    Uint32 blit_hw      :1;
    Uint32 blit_hw_CC   :1;
    Uint32 blit_hw_A    :1;
    Uint32 blit_sw      :1;
    Uint32 blit_sw_CC   :1;
    Uint32 blit_sw_A    :1;
    Uint32 blit_fill    :1;
    Uint32 UnusedBits3  :16;
    Uint32 video_mem;
    SDL_PixelFormat *vfmt;
    int current_w;
    int current_h;
} SDL12_VideoInfo;


#define SDL12_HWSURFACE 0x00000001
#define SDL12_ASYNCBLIT 0x00000004
#define SDL12_ANYFORMAT 0x10000000
#define SDL12_HWPALETTE 0x20000000
#define SDL12_DOUBLEBUF 0x40000000
#define SDL12_FULLSCREEN 0x80000000
#define SDL12_OPENGL 0x00000002
#define SDL12_OPENGLBLIT 0x0000000A
#define SDL12_RESIZABLE 0x00000010
#define SDL12_NOFRAME 0x00000020
#define SDL12_HWACCEL 0x00000100
#define SDL12_SRCCOLORKEY 0x00001000
#define SDL12_RLEACCELOK 0x00002000
#define SDL12_RLEACCEL 0x00004000
#define SDL12_SRCALPHA 0x00010000
#define SDL12_PREALLOC 0x01000000

typedef enum
{
    SDL12_NOEVENT = 0,
    SDL12_ACTIVEEVENT,
    SDL12_KEYDOWN,
    SDL12_KEYUP,
    SDL12_MOUSEMOTION,
    SDL12_MOUSEBUTTONDOWN,
    SDL12_MOUSEBUTTONUP,
    SDL12_JOYAXISMOTION,
    SDL12_JOYBALLMOTION,
    SDL12_JOYHATMOTION,
    SDL12_JOYBUTTONDOWN,
    SDL12_JOYBUTTONUP,
    SDL12_QUIT,
    SDL12_SYSWMEVENT,
    SDL12_EVENT_RESERVEDA,
    SDL12_EVENT_RESERVEDB,
    SDL12_VIDEORESIZE,
    SDL12_VIDEOEXPOSE,
    SDL12_USEREVENT = 24,
    SDL12_NUMEVENTS = 32
} SDL12_EventType;


#define SDL12_APPMOUSEFOCUS (1<<0)
#define SDL12_APPINPUTFOCUS (1<<1)
#define SDL12_APPACTIVE     (1<<2)

typedef struct
{
    Uint8 type;
    Uint8 gain;
    Uint8 state;
} SDL12_ActiveEvent;

typedef struct
{
    Uint8 type;
    Uint8 which;
    Uint8 state;
    //FIXME: SDL12_keysym keysym;
} SDL12_KeyboardEvent;

typedef struct
{
    Uint8 type;
    Uint8 which;
    Uint8 state;
    Uint16 x, y;
    Sint16 xrel;
    Sint16 yrel;
} SDL12_MouseMotionEvent;

typedef struct
{
    Uint8 type;
    Uint8 which;
    Uint8 button;
    Uint8 state;
    Uint16 x, y;
} SDL12_MouseButtonEvent;

typedef struct
{
    Uint8 type;
    Uint8 which;
    Uint8 axis;
    Sint16 value;
} SDL12_JoyAxisEvent;

typedef struct
{
    Uint8 type;
    Uint8 which;
    Uint8 ball;
    Sint16 xrel;
    Sint16 yrel;
} SDL12_JoyBallEvent;

typedef struct
{
    Uint8 type;
    Uint8 which;
    Uint8 hat;
    Uint8 value;
} SDL12_JoyHatEvent;

typedef struct
{
    Uint8 type;
    Uint8 which;
    Uint8 button;
    Uint8 state;
} SDL12_JoyButtonEvent;

typedef struct
{
    Uint8 type;
    int w;
    int h;
} SDL12_ResizeEvent;

typedef struct
{
    Uint8 type;
} SDL12_ExposeEvent;

typedef struct
{
    Uint8 type;
} SDL12_QuitEvent;

typedef struct
{
    Uint8 type;
    int code;
    void *data1;
    void *data2;
} SDL12_UserEvent;

typedef struct
{
    Uint8 type;
    void *msg;
} SDL12_SysWMEvent;

typedef union
{
    Uint8 type;
    SDL12_ActiveEvent active;
    SDL12_KeyboardEvent key;
    SDL12_MouseMotionEvent motion;
    SDL12_MouseButtonEvent button;
    SDL12_JoyAxisEvent jaxis;
    SDL12_JoyBallEvent jball;
    SDL12_JoyHatEvent jhat;
    SDL12_JoyButtonEvent jbutton;
    SDL12_ResizeEvent resize;
    SDL12_ExposeEvent expose;
    SDL12_QuitEvent quit;
    SDL12_UserEvent user;
    SDL12_SysWMEvent syswm;
} SDL12_Event;

typedef int (SDLCALL *SDL12_EventFilter)(const SDL12_Event *event12);
static int EventFilter20to12(void *data, SDL_Event *event20);

typedef Uint32 (SDLCALL *SDL12_TimerCallback)(Uint32 interval);
typedef SDL_TimerCallback SDL12_NewTimerCallback;

typedef struct
{
    SDL_Rect area;
    Sint16 hot_x;
    Sint16 hot_y;
    Uint8 *data;
    Uint8 *mask;
    Uint8 *save[2];
    SDL_Cursor *wm_cursor;  /* the real SDL 1.2 has an opaque pointer to a platform-specific cursor here. */
} SDL12_Cursor;

typedef enum
{
    SDL12_GL_RED_SIZE,
    SDL12_GL_GREEN_SIZE,
    SDL12_GL_BLUE_SIZE,
    SDL12_GL_ALPHA_SIZE,
    SDL12_GL_BUFFER_SIZE,
    SDL12_GL_DOUBLEBUFFER,
    SDL12_GL_DEPTH_SIZE,
    SDL12_GL_STENCIL_SIZE,
    SDL12_GL_ACCUM_RED_SIZE,
    SDL12_GL_ACCUM_GREEN_SIZE,
    SDL12_GL_ACCUM_BLUE_SIZE,
    SDL12_GL_ACCUM_ALPHA_SIZE,
    SDL12_GL_STEREO,
    SDL12_GL_MULTISAMPLEBUFFERS,
    SDL12_GL_MULTISAMPLESAMPLES,
    SDL12_GL_ACCELERATED_VISUAL,
    SDL12_GL_SWAP_CONTROL,
    SDL12_GL_MAX_ATTRIBUTE
} SDL12_GLattr;


typedef struct
{
    Uint32 format;
    SDL_Rect *modeslist;
    SDL_Rect **modes;  /* ptrs to each item in modeslist, for SDL_ListModes() */
} VideoModeList;

// !!! FIXME: go through all of these.
static VideoModeList *VideoModes = NULL;
static int VideoModesCount = 0;
static SDL12_VideoInfo VideoInfo12;
static SDL_Window *VideoWindow20 = NULL;
static SDL_Renderer *VideoRenderer20 = NULL;
static SDL_Texture *VideoTexture20 = NULL;
static SDL12_Surface *VideoSurface12 = NULL;
static SDL_Surface *VideoConvertSurface20 = NULL;
static SDL_GLContext *VideoGLContext20 = NULL;
static char *WindowTitle = NULL;
static char *WindowIconTitle = NULL;
static SDL12_Surface *VideoIcon12;
static int EnabledUnicode = 0;
static int VideoDisplayIndex = 0;
static int CDRomInit = 0;
static SDL12_EventFilter EventFilter12 = NULL;
static SDL12_Cursor *CurrentCursor12 = NULL;
static Uint8 EventStates[SDL12_NUMEVENTS];
static int SwapInterval = 0;

// !!! FIXME: need a mutex for the event queue.
#define SDL12_MAXEVENTS 128
typedef struct EventQueueType
{
    SDL12_Event event12;
    struct EventQueueType *next;
} EventQueueType;

static EventQueueType EventQueuePool[SDL12_MAXEVENTS];
static EventQueueType *EventQueueHead = NULL;
static EventQueueType *EventQueueTail = NULL;
static EventQueueType *EventQueueAvailable = NULL;


/* Obviously we can't use SDL_LoadObject() to load SDL2.  :)  */
#if defined(_WINDOWS)
    #define WIN32_LEAN_AND_MEAN 1
    #include <windows.h>
    #define SDL20_LIBNAME "SDL2.dll"
    static HANDLE Loaded_SDL20 = NULL;
    #define LoadSDL20Library() ((Loaded_SDL20 = LoadLibraryA(SDL20_LIBNAME)) != NULL)
    #define LookupSDL20Sym(sym) GetProcAddress(Loaded_SDL20, sym)
    #define CloseSDL20Library() { { if (Loaded_SDL20) { FreeLibrary(Loaded_SDL20); Loaded_SDL20 = NULL; } }
#elif defined(unix) || defined(__APPLE__)
    #include <dlfcn.h>
    #ifdef __APPLE__
    #define SDL20_LIBNAME "libSDL2.dylib"
    #else
    #define SDL20_LIBNAME "libSDL2-2.0.so.0"
    #endif
    static void *Loaded_SDL20 = NULL;
    #define LoadSDL20Library() ((Loaded_SDL20 = dlopen(SDL20_LIBNAME, RTLD_LOCAL)) != NULL)
    #define LookupSDL20Sym(sym) dlsym(Loaded_SDL20, sym)
    #define CloseSDL20Library() { if (Loaded_SDL20) { dlclose(Loaded_SDL20); Loaded_SDL20 = NULL; } }
#else
    #error Please define your platform.
#endif

static void *
LoadSDL20Symbol(const char *fn, int *okay)
{
    void *retval = NULL;
    if (*okay)  /* only bother trying if we haven't previously failed. */
    {
        retval = LookupSDL20Sym(fn);
if (!retval) { fprintf(stderr, "WARNING: LOAD FAILED: %s\n", fn); }
//        if (retval == NULL)
//            *okay = 0;
    }
    return retval;
}

static void
UnloadSDL20(void)
{
    #define SDL20_SYM(rc,fn,params,args,ret) SDL20_##fn = NULL;
    #include "SDL20_syms.h"
    CloseSDL20Library();
}

static int
LoadSDL20(void)
{
    int okay = 1;
    if (!Loaded_SDL20)
    {
        okay = LoadSDL20Library();
        #define SDL20_SYM(rc,fn,params,args,ret) SDL20_##fn = (SDL20_##fn##_t) LoadSDL20Symbol("SDL_" #fn, &okay);
        #include "SDL20_syms.h"
        if (!okay)
            UnloadSDL20();
    }
    return okay;
}

DECLSPEC const SDL_version * SDLCALL
SDL_Linked_Version(void)
{
    static const SDL_version version = { 1, 2, SDL12_COMPAT_VERSION };
    return &version;
}

DECLSPEC int SDLCALL
SDL_sscanf(const char *text, const char *fmt, ...)
{
    int retval;
    va_list ap;
    va_start(ap, fmt);
    retval = (int) SDL20_sscanf(text, fmt, ap);
    va_end(ap);
    return retval;
}

DECLSPEC int SDLCALL
SDL_snprintf(char *text, size_t maxlen, const char *fmt, ...)
{
    int retval;
    va_list ap;
    va_start(ap, fmt);
    retval = (int) SDL20_vsnprintf(text, maxlen, fmt, ap);
    va_end(ap);
    return retval;
}

DECLSPEC SDL_bool SDLCALL
SDL_HasMMXExt(void)
{
    /* this isn't accurate, but SDL2 doesn't have this for some reason.
        MMXExt is available in all SSE1 machines, except early Athlon chips,
        so we'll just say it's available if they have SSE1. Oh well. */
    return SDL20_HasSSE();
}

DECLSPEC SDL_bool SDLCALL
SDL_Has3DNowExt(void)
{
    FIXME("check this");
    return SDL20_HasSSE();
}

DECLSPEC int SDLCALL SDL_JoystickOpened(int device_index)
{
    FIXME("write me");
    return 0;
}

static int
GetVideoDisplay()
{
    FIXME("cache this value during SDL_Init() so it doesn't change.");
    const char *variable = SDL20_getenv("SDL_VIDEO_FULLSCREEN_DISPLAY");
    if ( !variable ) {
        variable = SDL20_getenv("SDL_VIDEO_FULLSCREEN_HEAD");
    }
    if ( variable ) {
        return SDL20_atoi(variable);
    } else {
        return 0;
    }
}

/* This sets up VideoModes and VideoModesCount. You end up with arrays by pixel
    format, each with a value that 1.2's SDL_ListModes() can return. */
static int
Init12VidModes(void)
{
    const int total = SDL20_GetNumDisplayModes(VideoDisplayIndex);
    VideoModeList *vmode = NULL;
    int num_modes = 0;
    void *ptr = NULL;
    int i, j;

    SDL_assert(VideoModes == NULL);
    SDL_assert(VideoModesCount == 0);

    for (i = 0; i < total; ++i) {
        SDL_DisplayMode mode;

        if (SDL20_GetDisplayMode(VideoDisplayIndex, i, &mode) == -1) {
            continue;
        } else if (!mode.w || !mode.h) {
            SDL_assert(0 && "Can this actually happen?");
            continue;
        }

        if (!vmode || (mode.format != vmode->format)) {  // SDL20_GetDisplayMode() sorts on bpp first. We know when to change arrays.
            if (VideoModesCount > 0) {
                VideoModes[VideoModesCount-1].modes[num_modes] = NULL;
            }
            ptr = (VideoModeList *) SDL20_realloc(VideoModes, sizeof (VideoModeList) * (VideoModesCount+1));
            if (!ptr) {
                return SDL20_OutOfMemory();
            }
            VideoModes = (VideoModeList *) ptr;
            vmode = &VideoModes[VideoModesCount];
            vmode->format = mode.format;
            vmode->modeslist = NULL;
            vmode->modes = NULL;
            VideoModesCount++;
            num_modes = 0;
        }

        /* make sure we don't have this one already (with a different refresh rate, etc). */
        for (j = 0; j < num_modes; j++) {
            if ((vmode->modeslist[j].w == mode.w) && (vmode->modeslist[j].h == mode.h)) {
                break;
            }
        }

        if (j < num_modes) {
            continue;  /* already have this one. */
        }

        ptr = SDL20_realloc(vmode->modes, sizeof (SDL_Rect *) * (num_modes + 2));
        if (ptr == NULL) {
            return SDL20_OutOfMemory();
        }
        vmode->modes = (SDL_Rect **) ptr;

        ptr = SDL20_realloc(vmode->modeslist, sizeof (SDL_Rect) * (num_modes + 1));
        if (ptr == NULL) {
            return SDL20_OutOfMemory();
        }
        vmode->modeslist = (SDL_Rect *) ptr;

        vmode->modeslist[num_modes].x = 0;
        vmode->modeslist[num_modes].y = 0;
        vmode->modeslist[num_modes].w = mode.w;
        vmode->modeslist[num_modes].h = mode.h;

        vmode->modes[num_modes] = &vmode->modeslist[num_modes];

        num_modes++;
    }

    if (VideoModesCount > 0) {
        VideoModes[VideoModesCount-1].modes[num_modes] = NULL;
    }

    return 0;
}

static int
Init12Video(void)
{
    int i;

    for (i = 0; i < SDL12_MAXEVENTS-1; i++)
        EventQueuePool[i].next = &EventQueuePool[i+1];
    EventQueuePool[SDL12_MAXEVENTS-1].next = NULL;

    EventQueueHead = EventQueueTail = NULL;
    EventQueueAvailable = EventQueuePool;

    SDL_memset(EventStates, SDL_ENABLE, sizeof (EventStates)); /* on by default */
    EventStates[SDL12_SYSWMEVENT] = SDL_IGNORE;  /* off by default. */

    SDL20_SetEventFilter(EventFilter20to12, NULL);

    VideoDisplayIndex = GetVideoDisplay();
    SwapInterval = 0;

    if (Init12VidModes() == -1) {
        return -1;
    }

    return 0;
}


DECLSPEC int SDLCALL
SDL_InitSubSystem(Uint32 sdl12flags)
{
    FIXME("there is never a parachute in SDL2, should we catch segfaults ourselves?");

    FIXME("support event thread where it makes sense to do so?");

	if ( (sdl12flags & SDL12_INIT_EVENTTHREAD) == SDL12_INIT_EVENTTHREAD ) {
		return SDL20_SetError("OS doesn't support threaded events");
	}

    Uint32 sdl20flags = 0;
    int rc;

    if (!LoadSDL20())
        return -1;

#ifdef __MACOSX__
    extern void sdl12_compat_macos_init(void);
    sdl12_compat_macos_init();
#endif

    #define SETFLAG(flag) if (sdl12flags & SDL12_INIT_##flag) sdl20flags |= SDL_INIT_##flag
    SETFLAG(TIMER);
    SETFLAG(AUDIO);
    SETFLAG(VIDEO);
    SETFLAG(JOYSTICK);
    SETFLAG(NOPARACHUTE);
    #undef SETFLAG

    // There's no CDROM in 2.0, but we'll just pretend it succeeded.
    if (sdl12flags & SDL12_INIT_CDROM)
        CDRomInit = 1;

    FIXME("do something about SDL12_INIT_EVENTTHREAD");

    rc = SDL20_Init(sdl20flags);
    if ((rc == 0) && (sdl20flags & SDL_INIT_VIDEO)) {
        if (Init12Video() == -1) {
            FIXME("should we deinit other subsystems?");
            return -1;
        }
    }

    return rc;
}

DECLSPEC int SDLCALL
SDL_Init(Uint32 sdl12flags)
{
    FIXME("what was different in 1.2?");
    return SDL_InitSubSystem(sdl12flags);   /* there's no difference betwee Init and InitSubSystem in SDL2. */
}


static void
InitFlags12To20(const Uint32 flags12, Uint32 *_flags20, Uint32 *_extraflags)
{
    Uint32 flags20 = 0;
    Uint32 extraflags = 0;

    #define SETFLAG(flag) if (flags12 & SDL12_INIT_##flag) flags20 |= SDL_INIT_##flag
    SETFLAG(TIMER);
    SETFLAG(AUDIO);
    SETFLAG(VIDEO);
    SETFLAG(JOYSTICK);
    SETFLAG(NOPARACHUTE);
    #undef SETFLAG

    if ((flags12 & SDL12_INIT_CDROM) && (CDRomInit)) {
        extraflags |= SDL12_INIT_CDROM;
    }

    FIXME("do something about SDL12_INIT_EVENTTHREAD");

    *_flags20 = flags20;
    *_extraflags = extraflags;
}

static Uint32
InitFlags20to12(const Uint32 flags20)
{
    Uint32 flags12 = 0;

    #define SETFLAG(flag) if (flags20 & SDL_INIT_##flag) flags12 |= SDL12_INIT_##flag
    SETFLAG(TIMER);
    SETFLAG(AUDIO);
    SETFLAG(VIDEO);
    SETFLAG(JOYSTICK);
    SETFLAG(NOPARACHUTE);
    #undef SETFLAG

    return flags12;
}


DECLSPEC Uint32 SDLCALL
SDL_WasInit(Uint32 sdl12flags)
{
    Uint32 sdl20flags, extraflags;
    InitFlags12To20(sdl12flags, &sdl20flags, &extraflags);

    return InitFlags20to12(SDL20_WasInit(sdl20flags)) | extraflags;
}

static void
Quit12Video(void)
{
    int i;

    for (i = 0; i < VideoModesCount; i++) {
        SDL20_free(VideoModes[i].modeslist);
        SDL20_free(VideoModes[i].modes);
    }
    SDL20_free(VideoModes);

    SDL20_FreeFormat(VideoInfo12.vfmt);
    SDL20_zero(VideoInfo12);

    EventFilter12 = NULL;
    EventQueueAvailable = EventQueueHead = EventQueueTail = NULL;
    CurrentCursor12 = NULL;
    VideoModes = NULL;
    VideoModesCount = 0;
}

DECLSPEC void SDLCALL
SDL_QuitSubSystem(Uint32 sdl12flags)
{
    Uint32 sdl20flags, extraflags;
    InitFlags12To20(sdl12flags, &sdl20flags, &extraflags);

    if (extraflags & SDL12_INIT_CDROM) {
        CDRomInit = 0;
    }

    FIXME("reset a bunch of other global variables too.");
    if (sdl12flags & SDL12_INIT_VIDEO) {
        Quit12Video();
    }

    FIXME("do something about SDL12_INIT_EVENTTHREAD");
    SDL20_QuitSubSystem(sdl20flags);

    if ((SDL20_WasInit(0) == 0) && (!CDRomInit)) {
        SDL20_Quit();
        UnloadSDL20();
    }
}

DECLSPEC void SDLCALL
SDL_Quit(void)
{
    SDL_QuitSubSystem(SDL_WasInit(0) | SDL12_INIT_CDROM);
}

DECLSPEC void SDLCALL
SDL_SetError(const char *fmt, ...)
{
    char ch;
    char *str = NULL;
    size_t len = 0;
    va_list ap;
    va_start(ap, fmt);
    len = SDL20_vsnprintf(&ch, 1, fmt, ap);
    va_end(ap);

    str = (char *) SDL20_malloc(len + 1);
    if (!str)
        SDL20_OutOfMemory();
    else
    {
        va_start(ap, fmt);
        SDL20_vsnprintf(str, len + 1, fmt, ap);
        va_end(ap);
        SDL20_SetError("%s", str);
        SDL20_free(str);
    }
}

DECLSPEC const char * SDLCALL
SDL_GetError(void)
{
    if (SDL20_GetError == NULL)
    {
        static const char noload_errstr[] = "The SDL 2.0 library that the 1.2 compatibility layer needs isn't loaded";
        return noload_errstr;
    }
    return SDL20_GetError();
}


static const char *
GetDriverName(const char *name, char *namebuf, int maxlen)
{
    if (name) {
        if (namebuf) {
            SDL20_strlcpy(namebuf, name, maxlen);
            return namebuf;
        } else {
            return name;
        }
    }
    return NULL;
}

DECLSPEC const char * SDLCALL
SDL_AudioDriverName(char *namebuf, int maxlen)
{
    return GetDriverName(SDL20_GetCurrentAudioDriver(), namebuf, maxlen);
}

DECLSPEC const char * SDLCALL
SDL_VideoDriverName(char *namebuf, int maxlen)
{
    return GetDriverName(SDL20_GetCurrentVideoDriver(), namebuf, maxlen);
}

DECLSPEC int SDLCALL
SDL_PollEvent(SDL12_Event *event12)
{
    EventQueueType *next;

    SDL20_PumpEvents();  /* this will run our filter and build our 1.2 queue. */

    if (EventQueueHead == NULL)
        return 0;  /* no events at the moment. */

    SDL_memcpy(event12, &EventQueueHead->event12, sizeof (SDL12_Event));
    next = EventQueueHead->next;
    EventQueueHead->next = EventQueueAvailable;
    EventQueueAvailable = EventQueueHead;
    EventQueueHead = next;
    return 1;
}

DECLSPEC int SDLCALL
SDL_PushEvent(SDL12_Event *event12)
{
    EventQueueType *item = EventQueueAvailable;
    if (item == NULL)
        return -1;  /* no space available at the moment. */

    EventQueueAvailable = item->next;
    if (EventQueueTail)
        EventQueueTail->next = item;
    else
        EventQueueHead = EventQueueTail = item;
    item->next = NULL;

    SDL_memcpy(&item->event12, event12, sizeof (SDL12_Event));
    return 0;
}

DECLSPEC int SDLCALL
SDL_PeepEvents(SDL12_Event *events12, int numevents, SDL_eventaction action, Uint32 mask)
{
    if (action == SDL_ADDEVENT)
    {
        int i;
        for (i = 0; i < numevents; i++)
        {
            if (SDL_PushEvent(&events12[i]) == -1)
                break;  /* out of space for more events. */
        }
        return i;
    }
    else if ((action == SDL_PEEKEVENT) || (action == SDL_GETEVENT))
    {
        const SDL_bool isGet = (action == SDL_GETEVENT);
        EventQueueType *prev = NULL;
        EventQueueType *item = EventQueueHead;
        EventQueueType *next = NULL;
        int chosen = 0;
        while (chosen < numevents)
        {
            EventQueueType *nextPrev = item;
            if (!item)
                break;  /* no more events at the moment. */

            next = item->next;  /* copy, since we might overwrite item->next */

            if (mask & (1<<item->event12.type))
            {
                SDL_memcpy(&events12[chosen++], &item->event12, sizeof (SDL12_Event));
                if (isGet)  /* remove from list? */
                {
                    if (prev != NULL)
                        prev->next = next;
                    if (item == EventQueueHead)
                        EventQueueHead = next;
                    if (item == EventQueueTail)
                        EventQueueTail = prev;

                    /* put it back in the free pool. */
                    item->next = EventQueueAvailable;
                    EventQueueAvailable = item;
                    nextPrev = prev;  /* previous item doesn't change. */
                }
            }

            item = next;
            prev = nextPrev;
        }
        return chosen;
    }

    return 0;
}

DECLSPEC int SDLCALL
SDL_WaitEvent(SDL12_Event *event12)
{
    FIXME("In 1.2, this only fails (-1) if you haven't SDL_Init()'d.");
    while (!SDL_PollEvent(event12))
        SDL20_Delay(10);
    return 1;
}

static SDL_bool
PushEventIfNotFiltered(SDL12_Event *event12)
{
    if (event12->type != SDL12_NOEVENT)
    {
        if (EventStates[event12->type] != SDL_IGNORE)
        {
            if ((!EventFilter12) || (EventFilter12(event12)))
                return (SDL_PushEvent(event12) == 0);
        }
    }
    return SDL_FALSE;
}

DECLSPEC Uint8 SDLCALL
SDL_EventState(Uint8 type, int state)
{
    /* the values of "state" match between 1.2 and 2.0 */
    const Uint8 retval = EventStates[type];
    SDL12_Event e;

    if (state != SDL_QUERY)
        EventStates[type] = state;
    if (state == SDL_IGNORE)  /* drop existing events of this type. */
        while (SDL_PeepEvents(&e, 1, SDL_GETEVENT, (1<<type))) {}

    return retval;
}

DECLSPEC Uint8 SDLCALL
SDL_GetMouseState(int *x, int *y)
{
    const Uint32 state20 = SDL20_GetMouseState(x, y);
    Uint8 retval = (state20 & 0x7);  /* left, right, and middle will match. */

    /* the X[12] buttons are different in 1.2; mousewheel was in the way. */
    if (state20 & SDL_BUTTON(SDL_BUTTON_X1))
        retval |= (1<<5);
    if (state20 & SDL_BUTTON(SDL_BUTTON_X2))
        retval |= (1<<6);

    return retval;
}

static int
EventFilter20to12(void *data, SDL_Event *event20)
{
    //const int maxUserEvents12 = SDL12_NUMEVENTS - SDL12_USEREVENT;
    SDL12_Event event12;
    int x, y;

    SDL_assert(data == NULL);  /* currently unused. */

    SDL20_zero(event12);

    switch (event20->type)
    {
        case SDL_QUIT:
            event12.type = SDL12_QUIT;
            break;

        case SDL_WINDOWEVENT:
            switch (event20->window.event)
            {
                case SDL_WINDOWEVENT_CLOSE:
                    event12.type = SDL12_QUIT;
                    break;

                case SDL_WINDOWEVENT_SHOWN:
                case SDL_WINDOWEVENT_EXPOSED:
                    event12.type = SDL12_VIDEOEXPOSE;
                    break;

                case SDL_WINDOWEVENT_RESIZED:
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    FIXME("what's the difference between RESIZED and SIZE_CHANGED?");
                    event12.type = SDL12_VIDEORESIZE;
                    event12.resize.w = event20->window.data1;
                    event12.resize.h = event20->window.data2;
                    break;

                case SDL_WINDOWEVENT_MINIMIZED:
                    event12.type = SDL12_ACTIVEEVENT;
                    event12.active.gain = 0;
                    event12.active.state = SDL12_APPACTIVE;
                    break;

                case SDL_WINDOWEVENT_RESTORED:
                    event12.type = SDL12_ACTIVEEVENT;
                    event12.active.gain = 1;
                    event12.active.state = SDL12_APPACTIVE;
                    break;

                case SDL_WINDOWEVENT_ENTER:
                    event12.type = SDL12_ACTIVEEVENT;
                    event12.active.gain = 1;
                    event12.active.state = SDL12_APPMOUSEFOCUS;
                    break;

                case SDL_WINDOWEVENT_LEAVE:
                    event12.type = SDL12_ACTIVEEVENT;
                    event12.active.gain = 0;
                    event12.active.state = SDL12_APPMOUSEFOCUS;
                    break;

                case SDL_WINDOWEVENT_FOCUS_GAINED:
                    event12.type = SDL12_ACTIVEEVENT;
                    event12.active.gain = 1;
                    event12.active.state = SDL12_APPINPUTFOCUS;
                    break;

                case SDL_WINDOWEVENT_FOCUS_LOST:
                    event12.type = SDL12_ACTIVEEVENT;
                    event12.active.gain = 0;
                    event12.active.state = SDL12_APPINPUTFOCUS;
                    break;
            }
            break;

        // !!! FIXME: this is sort of a mess to convert.
        //case SDL_SYSWMEVENT:

        case SDL_KEYDOWN: FIXME("write me"); return 0;
        case SDL_KEYUP: FIXME("write me"); return 0;

        case SDL_TEXTEDITING: FIXME("write me"); return 0;
        case SDL_TEXTINPUT: FIXME("write me"); return 0;

        case SDL_MOUSEMOTION:
        	event12.type = SDL12_MOUSEMOTION;
            event12.motion.which = (Uint8) event20->motion.which;
            event12.motion.state = event20->motion.state;
            event12.motion.x = (Uint16) event20->motion.x;
            event12.motion.y = (Uint16) event20->motion.y;
            event12.motion.xrel = (Sint16) event20->motion.xrel;
            event12.motion.yrel = (Sint16) event20->motion.yrel;
            break;

        case SDL_MOUSEBUTTONDOWN:
        	event12.type = SDL12_MOUSEBUTTONDOWN;
            event12.button.which = (Uint8) event20->button.which;
            event12.button.button = event20->button.button;
            event12.button.state = event20->button.state;
            event12.button.x = (Uint16) event20->button.x;
            event12.button.y = (Uint16) event20->button.y;
            break;

        case SDL_MOUSEBUTTONUP:
        	event12.type = SDL12_MOUSEBUTTONUP;
            event12.button.which = (Uint8) event20->button.which;
            event12.button.button = event20->button.button;
            event12.button.state = event20->button.state;
            event12.button.x = (Uint16) event20->button.x;
            event12.button.y = (Uint16) event20->button.y;
            break;

        case SDL_MOUSEWHEEL:
            if (event20->wheel.y == 0)
                break;  /* don't support horizontal wheels in 1.2. */

            event12.type = SDL12_MOUSEBUTTONDOWN;
            event12.button.which = (Uint8) event20->wheel.which;
            event12.button.button = (event20->wheel.y > 0) ? 4 : 5;  /* wheelup is 4, down is 5. */
            event12.button.state = SDL_GetMouseState(&x, &y);
            event12.button.x = (Uint16) x;
            event12.button.y = (Uint16) y;
            PushEventIfNotFiltered(&event12);

            event12.type = SDL12_MOUSEBUTTONUP;  /* immediately release mouse "button" at the end of this switch. */
            break;

        case SDL_JOYAXISMOTION:
            event12.type = SDL12_JOYAXISMOTION;
            event12.jaxis.which = (Uint8) event20->jaxis.which;
            event12.jaxis.axis = event20->jaxis.axis;
            event12.jaxis.value = event20->jaxis.value;
            break;

        case SDL_JOYBALLMOTION:
            event12.type = SDL12_JOYBALLMOTION;
            event12.jball.which = (Uint8) event20->jball.which;
            event12.jball.ball = event20->jball.ball;
            event12.jball.xrel = event20->jball.xrel;
            event12.jball.yrel = event20->jball.yrel;
            break;

        case SDL_JOYHATMOTION:
            event12.type = SDL12_JOYHATMOTION;
            event12.jhat.which = (Uint8) event20->jhat.which;
            event12.jhat.hat = event20->jhat.hat;
            event12.jhat.value = event20->jhat.value;
            break;

        case SDL_JOYBUTTONDOWN:
            event12.type = SDL12_JOYBUTTONDOWN;
            event12.jbutton.which = (Uint8) event20->jbutton.which;
            event12.jbutton.button = event20->jbutton.button;
            event12.jbutton.state = event20->jbutton.state;
            break;

        case SDL_JOYBUTTONUP:
            event12.type = SDL12_JOYBUTTONUP;
            event12.jbutton.which = (Uint8) event20->jbutton.which;
            event12.jbutton.button = event20->jbutton.button;
            event12.jbutton.state = event20->jbutton.state;
            break;

        //case SDL_JOYDEVICEADDED:
        //case SDL_JOYDEVICEREMOVED:
	    //case SDL_CONTROLLERAXISMOTION:
	    //case SDL_CONTROLLERBUTTONDOWN:
	    //case SDL_CONTROLLERBUTTONUP:
	    //case SDL_CONTROLLERDEVICEADDED:
	    //case SDL_CONTROLLERDEVICEREMOVED:
	    //case SDL_CONTROLLERDEVICEREMAPPED:
        //case SDL_FINGERDOWN:
        //case SDL_FINGERUP:
        //case SDL_FINGERMOTION:
        //case SDL_DOLLARGESTURE:
        //case SDL_DOLLARRECORD:
        //case SDL_MULTIGESTURE:
        //case SDL_CLIPBOARDUPDATE:
        //case SDL_DROPFILE:

        default:
            return 0;  /* drop everything else. */
    }

    PushEventIfNotFiltered(&event12);

    return 0;  /* always drop it from the 2.0 event queue. */
}

DECLSPEC void SDLCALL
SDL_SetEventFilter(SDL12_EventFilter filter12)
{
    /* We always have a filter installed, but will call the app's too. */
    EventFilter12 = filter12;
}

DECLSPEC SDL12_EventFilter SDLCALL
SDL_GetEventFilter(void)
{
    return EventFilter12;
}


static SDL12_Surface *
Surface20to12(SDL_Surface *surface20)
{
    SDL12_Surface *surface12 = NULL;
    SDL12_Palette *palette12 = NULL;
    SDL12_PixelFormat *format12 = NULL;
    Uint32 flags = 0;

    if (!surface20)
        return NULL;

    surface12 = (SDL12_Surface *) SDL20_malloc(sizeof (SDL12_Surface));
    if (!surface12)
        goto failed;

    palette12 = (SDL12_Palette *) SDL20_malloc(sizeof (SDL12_Palette));
    if (!palette12)
        goto failed;

    format12 = (SDL12_PixelFormat *) SDL20_malloc(sizeof (SDL12_PixelFormat));
    if (!format12)
        goto failed;

    SDL20_zerop(palette12);
    palette12->ncolors = surface20->format->palette->ncolors;
    palette12->colors = surface20->format->palette->colors;

    SDL20_zerop(format12);
    format12->palette = palette12;
    format12->BitsPerPixel = surface20->format->BitsPerPixel;
    format12->BytesPerPixel = surface20->format->BytesPerPixel;
    format12->Rloss = surface20->format->Rloss;
    format12->Gloss = surface20->format->Gloss;
    format12->Bloss = surface20->format->Bloss;
    format12->Aloss = surface20->format->Aloss;
    format12->Rshift = surface20->format->Rshift;
    format12->Gshift = surface20->format->Gshift;
    format12->Bshift = surface20->format->Bshift;
    format12->Ashift = surface20->format->Ashift;
    format12->Rmask = surface20->format->Rmask;
    format12->Gmask = surface20->format->Gmask;
    format12->Bmask = surface20->format->Bmask;
    format12->Amask = surface20->format->Amask;
    FIXME("format12->colorkey");
    FIXME("format12->alpha");

    SDL20_zerop(surface12);
    flags = surface20->flags;
    #define MAPSURFACEFLAGS(fl) { if (surface20->flags & SDL_##fl) { surface12->flags |= SDL12_##fl; flags &= ~SDL_##fl; } }
    MAPSURFACEFLAGS(PREALLOC);
    MAPSURFACEFLAGS(RLEACCEL);
    /*MAPSURFACEFLAGS(DONTFREE);*/
    #undef MAPSURFACEFLAGS
    SDL_assert(flags == 0);  /* non-zero if there's a flag we didn't map. */

    surface12->format = format12;
    surface12->w = surface20->w;
    surface12->h = surface20->h;
    surface12->pitch = (Uint16) surface20->pitch;  FIXME("make sure this fits in a Uint16");
    surface12->pixels = surface20->pixels;
    surface12->offset = 0;
    surface12->surface20 = surface20;
    SDL20_memcpy(&surface12->clip_rect, &surface20->clip_rect, sizeof (SDL_Rect));
    surface12->refcount = surface20->refcount;

    return surface12;

failed:
    SDL20_free(surface12);
    SDL20_free(palette12);
    SDL20_free(format12);
    return NULL;
}

DECLSPEC SDL12_Surface * SDLCALL
SDL_CreateRGBSurface(Uint32 sdl12flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    SDL_Surface *surface20 = SDL20_CreateRGBSurface(0, width, height, depth, Rmask, Gmask, Bmask, Amask);
    SDL12_Surface *surface12 = Surface20to12(surface20);
    if (!surface12) {
        SDL20_FreeSurface(surface20);
        return NULL;
    }

    SDL_assert(surface12->flags == 0);  // shouldn't have prealloc, rleaccel, or dontfree.
    return surface12;
}

DECLSPEC SDL12_Surface * SDLCALL
SDL_CreateRGBSurfaceFrom(void *pixels, int width, int height, int depth, int pitch, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    SDL_Surface *surface20 = SDL20_CreateRGBSurfaceFrom(pixels, width, height, depth, pitch, Rmask, Gmask, Bmask, Amask);
    SDL12_Surface *surface12 = Surface20to12(surface20);
    if (!surface12) {
        SDL20_FreeSurface(surface20);
        return NULL;
    }

    SDL_assert(surface12->flags == SDL12_PREALLOC);  // should _only_ have prealloc.
    return surface12;
}

DECLSPEC void SDLCALL
SDL_FreeSurface(SDL12_Surface *surface12)
{
    if (surface12) {
        SDL20_FreeSurface(surface12->surface20);
        if (surface12->format) {
            SDL20_free(surface12->format->palette);
            SDL20_free(surface12->format);
        }
        SDL20_free(surface12);
    }
}

DECLSPEC void SDLCALL
SDL_GetClipRect(SDL12_Surface *surface12, SDL_Rect *rect)
{
    if (surface12 && rect)
	    SDL_memcpy(rect, &surface12->clip_rect, sizeof (SDL_Rect));
}

DECLSPEC SDL_bool SDLCALL
SDL_SetClipRect(SDL12_Surface *surface12, const SDL_Rect *rect)
{
    SDL_bool retval = SDL_FALSE;
    if (surface12)
    {
        retval = SDL20_SetClipRect(surface12->surface20, rect);
        SDL20_GetClipRect(surface12->surface20, &surface12->clip_rect);
    }
    return retval;
}

DECLSPEC int SDLCALL
SDL_FillRect(SDL12_Surface *dst, SDL_Rect *dstrect, Uint32 color)
{
    const SDL_Rect orig_dstrect = *dstrect;
    const int retval = SDL20_FillRect(dst->surface20, &orig_dstrect, color);
    if (retval != -1)
    {
        if (dstrect)  /* 1.2 stores the clip intersection in dstrect */
            SDL20_IntersectRect(&orig_dstrect, &dst->clip_rect, dstrect);
    }
    return retval;
}


static SDL_PixelFormat *
PixelFormat12to20(SDL_PixelFormat *format20, SDL_Palette *palette20, const SDL12_PixelFormat *format12)
{
    palette20->ncolors = format12->palette->ncolors;
    palette20->colors = format12->palette->colors;
    palette20->version = 1;
    palette20->refcount = 1;
    format20->format = SDL20_MasksToPixelFormatEnum(format12->BitsPerPixel, format12->Rmask, format12->Gmask, format12->Bmask, format12->Amask);
    format20->palette = palette20;
    format20->BitsPerPixel = format12->BitsPerPixel;
    format20->BytesPerPixel = format12->BytesPerPixel;
    format20->Rmask = format12->Rmask;
    format20->Gmask = format12->Gmask;
    format20->Bmask = format12->Bmask;
    format20->Amask = format12->Amask;
    format20->Rloss = format12->Rloss;
    format20->Gloss = format12->Gloss;
    format20->Bloss = format12->Bloss;
    format20->Aloss = format12->Aloss;
    format20->Rshift = format12->Rshift;
    format20->Gshift = format12->Gshift;
    format20->Bshift = format12->Bshift;
    format20->Ashift = format12->Ashift;
    format20->refcount = 1;
    format20->next = NULL;
    return format20;
}

DECLSPEC Uint32 SDLCALL
SDL_MapRGB(const SDL12_PixelFormat *format12, Uint8 r, Uint8 g, Uint8 b)
{
    /* This is probably way slower than apps expect. */
    SDL_PixelFormat format20;
    SDL_Palette palette20;
    return SDL20_MapRGB(PixelFormat12to20(&format20, &palette20, format12), r, g, b);
}

DECLSPEC Uint32 SDLCALL
SDL_MapRGBA(const SDL12_PixelFormat *format12, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    /* This is probably way slower than apps expect. */
    SDL_PixelFormat format20;
    SDL_Palette palette20;
    return SDL20_MapRGBA(PixelFormat12to20(&format20, &palette20, format12), r, g, b, a);
}

DECLSPEC void SDLCALL
SDL_GetRGB(Uint32 pixel, const SDL12_PixelFormat *format12, Uint8 *r, Uint8 *g, Uint8 *b)
{
    /* This is probably way slower than apps expect. */
    SDL_PixelFormat format20;
    SDL_Palette palette20;
    return SDL20_GetRGB(pixel, PixelFormat12to20(&format20, &palette20, format12), r, g, b);
}

DECLSPEC void SDLCALL
SDL_GetRGBA(Uint32 pixel, const SDL12_PixelFormat *format12, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a)
{
    /* This is probably way slower than apps expect. */
    SDL_PixelFormat format20;
    SDL_Palette palette20;
    return SDL20_GetRGBA(pixel, PixelFormat12to20(&format20, &palette20, format12), r, g, b, a);
}

DECLSPEC const SDL12_VideoInfo * SDLCALL
SDL_GetVideoInfo(void)
{
    SDL_DisplayMode mode;

    FIXME("calculate this in Init12Video(), then this just does: return VideoInfo.vfmt ? &VideoInfo : NULL;");

    if (!VideoInfo12.vfmt && SDL20_GetDesktopDisplayMode(VideoDisplayIndex, &mode) == 0) {
        VideoInfo12.vfmt = SDL20_AllocFormat(mode.format);
        VideoInfo12.current_w = mode.w;
        VideoInfo12.current_h = mode.h;
        FIXME("vidinfo details commented out");
        //VideoInfo12.wm_available = 1;
        //VideoInfo12.video_mem = 1024 * 256;
    }
    return &VideoInfo12;
}

DECLSPEC int SDLCALL
SDL_VideoModeOK(int width, int height, int bpp, Uint32 sdl12flags)
{
    int i, nummodes, actual_bpp = 0;

    if (!SDL20_WasInit(SDL_INIT_VIDEO)) {
        return 0;
    }

    if (!(sdl12flags & SDL12_FULLSCREEN)) {
        SDL_DisplayMode mode;
        SDL20_GetDesktopDisplayMode(VideoDisplayIndex, &mode);
        return SDL_BITSPERPIXEL(mode.format);
    }

    nummodes = SDL20_GetNumDisplayModes(VideoDisplayIndex);
    for (i = 0; i < nummodes; ++i) {
        SDL_DisplayMode mode;
        SDL20_GetDisplayMode(VideoDisplayIndex, i, &mode);
        if (!mode.w || !mode.h || (width == mode.w && height == mode.h)) {
            if (!mode.format) {
                return bpp;
            }
            if (SDL_BITSPERPIXEL(mode.format) >= (Uint32) bpp) {
                actual_bpp = SDL_BITSPERPIXEL(mode.format);
            }
        }
    }
    return actual_bpp;
}

DECLSPEC SDL_Rect ** SDLCALL
SDL_ListModes(const SDL12_PixelFormat *format12, Uint32 flags)
{
    Uint32 fmt;
    int i;

    if (!SDL20_WasInit(SDL_INIT_VIDEO)) {
        return NULL;
    }

    if ((!format12) && (!VideoInfo12.vfmt)) {
        SDL20_SetError("No pixel format specified");
        return NULL;
    }

    if (!(flags & SDL12_FULLSCREEN)) {
        return (SDL_Rect **) (-1);  /* any resolution is fine. */
    }

    if (format12) {
        fmt = SDL20_MasksToPixelFormatEnum(format12->BitsPerPixel, format12->Rmask, format12->Gmask, format12->Bmask, format12->Amask);
    } else {
        fmt = VideoInfo12.vfmt->format;
    }

    for (i = 0; i < VideoModesCount; i++) {
        VideoModeList *modes = &VideoModes[i];
        if (modes->format == fmt) {
            return modes->modes;
        }
    }

    SDL20_SetError("No modes support requested pixel format");
    return NULL;
}

DECLSPEC void SDLCALL
SDL_FreeCursor(SDL12_Cursor *cursor12)
{
    if (cursor12) {
        if (cursor12->wm_cursor)
            SDL20_FreeCursor(cursor12->wm_cursor);
        SDL20_free(cursor12->data);
        SDL20_free(cursor12->mask);
        SDL20_free(cursor12);
    }
}

DECLSPEC SDL12_Cursor * SDLCALL
SDL_CreateCursor(Uint8 *data, Uint8 *mask, int w, int h, int hot_x, int hot_y)
{
    const size_t datasize = h * (w / 8);
    SDL_Cursor *cursor20 = NULL;
    SDL12_Cursor *retval = NULL;

    retval = (SDL12_Cursor *) SDL20_malloc(sizeof (SDL12_Cursor));
    if (!retval)
        goto outofmem;

    SDL20_zerop(retval);

    retval->data = (Uint8 *) SDL20_malloc(datasize);
    if (!retval->data)
        goto outofmem;

    retval->mask = (Uint8 *) SDL20_malloc(datasize);
    if (!retval->mask)
        goto outofmem;

    cursor20 = SDL20_CreateCursor(data, mask, w, h, hot_x, hot_y);
    if (!cursor20)
        goto failed;

    retval->area.w = w;
    retval->area.h = h;
    retval->hot_x = hot_x;
    retval->hot_y = hot_y;
    retval->wm_cursor = cursor20;
    /* we always leave retval->save as null pointers. */

    SDL20_memcpy(retval->data, data, datasize);
    SDL20_memcpy(retval->mask, mask, datasize);

    return retval;

outofmem:
    SDL20_OutOfMemory();

failed:
    SDL_FreeCursor(retval);
    return NULL;
}

DECLSPEC void SDLCALL
SDL_SetCursor(SDL12_Cursor *cursor)
{
    CurrentCursor12 = cursor;
    SDL20_SetCursor(cursor ? cursor->wm_cursor : NULL);
}

DECLSPEC SDL12_Cursor * SDLCALL
SDL_GetCursor(void)
{
    return CurrentCursor12;
}

static void
GetEnvironmentWindowPosition(int *x, int *y)
{
    int display = VideoDisplayIndex;
    const char *window = SDL20_getenv("SDL_VIDEO_WINDOW_POS");
    const char *center = SDL20_getenv("SDL_VIDEO_CENTERED");
    if (window) {
        if (SDL20_strcmp(window, "center") == 0) {
            center = window;
        } else if (SDL20_sscanf(window, "%d,%d", x, y) == 2) {
            return;
        }
    }

    if (center) {
        *x = SDL_WINDOWPOS_CENTERED_DISPLAY(display);
        *y = SDL_WINDOWPOS_CENTERED_DISPLAY(display);
    }
}

static void
SetupScreenSaver(const int flags12)
{
    const char *env;
    SDL_bool allow_screensaver;

    /* Allow environment override of screensaver disable */
    env = SDL20_getenv("SDL_VIDEO_ALLOW_SCREENSAVER");
    if (env) {
        allow_screensaver = SDL20_atoi(env) ? SDL_TRUE : SDL_FALSE;
    } else if (flags12 & SDL12_FULLSCREEN) {
        allow_screensaver = SDL_FALSE;
    } else {
        allow_screensaver = SDL_TRUE;
    }
    if (allow_screensaver) {
        SDL20_EnableScreenSaver();
    } else {
        SDL20_DisableScreenSaver();
    }
}


static SDL12_Surface *
EndVidModeCreate(void)
{
    if (VideoTexture20) {
        SDL20_DestroyTexture(VideoTexture20);
        VideoTexture20 = NULL;
    }
    if (VideoRenderer20) {
        SDL20_DestroyRenderer(VideoRenderer20);
        VideoRenderer20 = NULL;
    }
    if (VideoGLContext20) {
        SDL20_GL_MakeCurrent(NULL, NULL);
        SDL20_GL_DeleteContext(VideoGLContext20);
        VideoGLContext20 = NULL;
    }
    if (VideoWindow20) {
        SDL20_DestroyWindow(VideoWindow20);
        VideoWindow20 = NULL;
    }
    if (VideoSurface12) {
        SDL20_free(VideoSurface12->pixels);
        VideoSurface12->pixels = NULL;
        SDL_FreeSurface(VideoSurface12);
        VideoSurface12 = NULL;
    }
    if (VideoConvertSurface20) {
        SDL20_FreeSurface(VideoConvertSurface20);
        VideoConvertSurface20 = NULL;
    }
    return NULL;
}


static SDL12_Surface *
CreateSurface12WithFormat(const int w, const int h, const Uint32 fmt)
{
    Uint32 rmask, gmask, bmask, amask;
    int bpp;
    if (!SDL20_PixelFormatEnumToMasks(fmt, &bpp, &rmask, &gmask, &bmask, &amask)) {
        return NULL;
    }
    return SDL_CreateRGBSurface(0, w, h, bpp, rmask, gmask, bmask, amask);
}

static SDL_Surface *
CreateNullPixelSurface20(const int width, const int height, const Uint32 fmt)
{
    SDL_Surface *surface20 = SDL20_CreateRGBSurfaceWithFormat(0, 0, 0, SDL_BITSPERPIXEL(fmt), fmt);
    if (surface20) {
        surface20->flags |= SDL_PREALLOC;
        surface20->pixels = NULL;
        surface20->w = width;
        surface20->h = height;
        surface20->pitch = 0;
        SDL20_SetClipRect(surface20, NULL);
    }
    return surface20;
}


DECLSPEC SDL12_Surface * SDLCALL
SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags12)
{
    FIXME("currently ignores SDL_WINDOWID, which we could use with SDL_CreateWindowFrom ...?");
    SDL_DisplayMode dmode;
    Uint32 fullscreen_flags20 = 0;
    Uint32 appfmt;

    /* SDL_SetVideoMode() implicitly inits if necessary. */
    if (SDL20_WasInit(SDL_INIT_VIDEO) == 0) {
        if (SDL20_Init(SDL_INIT_VIDEO) < 0) {
            return NULL;
        }
    }

    if ((flags12 & SDL12_OPENGLBLIT) == SDL12_OPENGLBLIT) {
        FIXME("No OPENGLBLIT support at the moment");
        SDL20_SetError("SDL_OPENGLBLIT is (currently) unsupported");
        return NULL;
    }

    FIXME("handle SDL_ANYFORMAT");

    if ((width < 0) || (height < 0)) {
        SDL20_SetError("Invalid width or height");
        return NULL;
    }

    FIXME("There's an environment variable to choose a display");
    if (SDL20_GetCurrentDisplayMode(0, &dmode) < 0) {
        return NULL;
    }

    if (width == 0) {
        width = dmode.w;
    }

    if (height == 0) {
        height = dmode.h;
    }

    if (bpp == 0) {
        bpp = SDL_BITSPERPIXEL(dmode.format);
    }

    switch (bpp) {
        case  8: appfmt = SDL_PIXELFORMAT_INDEX8; break;
        case 16: appfmt = SDL_PIXELFORMAT_RGB565; FIXME("bgr instead of rgb?"); break;
        case 24: appfmt = SDL_PIXELFORMAT_RGB24; FIXME("bgr instead of rgb?"); break;
        case 32: appfmt = SDL_PIXELFORMAT_ARGB8888; FIXME("bgr instead of rgb?"); break;
        default: SDL20_SetError("Unsupported bits-per-pixel"); return NULL;
    }

    SDL_assert((VideoSurface12 != NULL) == (VideoWindow20 != NULL));

    FIXME("don't do anything if the window's dimensions, etc haven't changed.");
    FIXME("we need to preserve VideoSurface12 (but not its pixels), I think...");

    if ( VideoSurface12 && ((VideoSurface12->flags & SDL12_OPENGL) != (flags12 & SDL12_OPENGL)) ) {
        EndVidModeCreate();  /* rebuild the window if moving to/from a GL context */
    } else if ( VideoSurface12 && (VideoSurface12->surface20->format->format != appfmt)) {
        EndVidModeCreate();  /* rebuild the window if changing pixel format */
    } else if (VideoGLContext20) {
        /* SDL 1.2 (infuriatingly!) destroys the GL context on each resize, so we will too */
        SDL20_GL_MakeCurrent(NULL, NULL);
        SDL20_GL_DeleteContext(VideoGLContext20);
        VideoGLContext20 = NULL;
    }

    if (flags12 & SDL12_FULLSCREEN) {
        // OpenGL tries to force the real resolution requested, but for
        //  software rendering, we're just going to push it off onto the
        //  GPU, so use FULLSCREEN_DESKTOP and logical scaling there.
        FIXME("OpenGL will still expect letterboxing and centering if it didn't get an exact resolution match.");
        if (flags12 & SDL12_OPENGL) {
            fullscreen_flags20 |= SDL_WINDOW_FULLSCREEN;
        } else {
            fullscreen_flags20 |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        }
    }

    if (!VideoWindow20) {  /* create it */
        int x = SDL_WINDOWPOS_UNDEFINED, y = SDL_WINDOWPOS_UNDEFINED;
        Uint32 flags20 = fullscreen_flags20;
        if (flags12 & SDL12_OPENGL) { flags20 |= SDL_WINDOW_OPENGL; }
        if (flags12 & SDL12_RESIZABLE) { flags20 |= SDL_WINDOW_RESIZABLE; }
        if (flags12 & SDL12_NOFRAME) { flags20 |= SDL_WINDOW_BORDERLESS; }

        /* most platforms didn't check these environment variables, but the major
           ones did (x11, windib, quartz), so we'll just offer it everywhere. */
        GetEnvironmentWindowPosition(&x, &y);

        VideoWindow20 = SDL20_CreateWindow(WindowTitle, x, y, width, height, flags20);
        if (!VideoWindow20) {
            return EndVidModeCreate();
        }
    } else {  /* resize it */
        SDL20_SetWindowSize(VideoWindow20, width, height);
        SDL20_SetWindowFullscreen(VideoWindow20, fullscreen_flags20);
        SDL20_SetWindowBordered(VideoWindow20, (flags12 & SDL12_NOFRAME) ? SDL_FALSE : SDL_TRUE);
        SDL20_SetWindowResizable(VideoWindow20, (flags12 & SDL12_RESIZABLE) ? SDL_TRUE : SDL_FALSE);
    }

    if (VideoSurface12) {
        SDL20_free(VideoSurface12->pixels);
    } else {
        VideoSurface12 = CreateSurface12WithFormat(0, 0, appfmt);
        if (!VideoSurface12) {
            return EndVidModeCreate();
        }
    }

    VideoSurface12->surface20->flags |= SDL_PREALLOC;
    VideoSurface12->flags |= SDL12_PREALLOC;
    VideoSurface12->pixels = VideoSurface12->surface20->pixels = NULL;
    VideoSurface12->w = VideoSurface12->surface20->w = width;
    VideoSurface12->h = VideoSurface12->surface20->h = height;
    VideoSurface12->pitch = VideoSurface12->surface20->pitch = width * SDL_BYTESPERPIXEL(appfmt);
    SDL_SetClipRect(VideoSurface12, NULL);

    if (flags12 & SDL12_OPENGL) {
        SDL_assert(!VideoTexture20);  /* either a new window or we destroyed all this */
        SDL_assert(!VideoRenderer20);
        VideoGLContext20 = SDL20_GL_CreateContext(VideoWindow20);
        if (!VideoGLContext20) {
            return EndVidModeCreate();
        }

        VideoSurface12->flags |= SDL12_OPENGL;
    } else {
        /* always use a renderer for non-OpenGL windows. */
        SDL_RendererInfo rinfo;
        SDL_assert(!VideoGLContext20);  /* either a new window or we destroyed all this */
        if (!VideoRenderer20) {
            VideoRenderer20 = SDL20_CreateRenderer(VideoWindow20, -1, 0);
            if (!VideoRenderer20) {
                return EndVidModeCreate();
            }
        }

        SDL20_RenderSetLogicalSize(VideoRenderer20, width, height);
        SDL20_SetRenderDrawColor(VideoRenderer20, 0, 0, 0, 255);
        SDL20_RenderClear(VideoRenderer20);
        SDL20_RenderPresent(VideoRenderer20);
        SDL20_SetRenderDrawColor(VideoRenderer20, 255, 255, 255, 255);

        if (SDL20_GetRendererInfo(VideoRenderer20, &rinfo) < 0) {
            return EndVidModeCreate();
        }

        if (VideoTexture20) {
            SDL20_DestroyTexture(VideoTexture20);
        }

        if (VideoConvertSurface20) {
            SDL20_FreeSurface(VideoConvertSurface20);
            VideoConvertSurface20 = NULL;
        }

        VideoTexture20 = SDL20_CreateTexture(VideoRenderer20, rinfo.texture_formats[0], SDL_TEXTUREACCESS_STREAMING, width, height);
        if (!VideoTexture20) {
            return EndVidModeCreate();
        }

        if (rinfo.texture_formats[0] != appfmt) {
            /* need to convert between app's format and texture format */
            VideoConvertSurface20 = CreateNullPixelSurface20(width, height, rinfo.texture_formats[0]);
            if (!VideoConvertSurface20) {
                return EndVidModeCreate();
            }
        }

        VideoSurface12->flags &= ~SDL12_OPENGL;
        VideoSurface12->surface20->pixels = SDL20_malloc(height * VideoSurface12->pitch);
        VideoSurface12->pixels = VideoSurface12->surface20->pixels;
        if (!VideoSurface12->pixels) {
            SDL20_OutOfMemory();
            return EndVidModeCreate();
        }
    }

    FIXME("setup screen saver");

    return VideoSurface12;
}

DECLSPEC SDL12_Surface * SDLCALL
SDL_GetVideoSurface(void)
{
    return VideoSurface12;
}

DECLSPEC int SDLCALL
SDL_UpperBlit(SDL12_Surface *src, SDL_Rect *srcrect, SDL12_Surface *dst, SDL_Rect *dstrect)
{
    return SDL20_UpperBlit(src->surface20, srcrect, dst->surface20, dstrect);
}

DECLSPEC int SDLCALL
SDL_LowerBlit(SDL12_Surface *src, SDL_Rect *srcrect, SDL12_Surface *dst, SDL_Rect *dstrect)
{
    return SDL20_LowerBlit(src->surface20, srcrect, dst->surface20, dstrect);
}

DECLSPEC int SDLCALL
SDL_SetAlpha(SDL12_Surface * surface, Uint32 flag, Uint8 value)
{
    FIXME("write me");
    return SDL20_Unsupported();
}

DECLSPEC SDL12_Surface * SDLCALL
SDL_ConvertSurface(SDL12_Surface *src12, const SDL12_PixelFormat *format12, Uint32 flags12)
{
    Uint32 flags20 = 0;
    SDL_PixelFormat format20;
    SDL_Palette palette20;
    SDL_Surface *surface20;
    SDL12_Surface *retval = NULL;

    if (flags12 & SDL12_PREALLOC) flags20 |= SDL_PREALLOC;
    if (flags12 & SDL12_RLEACCEL) flags20 |= SDL_RLEACCEL;

    surface20 = SDL20_ConvertSurface(src12->surface20, PixelFormat12to20(&format20, &palette20, format12), flags20);
    if (surface20) {
        retval = Surface20to12(surface20);
        if (!retval) {
            SDL20_FreeSurface(surface20);
        }
    }
    return retval;
}

DECLSPEC SDL12_Surface * SDLCALL
SDL_DisplayFormat(SDL12_Surface *surface12)
{
    const Uint32 flags = surface12->flags & (SDL12_SRCCOLORKEY|SDL12_SRCALPHA|SDL12_RLEACCELOK);
    return SDL_ConvertSurface(surface12, VideoSurface12->format, flags);
}

DECLSPEC SDL12_Surface * SDLCALL
SDL_DisplayFormatAlpha(SDL12_Surface *surface)
{
    FIXME("write me");
    SDL20_Unsupported();
    return NULL;
}

DECLSPEC void SDLCALL
SDL_UpdateRects(SDL12_Surface * screen12, int numrects, SDL_Rect * rects)
{
    FIXME("write me");
    SDL20_Unsupported();
}

DECLSPEC void SDLCALL
SDL_UpdateRect(SDL12_Surface *screen12, Sint32 x, Sint32 y, Uint32 w, Uint32 h)
{
    if (screen12) {
        SDL_Rect rect;
        rect.x = (int) x;
        rect.y = (int) y;
        rect.w = (int) (w ? w : screen12->w);
        rect.h = (int) (h ? h : screen12->h);
        SDL_UpdateRects(screen12, 1, &rect);
    }
}

DECLSPEC int SDLCALL
SDL_Flip(SDL12_Surface *screen12)
{
    SDL_UpdateRect(screen12, 0, 0, 0, 0);
    return 0;
}

DECLSPEC void SDLCALL
SDL_WM_SetCaption(const char *title, const char *icon)
{
    if (WindowTitle) {
        SDL20_free(WindowTitle);
    }
    if (WindowIconTitle) {
        SDL20_free(WindowIconTitle);
    }
    WindowTitle = title ? SDL_strdup(title) : NULL;
    WindowIconTitle = icon ? SDL_strdup(icon) : NULL;
    if (VideoWindow20) {
        SDL20_SetWindowTitle(VideoWindow20, WindowTitle);
    }
}

DECLSPEC void SDLCALL
SDL_WM_GetCaption(const char **title, const char **icon)
{
    if (title) {
        *title = WindowTitle;
    }
    if (icon) {
        *icon = WindowIconTitle;
    }
}

DECLSPEC void SDLCALL
SDL_WM_SetIcon(SDL_Surface *icon, Uint8 *mask)
{
    FIXME("write me");
    SDL20_Unsupported();
}

DECLSPEC int SDLCALL
SDL_WM_IconifyWindow(void)
{
    SDL20_MinimizeWindow(VideoWindow20);
    return 0;
}

DECLSPEC int SDLCALL
SDL_WM_ToggleFullScreen(SDL12_Surface *surface)
{
    FIXME("write me");
    return SDL20_Unsupported();
}

typedef enum
{
    SDL12_GRAB_QUERY = -1,
    SDL12_GRAB_OFF = 0,
    SDL12_GRAB_ON = 1
} SDL12_GrabMode;

DECLSPEC SDL12_GrabMode SDLCALL
SDL_WM_GrabInput(SDL12_GrabMode mode)
{
    if (mode != SDL12_GRAB_QUERY) {
        SDL20_SetWindowGrab(VideoWindow20, (mode == SDL12_GRAB_ON));
    }
    return SDL20_GetWindowGrab(VideoWindow20) ? SDL12_GRAB_ON : SDL12_GRAB_OFF;
}

DECLSPEC void SDLCALL
SDL_WarpMouse(Uint16 x, Uint16 y)
{
    SDL20_WarpMouseInWindow(VideoWindow20, x, y);
}

DECLSPEC Uint8 SDLCALL
SDL_GetAppState(void)
{
    Uint8 state12 = 0;
    Uint32 flags20 = 0;

    flags20 = SDL20_GetWindowFlags(VideoWindow20);
    if ((flags20 & SDL_WINDOW_SHOWN) && !(flags20 & SDL_WINDOW_MINIMIZED)) {
        state12 |= SDL12_APPACTIVE;
    }
    if (flags20 & SDL_WINDOW_INPUT_FOCUS) {
        state12 |= SDL12_APPINPUTFOCUS;
    }
    if (flags20 & SDL_WINDOW_MOUSE_FOCUS) {
        state12 |= SDL12_APPMOUSEFOCUS;
    }
    return state12;
}

DECLSPEC int SDLCALL
SDL_SetPalette(SDL12_Surface *surface12, int flags, const SDL_Color *colors,
               int firstcolor, int ncolors)
{
    FIXME("write me");
    return SDL20_Unsupported();
}

DECLSPEC int SDLCALL
SDL_SetColors(SDL12_Surface *surface12, const SDL_Color * colors, int firstcolor,
              int ncolors)
{
    FIXME("write me");
    return SDL20_Unsupported();
}

DECLSPEC int SDLCALL
SDL_GetWMInfo(SDL_SysWMinfo * info)
{
    FIXME("write me");
    //return SDL20_GetWindowWMInfo(VideoWindow20, info);
    return SDL20_Unsupported();
}

DECLSPEC SDL12_Overlay * SDLCALL
SDL_CreateYUVOverlay(int w, int h, Uint32 format, SDL12_Surface *display)
{
    FIXME("write me");
    SDL20_Unsupported();
    return NULL;
}

DECLSPEC int SDLCALL
SDL_LockYUVOverlay(SDL12_Overlay * overlay)
{
    FIXME("write me");
    return SDL20_Unsupported();
}

DECLSPEC void SDLCALL
SDL_UnlockYUVOverlay(SDL12_Overlay * overlay)
{
    FIXME("write me");
}

DECLSPEC int SDLCALL
SDL_DisplayYUVOverlay(SDL12_Overlay * overlay, SDL_Rect * dstrect)
{
    FIXME("write me");
    return SDL20_Unsupported();
}

DECLSPEC void SDLCALL
SDL_FreeYUVOverlay(SDL12_Overlay * overlay)
{
    FIXME("write me");
}

DECLSPEC int SDLCALL
SDL_GL_SetAttribute(SDL12_GLattr attr, int value)
{
    if (attr >= SDL12_GL_MAX_ATTRIBUTE)
        return SDL20_SetError("Unknown GL attribute");

    /* swap control was moved out of this API, everything else lines up. */
    if (attr == SDL12_GL_SWAP_CONTROL)
    {
        SwapInterval = value;
        FIXME("Actually set swap interval somewhere");
        return 0;
    }

    return SDL20_GL_SetAttribute((SDL_GLattr) attr, value);
}

DECLSPEC int SDLCALL
SDL_GL_GetAttribute(SDL12_GLattr attr, int* value)
{
    if (attr >= SDL12_GL_MAX_ATTRIBUTE)
        return SDL20_SetError("Unknown GL attribute");

    /* swap control was moved out of this API, everything else lines up. */
    if (attr == SDL12_GL_SWAP_CONTROL)
    {
        *value = SDL20_GL_GetSwapInterval();
        return 0;
    }

    return SDL20_GL_GetAttribute((SDL_GLattr) attr, value);
}


DECLSPEC void SDLCALL
SDL_GL_SwapBuffers(void)
{
    if (VideoWindow20)
        SDL20_GL_SwapWindow(VideoWindow20);
}

DECLSPEC int SDLCALL
SDL_SetGamma(float red, float green, float blue)
{
    Uint16 red_ramp[256];
    Uint16 green_ramp[256];
    Uint16 blue_ramp[256];

    SDL20_CalculateGammaRamp(red, red_ramp);
    if (green == red) {
        SDL20_memcpy(green_ramp, red_ramp, sizeof(red_ramp));
    } else {
        SDL20_CalculateGammaRamp(green, green_ramp);
    }
    if (blue == red) {
        SDL20_memcpy(blue_ramp, red_ramp, sizeof(red_ramp));
    } else if (blue == green) {
        SDL20_memcpy(blue_ramp, green_ramp, sizeof(green_ramp));
    } else {
        SDL20_CalculateGammaRamp(blue, blue_ramp);
    }
    return SDL20_SetWindowGammaRamp(VideoWindow20, red_ramp, green_ramp, blue_ramp);
}

DECLSPEC int SDLCALL
SDL_SetGammaRamp(const Uint16 *red, const Uint16 *green, const Uint16 *blue)
{
    return SDL20_SetWindowGammaRamp(VideoWindow20, red, green, blue);
}

DECLSPEC int SDLCALL
SDL_GetGammaRamp(Uint16 *red, Uint16 *green, Uint16 *blue)
{
    return SDL20_GetWindowGammaRamp(VideoWindow20, red, green, blue);
}

DECLSPEC int SDLCALL
SDL_EnableKeyRepeat(int delay, int interval)
{
    return 0;
}

DECLSPEC void SDLCALL
SDL_GetKeyRepeat(int *delay, int *interval)
{
    if (delay) {
        *delay = SDL12_DEFAULT_REPEAT_DELAY;
    }
    if (interval) {
        *interval = SDL12_DEFAULT_REPEAT_INTERVAL;
    }
}

DECLSPEC int SDLCALL
SDL_EnableUNICODE(int enable)
{
    FIXME("write me");
    return SDL20_Unsupported();
}

static Uint32
SetTimerOld_Callback(Uint32 interval, void* param)
{
    return ((SDL12_TimerCallback)param)(interval);
}

DECLSPEC int SDLCALL
SDL_SetTimer(Uint32 interval, SDL12_TimerCallback callback)
{
    static SDL_TimerID compat_timer;

    if (compat_timer) {
        SDL20_RemoveTimer(compat_timer);
        compat_timer = 0;
    }

    if (interval && callback) {
        compat_timer = SDL20_AddTimer(interval, SetTimerOld_Callback, callback);
        if (!compat_timer) {
            return -1;
        }
    }
    return 0;
}

DECLSPEC int SDLCALL
SDL_putenv(const char *_var)
{
    char *ptr = NULL;
    char *var = SDL20_strdup(_var);
    if (var == NULL) {
        return -1;  /* we don't set errno. */
    }

    ptr = SDL20_strchr(var, '=');
    if (ptr == NULL) {
        SDL20_free(var);
        return -1;
    }

    *ptr = '\0';  /* split the string into name and value. */
    SDL20_setenv(var, ptr + 1, 1);
    SDL20_free(var);
    return 0;
}



/* CD-ROM support is gone from SDL 2.0, so just have stubs that fail. */

typedef void *SDL12_CD;  /* close enough.  :) */
typedef int SDL12_CDstatus;  /* close enough.  :) */

DECLSPEC int SDLCALL
SDL_CDNumDrives(void)
{
    FIXME("should return -1 without SDL_INIT_CDROM");
    return 0;
}

DECLSPEC const char *SDLCALL SDL_CDName(int drive) { SDL20_Unsupported(); return NULL; }
DECLSPEC SDL12_CD *SDLCALL SDL_CDOpen(int drive) { SDL20_Unsupported(); return NULL; }
DECLSPEC SDL12_CDstatus SDLCALL SDL_CDStatus(SDL12_CD *cdrom) { return SDL20_Unsupported(); }
DECLSPEC int SDLCALL SDL_CDPlayTracks(SDL12_CD *cdrom, int start_track, int start_frame, int ntracks, int nframes) { return SDL20_Unsupported(); }
DECLSPEC int SDLCALL SDL_CDPlay(SDL12_CD *cdrom, int start, int length) { return SDL20_Unsupported(); }
DECLSPEC int SDLCALL SDL_CDPause(SDL12_CD *cdrom) { return SDL20_Unsupported(); }
DECLSPEC int SDLCALL SDL_CDResume(SDL12_CD *cdrom) { return SDL20_Unsupported(); }
DECLSPEC int SDLCALL SDL_CDStop(SDL12_CD *cdrom) { return SDL20_Unsupported(); }
DECLSPEC int SDLCALL SDL_CDEject(SDL12_CD *cdrom) { return SDL20_Unsupported(); }
DECLSPEC void SDLCALL SDL_CDClose(SDL12_CD *cdrom) {}


#ifdef SDL_PASSED_BEGINTHREAD_ENDTHREAD
DECLSPEC SDL_Thread * SDLCALL
SDL_CreateThread(int (SDLCALL *fn)(void *), void *data, pfnSDL_CurrentBeginThread pfnBeginThread, pfnSDL_CurrentEndThread pfnEndThread)
{
    return SDL20_CreateThread(fn, NULL, data, pfnBeginThread, pfnEndThread);
}
#else
DECLSPEC SDL_Thread * SDLCALL
SDL_CreateThread(int (SDLCALL *fn)(void *), void *data)
{
    return SDL20_CreateThread(fn, NULL, data);
}
#endif

DECLSPEC int SDLCALL
SDL_mutexP(SDL_mutex *mutex)
{
    return SDL20_LockMutex(mutex);
}

DECLSPEC int SDLCALL
SDL_mutexV(SDL_mutex *mutex)
{
    return SDL20_UnlockMutex(mutex);
}

DECLSPEC void SDLCALL SDL_KillThread(SDL_Thread *thread)
{
    FIXME("Removed from 2.0; do nothing. We can't even report failure.");
    fprintf(stderr,
        "WARNING: this app used SDL_KillThread(), an unforgivable curse.\n"
        "This program should be fixed. No thread was actually harmed.\n");
}

/* This changed from an opaque pointer to an int in 2.0. */
typedef struct _SDL12_TimerID *SDL12_TimerID;
SDL_COMPILE_TIME_ASSERT(timer, sizeof(SDL12_TimerID) >= sizeof(SDL_TimerID));


DECLSPEC SDL12_TimerID SDLCALL
SDL_AddTimer(Uint32 interval, SDL12_NewTimerCallback callback, void *param)
{
    return (SDL12_TimerID) ((size_t) SDL20_AddTimer(interval, callback, param));
}

DECLSPEC SDL_bool SDLCALL
SDL_RemoveTimer(SDL12_TimerID id)
{
    return SDL20_RemoveTimer((SDL_TimerID) ((size_t)id));
}


typedef struct SDL12_RWops {
    int (SDLCALL *seek)(struct SDL12_RWops *context, int offset, int whence);
    int (SDLCALL *read)(struct SDL12_RWops *context, void *ptr, int size, int maxnum);
    int (SDLCALL *write)(struct SDL12_RWops *context, const void *ptr, int size, int num);
    int (SDLCALL *close)(struct SDL12_RWops *context);
    Uint32 type;
    void *padding[8];
    SDL_RWops *rwops20;
} SDL12_RWops;


DECLSPEC SDL12_RWops * SDLCALL
SDL_AllocRW(void)
{
    SDL12_RWops *rwops = (SDL12_RWops *) SDL20_malloc(sizeof (SDL12_RWops));
    if (!rwops)
        SDL20_OutOfMemory();
    return rwops;
}

DECLSPEC void SDLCALL
SDL_FreeRW(SDL12_RWops *rwops12)
{
    SDL20_free(rwops12);
}

static int SDLCALL
RWops20to12_seek(struct SDL12_RWops *rwops12, int offset, int whence)
{
    return rwops12->rwops20->seek(rwops12->rwops20, offset, whence);
}

static int SDLCALL
RWops20to12_read(struct SDL12_RWops *rwops12, void *ptr, int size, int maxnum)
{
    return rwops12->rwops20->read(rwops12->rwops20, ptr, size, maxnum);
}

static int SDLCALL
RWops20to12_write(struct SDL12_RWops *rwops12, const void *ptr, int size, int num)
{
    return rwops12->rwops20->write(rwops12->rwops20, ptr, size, num);
}

static int SDLCALL
RWops20to12_close(struct SDL12_RWops *rwops12)
{
    int rc = 0;
    if (rwops12)
    {
        rc = rwops12->rwops20->close(rwops12->rwops20);
        if (rc == 0)
            SDL_FreeRW(rwops12);
    }
    return rc;
}

static SDL12_RWops *
RWops20to12(SDL_RWops *rwops20)
{
    SDL12_RWops *rwops12;

    if (!rwops20)
        return NULL;

    rwops12 = SDL_AllocRW();
    if (!rwops12)
        return NULL;

    SDL20_zerop(rwops12);
    rwops12->type = rwops20->type;
    rwops12->rwops20 = rwops20;
    rwops12->seek = RWops20to12_seek;
    rwops12->read = RWops20to12_read;
    rwops12->write = RWops20to12_write;
    rwops12->close = RWops20to12_close;

    return rwops12;
}

DECLSPEC SDL12_RWops * SDLCALL
SDL_RWFromFile(const char *file, const char *mode)
{
    return RWops20to12(SDL20_RWFromFile(file, mode));
}

DECLSPEC SDL12_RWops * SDLCALL
SDL_RWFromFP(FILE *io, int autoclose)
{
    return RWops20to12(SDL20_RWFromFP(io, autoclose));
}

DECLSPEC SDL12_RWops * SDLCALL
SDL_RWFromMem(void *mem, int size)
{
    return RWops20to12(SDL20_RWFromMem(mem, size));
}

DECLSPEC SDL12_RWops * SDLCALL
SDL_RWFromConstMem(const void *mem, int size)
{
    return RWops20to12(SDL20_RWFromConstMem(mem, size));
}

#define READ_AND_BYTESWAP(endian, bits) \
    DECLSPEC Uint##bits SDLCALL SDL_Read##endian##bits(SDL12_RWops *rwops12) { \
        Uint##bits val; rwops12->read(rwops12, &val, sizeof (val), 1); \
        return SDL_Swap##endian##bits(val); \
    }

READ_AND_BYTESWAP(LE,16)
READ_AND_BYTESWAP(BE,16)
READ_AND_BYTESWAP(LE,32)
READ_AND_BYTESWAP(BE,32)
READ_AND_BYTESWAP(LE,64)
READ_AND_BYTESWAP(BE,64)
#undef READ_AND_BYTESWAP

#define BYTESWAP_AND_WRITE(endian, bits) \
    DECLSPEC int SDLCALL SDL_Write##endian##bits(SDL12_RWops *rwops12, Uint##bits val) { \
        val = SDL_Swap##endian##bits(val); \
        return rwops12->write(rwops12, &val, sizeof (val), 1); \
    }
BYTESWAP_AND_WRITE(LE,16)
BYTESWAP_AND_WRITE(BE,16)
BYTESWAP_AND_WRITE(LE,32)
BYTESWAP_AND_WRITE(BE,32)
BYTESWAP_AND_WRITE(LE,64)
BYTESWAP_AND_WRITE(BE,64)
#undef BYTESWAP_AND_WRITE


static Sint64 SDLCALL
RWops12to20_size(struct SDL_RWops *rwops20)
{
    SDL12_RWops *rwops12 = (SDL12_RWops *) rwops20->hidden.unknown.data1;
    int size = (int) ((size_t) rwops20->hidden.unknown.data2);
    int pos;

    if (size != -1)
        return size;

    pos = rwops12->seek(rwops12, 0, SEEK_CUR);
    if (pos == -1)
        return -1;

    size = (Sint64) rwops12->seek(rwops12, 0, SEEK_END);
    if (size == -1)
        return -1;

    rwops12->seek(rwops12, pos, SEEK_SET);  FIXME("...and if this fails?");
    rwops20->hidden.unknown.data2 = (void *) ((size_t) size);
    return size;
}

static Sint64 SDLCALL
RWops12to20_seek(struct SDL_RWops *rwops20, Sint64 offset, int whence)
{
    FIXME("fail if (offset) is too big");
    SDL12_RWops *rwops12 = (SDL12_RWops *) rwops20->hidden.unknown.data1;
    return (Sint64) rwops12->seek(rwops12, (int) offset, whence);
}

static size_t SDLCALL
RWops12to20_read(struct SDL_RWops *rwops20, void *ptr, size_t size, size_t maxnum)
{
    FIXME("fail if (size) or (maxnum) is too big");
    SDL12_RWops *rwops12 = (SDL12_RWops *) rwops20->hidden.unknown.data1;
    return (size_t) rwops12->read(rwops12, ptr, (int) size, (int) maxnum);
}

static size_t SDLCALL
RWops12to20_write(struct SDL_RWops *rwops20, const void *ptr, size_t size, size_t num)
{
    FIXME("fail if (size) or (maxnum) is too big");
    SDL12_RWops *rwops12 = (SDL12_RWops *) rwops20->hidden.unknown.data1;
    return (size_t) rwops12->write(rwops12, ptr, (int) size, (int) num);
}

static int SDLCALL
RWops12to20_close(struct SDL_RWops *rwops20)
{
    int rc = 0;
    if (rwops20)
    {
        SDL12_RWops *rwops12 = (SDL12_RWops *) rwops20->hidden.unknown.data1;
        rc = rwops12->close(rwops12);
        if (rc == 0)
            SDL20_FreeRW(rwops20);
    }
    return rc;
}

static SDL_RWops *
RWops12to20(SDL12_RWops *rwops12)
{
    SDL_RWops *rwops20;

    if (!rwops12)
        return NULL;

    rwops20 = SDL20_AllocRW();
    if (!rwops20)
        return NULL;

    SDL20_zerop(rwops20);
    rwops20->type = rwops12->type;
    rwops20->hidden.unknown.data1 = rwops12;
    rwops20->hidden.unknown.data2 = (void *) ((size_t) -1);  /* cached size of stream */
    rwops20->size = RWops12to20_size;
    rwops20->seek = RWops12to20_seek;
    rwops20->read = RWops12to20_read;
    rwops20->write = RWops12to20_write;
    rwops20->close = RWops12to20_close;
    return rwops20;
}

DECLSPEC SDL12_Surface * SDLCALL
SDL_LoadBMP_RW(SDL12_RWops *rwops12, int freerwops12)
{
    SDL_RWops *rwops20 = RWops12to20(rwops12);
    SDL_Surface *surface20 = SDL20_LoadBMP_RW(rwops20, freerwops12);
    SDL12_Surface *surface12 = Surface20to12(surface20);
    if (!freerwops12)  /* free our wrapper if SDL2 didn't close it. */
        SDL20_FreeRW(rwops20);
    if ((!surface12) && (surface20))
        SDL20_FreeSurface(surface20);
    return surface12;
}

DECLSPEC int SDLCALL
SDL_SaveBMP_RW(SDL12_Surface *surface12, SDL12_RWops *rwops12, int freerwops12)
{
    FIXME("wrap surface");
    SDL_RWops *rwops20 = RWops12to20(rwops12);
    const int retval = SDL20_SaveBMP_RW(surface12->surface20, rwops20, freerwops12);
    if (!freerwops12)  /* free our wrapper if SDL2 didn't close it. */
        SDL20_FreeRW(rwops20);
    return retval;
}

DECLSPEC SDL_AudioSpec * SDLCALL
SDL_LoadWAV_RW(SDL12_RWops *rwops12, int freerwops12,
               SDL_AudioSpec *spec, Uint8 **buf, Uint32 *len)
{
    SDL_RWops *rwops20 = RWops12to20(rwops12);
    SDL_AudioSpec *retval = SDL20_LoadWAV_RW(rwops20, freerwops12, spec, buf, len);
    FIXME("deal with non-1.2 formats, like float32");
    if (!freerwops12)  /* free our wrapper if SDL2 didn't close it. */
        SDL20_FreeRW(rwops20);
    return retval;
}

/* vi: set ts=4 sw=4 expandtab: */
