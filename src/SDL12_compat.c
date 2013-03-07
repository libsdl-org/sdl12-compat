/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2013 Sam Lantinga <slouken@libsdl.org>

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

#include "SDL.h"

#if !SDL_VERSION_ATLEAST(2,0,0)
#error You need to compile against SDL 2.0 headers.
#endif

#include "SDL_syswm.h"

//#include "video/SDL_sysvideo.h"
//#include "video/SDL_pixels_c.h"
//#include "render/SDL_yuv_sw_c.h"

// !!! IMPLEMENT_ME SDL_AudioInit
// !!! IMPLEMENT_ME SDL_AudioQuit
// !!! IMPLEMENT_ME SDL_ConvertSurface
// !!! IMPLEMENT_ME SDL_CreateCursor
// !!! IMPLEMENT_ME SDL_CreateRGBSurface
// !!! IMPLEMENT_ME SDL_CreateRGBSurfaceFrom
// !!! IMPLEMENT_ME SDL_CreateThread
// !!! IMPLEMENT_ME SDL_Error
// !!! IMPLEMENT_ME SDL_EventState
// !!! IMPLEMENT_ME SDL_FillRect
// !!! IMPLEMENT_ME SDL_FreeCursor
// !!! IMPLEMENT_ME SDL_FreeSurface
// !!! IMPLEMENT_ME SDL_GL_GetAttribute
// !!! IMPLEMENT_ME SDL_GL_GetProcAddress
// !!! IMPLEMENT_ME SDL_GL_LoadLibrary
// !!! IMPLEMENT_ME SDL_GL_Lock
// !!! IMPLEMENT_ME SDL_GL_SetAttribute
// !!! IMPLEMENT_ME SDL_GL_Unlock
// !!! IMPLEMENT_ME SDL_GL_UpdateRects
// !!! IMPLEMENT_ME SDL_GetClipRect
// !!! IMPLEMENT_ME SDL_GetCursor
// !!! IMPLEMENT_ME SDL_GetEventFilter
// !!! IMPLEMENT_ME SDL_GetKeyName
// !!! IMPLEMENT_ME SDL_GetKeyState
// !!! IMPLEMENT_ME SDL_GetModState
// !!! IMPLEMENT_ME SDL_GetMouseState
// !!! IMPLEMENT_ME SDL_GetRGB
// !!! IMPLEMENT_ME SDL_GetRGBA
// !!! IMPLEMENT_ME SDL_GetRelativeMouseState
// !!! IMPLEMENT_ME SDL_InitSubSystem
// !!! IMPLEMENT_ME SDL_LoadBMP_RW
// !!! IMPLEMENT_ME SDL_LoadWAV_RW
// !!! IMPLEMENT_ME SDL_LockSurface
// !!! IMPLEMENT_ME SDL_LowerBlit
// !!! IMPLEMENT_ME SDL_MapRGB
// !!! IMPLEMENT_ME SDL_MapRGBA
// !!! IMPLEMENT_ME SDL_PeepEvents
// !!! IMPLEMENT_ME SDL_PollEvent
// !!! IMPLEMENT_ME SDL_PumpEvents
// !!! IMPLEMENT_ME SDL_PushEvent
// !!! IMPLEMENT_ME SDL_QuitSubSystem
// !!! IMPLEMENT_ME SDL_SaveBMP_RW
// !!! IMPLEMENT_ME SDL_SetClipRect
// !!! IMPLEMENT_ME SDL_SetColorKey
// !!! IMPLEMENT_ME SDL_SetCursor
// !!! IMPLEMENT_ME SDL_SetError
// !!! IMPLEMENT_ME SDL_SetEventFilter
// !!! IMPLEMENT_ME SDL_SetModState
// !!! IMPLEMENT_ME SDL_ShowCursor
// !!! IMPLEMENT_ME SDL_SoftStretch
// !!! IMPLEMENT_ME SDL_UnlockSurface
// !!! IMPLEMENT_ME SDL_UpperBlit
// !!! IMPLEMENT_ME SDL_VideoInit
// !!! IMPLEMENT_ME SDL_VideoQuit
// !!! IMPLEMENT_ME SDL_WaitEvent
// !!! IMPLEMENT_ME SDL_WasInit
// !!! IMPLEMENT_ME SDL_iconv
// !!! IMPLEMENT_ME SDL_iconv_string
// !!! IMPLEMENT_ME SDL_lltoa
// !!! IMPLEMENT_ME SDL_ltoa
// !!! IMPLEMENT_ME SDL_revcpy
// !!! IMPLEMENT_ME SDL_strlcat
// !!! IMPLEMENT_ME SDL_strlcpy
// !!! IMPLEMENT_ME SDL_strlwr
// !!! IMPLEMENT_ME SDL_strrev
// !!! IMPLEMENT_ME SDL_strupr
// !!! IMPLEMENT_ME SDL_ulltoa
// !!! IMPLEMENT_ME SDL_ultoa
// !!! IMPLEMENT_ME X11_KeyToUnicode


#define SDL20_SYM(rc,fn,params,args,ret) \
    typedef rc (*SDL20_##fn##_t) params; \
    static SDL20_##fn##_t SDL20_##fn = NULL;
#define SDL20_SYM_PASSTHROUGH(rc,fn,params,args,ret) \
    SDL20_SYM(rc,fn,params,args,ret)
#include "SDL20_syms.h"
#undef SDL20_SYM_PASSTHROUGH
#undef SDL20_SYM




#define SDL12_INIT_TIMER       0x00000001
#define SDL12_INIT_AUDIO       0x00000010
#define SDL12_INIT_VIDEO       0x00000020
#define SDL12_INIT_CDROM       0x00000100
#define SDL12_INIT_JOYSTICK    0x00000200
#define SDL12_INIT_NOPARACHUTE 0x00100000
#define SDL12_INIT_EVENTTHREAD 0x01000000
#define SDL12_INIT_EVERYTHING  0x0000FFFF

typedef struct
{
	Uint32 hw_available :1;	/**< Flag: Can you create hardware surfaces? */
	Uint32 wm_available :1;	/**< Flag: Can you talk to a window manager? */
	Uint32 UnusedBits1  :6;
	Uint32 UnusedBits2  :1;
	Uint32 blit_hw      :1;	/**< Flag: Accelerated blits HW --> HW */
	Uint32 blit_hw_CC   :1;	/**< Flag: Accelerated blits with Colorkey */
	Uint32 blit_hw_A    :1;	/**< Flag: Accelerated blits with Alpha */
	Uint32 blit_sw      :1;	/**< Flag: Accelerated blits SW --> HW */
	Uint32 blit_sw_CC   :1;	/**< Flag: Accelerated blits with Colorkey */
	Uint32 blit_sw_A    :1;	/**< Flag: Accelerated blits with Alpha */
	Uint32 blit_fill    :1;	/**< Flag: Accelerated color fill */
	Uint32 UnusedBits3  :16;
	Uint32 video_mem;	/**< The total amount of video memory (in K) */
	SDL_PixelFormat *vfmt;	/**< Value: The format of the video surface */
	int    current_w;	/**< Value: The current video mode width */
	int    current_h;	/**< Value: The current video mode height */
} SDL12_VideoInfo;

typedef struct SDL12_Palette {
	int       ncolors;
	SDL_Color *colors;
} SDL12_Palette;

/** Everything in the pixel format structure is read-only */
typedef struct SDL12_PixelFormat {
	SDL12_Palette *palette;
	Uint8  BitsPerPixel;
	Uint8  BytesPerPixel;
	Uint8  Rloss;
	Uint8  Gloss;
	Uint8  Bloss;
	Uint8  Aloss;
	Uint8  Rshift;
	Uint8  Gshift;
	Uint8  Bshift;
	Uint8  Ashift;
	Uint32 Rmask;
	Uint32 Gmask;
	Uint32 Bmask;
	Uint32 Amask;

	/** RGB color key information */
	Uint32 colorkey;
	/** Alpha value information (per-surface alpha) */
	Uint8  alpha;
} SDL12_PixelFormat;

#define SDL12_SWSURFACE	0x00000000	/**< Surface is in system memory */
#define SDL12_HWSURFACE	0x00000001	/**< Surface is in video memory */
#define SDL12_ASYNCBLIT	0x00000004	/**< Use asynchronous blits if possible */
#define SDL12_ANYFORMAT	0x10000000	/**< Allow any video depth/pixel-format */
#define SDL12_HWPALETTE	0x20000000	/**< Surface has exclusive palette */
#define SDL12_DOUBLEBUF	0x40000000	/**< Set up double-buffered video mode */
#define SDL12_FULLSCREEN	0x80000000	/**< Surface is a full screen display */
#define SDL12_OPENGL      0x00000002      /**< Create an OpenGL rendering context */
#define SDL12_OPENGLBLIT	0x0000000A	/**< Create an OpenGL rendering context and use it for blitting */
#define SDL12_RESIZABLE	0x00000010	/**< This video mode may be resized */
#define SDL12_NOFRAME	0x00000020	/**< No window caption or edge frame */

#define SDL12_HWACCEL	0x00000100	/**< Blit uses hardware acceleration */
#define SDL12_SRCCOLORKEY	0x00001000	/**< Blit uses a source color key */
#define SDL12_RLEACCELOK	0x00002000	/**< Private flag */
#define SDL12_RLEACCEL	0x00004000	/**< Surface is RLE encoded */
#define SDL12_SRCALPHA	0x00010000	/**< Blit uses source alpha blending */
#define SDL12_PREALLOC	0x01000000	/**< Surface uses preallocated memory */


static SDL_Window *SDL_VideoWindow = NULL;
static SDL_Surface *SDL_WindowSurface = NULL;
static SDL_Surface *SDL_VideoSurface = NULL;
static SDL_Surface *SDL_ShadowSurface = NULL;
static SDL_Surface *SDL_PublicSurface = NULL;
static SDL_GLContext *SDL_VideoContext = NULL;
static Uint32 SDL_VideoFlags = 0;
static SDL_Rect SDL_VideoViewport;
static char *wm_title = NULL;
static char *wm_icon_caption = NULL;
static SDL_Surface *SDL_VideoIcon;
static int SDL_enabled_UNICODE = 0;
static int SDL_VideoDisplayIndex = 0;

/* Obviously we can't use SDL_LoadObject() to load SDL2.  :)  */
#if defined(_WINDOWS)
    #define SDL20_LIBNAME "SDL2.dll"
    static HANDLE Loaded_SDL20 = NULL;
    #define LoadSDL20Library() ((Loaded_SDL20 = LoadLibraryA(SDL20_LIBNAME)) != NULL)
    #define LookupSDL20Sym(sym) GetProcAddress(Loaded_SDL20, sym)
    #define CloseSDL20Library() { { if (Loaded_SDL20) { FreeLibrary(Loaded_SDL20); Loaded_SDL20 = NULL; } }
#elif defined(unix)
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
        if (retval == NULL)
            *okay = 0;
    }
    return retval;
}

static void
UnloadSDL20(void)
{
    #define SDL20_SYM(rc,fn,params,args,ret) SDL20_##fn = NULL;
    #define SDL20_SYM_PASSTHROUGH(rc,fn,params,args,ret) SDL20_##fn = NULL;
    #include "SDL20_syms.h"
    #undef SDL20_SYM_PASSTHROUGH
    #undef SDL20_SYM
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
        #define SDL20_SYM_PASSTHROUGH(rc,fn,params,args,ret) SDL20_SYM(rc,fn,params,args,ret)
        #include "SDL20_syms.h"
        #undef SDL20_SYM_PASSTHROUGH
        #undef SDL20_SYM
        if (!okay)
            UnloadSDL20();
    }
    return okay;
}


static int
GetVideoDisplay()
{
    // !!! FIXME: cache this value during SDL_Init() so it doesn't change.
    const char *variable = SDL_getenv("SDL_VIDEO_FULLSCREEN_DISPLAY");
    if ( !variable ) {
        variable = SDL_getenv("SDL_VIDEO_FULLSCREEN_HEAD");
    }
    if ( variable ) {
        return SDL_atoi(variable);
    } else {
        return 0;
    }
}

int
SDL_Init(Uint32 sdl12flags)
{
    Uint32 sdl20flags = 0;
    int rc;

    if (!LoadSDL20())
        return -1;

    #define SETFLAG(x) if (sdl12flags & SDL12_INIT_##flag) sdl20flags |= SDL_INIT_##flag)
    SETFLAG(TIMER);
    SETFLAG(AUDIO);
    SETFLAG(VIDEO);
    SETFLAG(JOYSTICK);
    SETFLAG(NOPARACHUTE);
    // There's no CDROM in 2.0, but we'll just pretend it succeeded.
    #undef SETFLAG

    // !!! FIXME: do something about SDL12_INIT_EVENTTHREAD

    rc = SDL20_Init(sdl20flags);
    if ((rc == 0) && (sdl20flags & SDL_INIT_VIDEO))
        SDL_VideoDisplayIndex = GetVideoDisplay();
    return rc;
}

void
SDL_Quit(void)
{
    SDL20_Quit();
    UnloadSDL20();
}


char *
SDL_GetError(void)
{
    if (!Loaded_SDL20)
    {
        static char noload_errstr[] = "Failed to load SDL 2.0 shared library";
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

const char *
SDL_AudioDriverName(char *namebuf, int maxlen)
{
    return GetDriverName(SDL20_GetCurrentAudioDriver(), namebuf, maxlen);
}

const char *
SDL_VideoDriverName(char *namebuf, int maxlen)
{
    return GetDriverName(SDL20_GetCurrentVideoDriver(), namebuf, maxlen);
}

const SDL12_VideoInfo *
SDL_GetVideoInfo(void)
{
    static SDL12_VideoInfo info;
    SDL_DisplayMode mode;

    /* !!! FIXME: Memory leak, compatibility code, who cares? */
    if (!info.vfmt && SDL20_GetDesktopDisplayMode(SDL_VideoDisplayIndex, &mode) == 0) {
        info.vfmt = SDL20_AllocFormat(mode.format);
        info.current_w = mode.w;
        info.current_h = mode.h;
        // !!! FIXME
        //info.wm_available = 1;
        //info.video_mem = 1024 * 256;
    }
    return &info;
}

int
SDL_VideoModeOK(int width, int height, int bpp, Uint32 sdl12flags)
{
    int i, nummodes, actual_bpp = 0;

    if (!SDL20_WasInit(SDL_INIT_VIDEO)) {
        return 0;
    }

    if (!(sdl12flags & SDL12_FULLSCREEN)) {
        SDL_DisplayMode mode;
        SDL20_GetDesktopDisplayMode(SDL_VideoDisplayIndex, &mode);
        return SDL_BITSPERPIXEL(mode.format);
    }

    nummodes = SDL20_GetNumDisplayModes(SDL_VideoDisplayIndex);
    for (i = 0; i < nummodes; ++i) {
        SDL_DisplayMode mode;
        SDL20_GetDisplayMode(SDL_VideoDisplayIndex, i, &mode);
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

SDL_Rect **
SDL_ListModes(const SDL12_PixelFormat * format, Uint32 flags)
{
    int i, nmodes;
    SDL_Rect **modes;

    if (!SDL20_WasInit(SDL_INIT_VIDEO)) {
        return NULL;
    }

    if (!(flags & SDL12_FULLSCREEN)) {
        return (SDL_Rect **) (-1);
    }

    if (!format) {
        format = SDL_GetVideoInfo()->vfmt;
    }

    /* Memory leak, but this is a compatibility function, who cares? */
    nmodes = 0;
    modes = NULL;
    for (i = 0; i < SDL20_GetNumDisplayModes(SDL_VideoDisplayIndex); ++i) {
        SDL_DisplayMode mode;
        int bpp;

        SDL20_GetDisplayMode(SDL_VideoDisplayIndex, i, &mode);
        if (!mode.w || !mode.h) {
            return (SDL_Rect **) (-1);
        }
        
        /* Copied from src/video/SDL_pixels.c:SDL_PixelFormatEnumToMasks */
        if (SDL_BYTESPERPIXEL(mode.format) <= 2) {
            bpp = SDL_BITSPERPIXEL(mode.format);
        } else {
            bpp = SDL_BYTESPERPIXEL(mode.format) * 8;
        }

        if (bpp != format->BitsPerPixel) {
            continue;
        }
        if (nmodes > 0 && modes[nmodes - 1]->w == mode.w
            && modes[nmodes - 1]->h == mode.h) {
            continue;
        }

        modes = SDL_realloc(modes, (nmodes + 2) * sizeof(*modes));
        if (!modes) {
            return NULL;
        }
        modes[nmodes] = (SDL_Rect *) SDL_malloc(sizeof(SDL_Rect));
        if (!modes[nmodes]) {
            return NULL;
        }
        modes[nmodes]->x = 0;
        modes[nmodes]->y = 0;
        modes[nmodes]->w = mode.w;
        modes[nmodes]->h = mode.h;
        ++nmodes;
    }
    if (modes) {
        modes[nmodes] = NULL;
    }
    return modes;
}

static int
SDL_CompatEventFilter(void *userdata, SDL_Event * event)
{
    SDL_Event fake;

    switch (event->type) {
    case SDL_WINDOWEVENT:
        switch (event->window.event) {
        case SDL_WINDOWEVENT_EXPOSED:
            if (!SDL_HasEvent(SDL_VIDEOEXPOSE)) {
                fake.type = SDL_VIDEOEXPOSE;
                SDL_PushEvent(&fake);
            }
            break;
        case SDL_WINDOWEVENT_RESIZED:
            SDL_FlushEvent(SDL_VIDEORESIZE);
            /* We don't want to expose that the window width and height will
               be different if we don't get the desired fullscreen mode.
            */
            if (SDL_VideoWindow && !(SDL_GetWindowFlags(SDL_VideoWindow) & SDL_WINDOW_FULLSCREEN)) {
                fake.type = SDL_VIDEORESIZE;
                fake.resize.w = event->window.data1;
                fake.resize.h = event->window.data2;
                SDL_PushEvent(&fake);
            }
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            fake.type = SDL_ACTIVEEVENT;
            fake.active.gain = 0;
            fake.active.state = SDL_APPACTIVE;
            SDL_PushEvent(&fake);
            break;
        case SDL_WINDOWEVENT_RESTORED:
            fake.type = SDL_ACTIVEEVENT;
            fake.active.gain = 1;
            fake.active.state = SDL_APPACTIVE;
            SDL_PushEvent(&fake);
            break;
        case SDL_WINDOWEVENT_ENTER:
            fake.type = SDL_ACTIVEEVENT;
            fake.active.gain = 1;
            fake.active.state = SDL_APPMOUSEFOCUS;
            SDL_PushEvent(&fake);
            break;
        case SDL_WINDOWEVENT_LEAVE:
            fake.type = SDL_ACTIVEEVENT;
            fake.active.gain = 0;
            fake.active.state = SDL_APPMOUSEFOCUS;
            SDL_PushEvent(&fake);
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            fake.type = SDL_ACTIVEEVENT;
            fake.active.gain = 1;
            fake.active.state = SDL_APPINPUTFOCUS;
            SDL_PushEvent(&fake);
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            fake.type = SDL_ACTIVEEVENT;
            fake.active.gain = 0;
            fake.active.state = SDL_APPINPUTFOCUS;
            SDL_PushEvent(&fake);
            break;
        case SDL_WINDOWEVENT_CLOSE:
            fake.type = SDL_QUIT;
            SDL_PushEvent(&fake);
            break;
        }
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        {
            Uint32 unicode = 0;
            if (event->key.type == SDL_KEYDOWN && event->key.keysym.sym < 256) {
                unicode = event->key.keysym.sym;
                if (unicode >= 'a' && unicode <= 'z') {
                    int shifted = !!(event->key.keysym.mod & KMOD_SHIFT);
                    int capslock = !!(event->key.keysym.mod & KMOD_CAPS);
                    if ((shifted ^ capslock) != 0) {
                        unicode = SDL_toupper(unicode);
                    }
                }
            }
            if (unicode) {
                event->key.keysym.unicode = unicode;
            }
            break;
        }
    case SDL_TEXTINPUT:
        {
            /* FIXME: Generate an old style key repeat event if needed */
            //printf("TEXTINPUT: '%s'\n", event->text.text);
            break;
        }
    case SDL_MOUSEMOTION:
        {
            event->motion.x -= SDL_VideoViewport.x;
            event->motion.y -= SDL_VideoViewport.y;
            break;
        }
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        {
            event->button.x -= SDL_VideoViewport.x;
            event->button.y -= SDL_VideoViewport.y;
            break;
        }
    case SDL_MOUSEWHEEL:
        {
            Uint8 button;
            int x, y;

            if (event->wheel.y == 0) {
                break;
            }

            SDL_GetMouseState(&x, &y);

            if (event->wheel.y > 0) {
                button = SDL_BUTTON_WHEELUP;
            } else {
                button = SDL_BUTTON_WHEELDOWN;
            }

            fake.button.button = button;
            fake.button.x = x;
            fake.button.y = y;
            fake.button.windowID = event->wheel.windowID;

            fake.type = SDL_MOUSEBUTTONDOWN;
            fake.button.state = SDL_PRESSED;
            SDL_PushEvent(&fake);

            fake.type = SDL_MOUSEBUTTONUP;
            fake.button.state = SDL_RELEASED;
            SDL_PushEvent(&fake);
            break;
        }

    }
    return 1;
}

static void
GetEnvironmentWindowPosition(int w, int h, int *x, int *y)
{
    int display = SDL_VideoDisplayIndex;
    const char *window = SDL_getenv("SDL_VIDEO_WINDOW_POS");
    const char *center = SDL_getenv("SDL_VIDEO_CENTERED");
    if (window) {
        if (SDL_sscanf(window, "%d,%d", x, y) == 2) {
            return;
        }
        if (SDL_strcmp(window, "center") == 0) {
            center = window;
        }
    }
    if (center) {
        *x = SDL_WINDOWPOS_CENTERED_DISPLAY(display);
        *y = SDL_WINDOWPOS_CENTERED_DISPLAY(display);
    }
}

static void
ClearVideoSurface()
{
    if (SDL_ShadowSurface) {
        SDL_FillRect(SDL_ShadowSurface, NULL,
            SDL_MapRGB(SDL_ShadowSurface->format, 0, 0, 0));
    }
    SDL_FillRect(SDL_WindowSurface, NULL, 0);
    SDL_UpdateWindowSurface(SDL_VideoWindow);
}

static void
SetupScreenSaver(int flags)
{
    const char *env;
    SDL_bool allow_screensaver;

    /* Allow environment override of screensaver disable */
    env = SDL_getenv("SDL_VIDEO_ALLOW_SCREENSAVER");
    if (env) {
        allow_screensaver = SDL_atoi(env) ? SDL_TRUE : SDL_FALSE;
    } else if (flags & SDL_FULLSCREEN) {
        allow_screensaver = SDL_FALSE;
    } else {
        allow_screensaver = SDL_TRUE;
    }
    if (allow_screensaver) {
        SDL_EnableScreenSaver();
    } else {
        SDL_DisableScreenSaver();
    }
}

static int
SDL_ResizeVideoMode(int width, int height, int bpp, Uint32 flags)
{
    int w, h;

    /* We can't resize something we don't have... */
    if (!SDL_VideoSurface) {
        return -1;
    }

    /* We probably have to recreate the window in fullscreen mode */
    if (flags & SDL_FULLSCREEN) {
        return -1;
    }

    /* I don't think there's any change we can gracefully make in flags */
    if (flags != SDL_VideoFlags) {
        return -1;
    }
    if (bpp != SDL_VideoSurface->format->BitsPerPixel) {
        return -1;
    }

    /* Resize the window */
    SDL_GetWindowSize(SDL_VideoWindow, &w, &h);
    if (w != width || h != height) {
        SDL_SetWindowSize(SDL_VideoWindow, width, height);
    }

    /* If we're in OpenGL mode, just resize the stub surface and we're done! */
    if (flags & SDL_OPENGL) {
        SDL_VideoSurface->w = width;
        SDL_VideoSurface->h = height;
        return 0;
    }

    SDL_WindowSurface = SDL_GetWindowSurface(SDL_VideoWindow);
    if (!SDL_WindowSurface) {
        return -1;
    }
    if (SDL_VideoSurface->format != SDL_WindowSurface->format) {
        return -1;
    }
    SDL_VideoSurface->w = width;
    SDL_VideoSurface->h = height;
    SDL_VideoSurface->pixels = SDL_WindowSurface->pixels;
    SDL_VideoSurface->pitch = SDL_WindowSurface->pitch;
    SDL_SetClipRect(SDL_VideoSurface, NULL);

    if (SDL_ShadowSurface) {
        SDL_ShadowSurface->w = width;
        SDL_ShadowSurface->h = height;
        SDL_ShadowSurface->pitch = SDL_CalculatePitch(SDL_ShadowSurface);
        SDL_ShadowSurface->pixels =
            SDL_realloc(SDL_ShadowSurface->pixels,
                        SDL_ShadowSurface->h * SDL_ShadowSurface->pitch);
        SDL_SetClipRect(SDL_ShadowSurface, NULL);
        SDL_InvalidateMap(SDL_ShadowSurface->map);
    } else {
        SDL_PublicSurface = SDL_VideoSurface;
    }

    ClearVideoSurface();

    return 0;
}

SDL_Surface *
SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags)
{
    SDL_DisplayMode desktop_mode;
    int display = SDL_VideoDisplayIndex;
    int window_x = SDL_WINDOWPOS_UNDEFINED_DISPLAY(display);
    int window_y = SDL_WINDOWPOS_UNDEFINED_DISPLAY(display);
    int window_w;
    int window_h;
    Uint32 window_flags;
    Uint32 surface_flags;

    if (!SDL_GetVideoDevice()) {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0) {
            return NULL;
        }
    }

    SDL_GetDesktopDisplayMode(display, &desktop_mode);

    if (width == 0) {
        width = desktop_mode.w;
    }
    if (height == 0) {
        height = desktop_mode.h;
    }
    if (bpp == 0) {
        bpp = SDL_BITSPERPIXEL(desktop_mode.format);
    }

    /* See if we can simply resize the existing window and surface */
    if (SDL_ResizeVideoMode(width, height, bpp, flags) == 0) {
        return SDL_PublicSurface;
    }

    /* Destroy existing window */
    SDL_PublicSurface = NULL;
    if (SDL_ShadowSurface) {
        SDL_ShadowSurface->flags &= ~SDL_DONTFREE;
        SDL_FreeSurface(SDL_ShadowSurface);
        SDL_ShadowSurface = NULL;
    }
    if (SDL_VideoSurface) {
        SDL_VideoSurface->flags &= ~SDL_DONTFREE;
        SDL_FreeSurface(SDL_VideoSurface);
        SDL_VideoSurface = NULL;
    }
    if (SDL_VideoContext) {
        /* SDL_GL_MakeCurrent(0, NULL); *//* Doesn't do anything */
        SDL_GL_DeleteContext(SDL_VideoContext);
        SDL_VideoContext = NULL;
    }
    if (SDL_VideoWindow) {
        SDL_GetWindowPosition(SDL_VideoWindow, &window_x, &window_y);
        SDL_DestroyWindow(SDL_VideoWindow);
    }

    /* Set up the event filter */
    if (!SDL_GetEventFilter(NULL, NULL)) {
        SDL_SetEventFilter(SDL_CompatEventFilter, NULL);
    }

    /* Create a new window */
    window_flags = SDL_WINDOW_SHOWN;
    if (flags & SDL_FULLSCREEN) {
        window_flags |= SDL_WINDOW_FULLSCREEN;
    }
    if (flags & SDL_OPENGL) {
        window_flags |= SDL_WINDOW_OPENGL;
    }
    if (flags & SDL_RESIZABLE) {
        window_flags |= SDL_WINDOW_RESIZABLE;
    }
    if (flags & SDL_NOFRAME) {
        window_flags |= SDL_WINDOW_BORDERLESS;
    }
    GetEnvironmentWindowPosition(width, height, &window_x, &window_y);
    SDL_VideoWindow =
        SDL_CreateWindow(wm_title, window_x, window_y, width, height,
                         window_flags);
    if (!SDL_VideoWindow) {
        return NULL;
    }
    SDL_SetWindowIcon(SDL_VideoWindow, SDL_VideoIcon);

    SetupScreenSaver(flags);

    window_flags = SDL_GetWindowFlags(SDL_VideoWindow);
    surface_flags = 0;
    if (window_flags & SDL_WINDOW_FULLSCREEN) {
        surface_flags |= SDL_FULLSCREEN;
    }
    if ((window_flags & SDL_WINDOW_OPENGL) && (flags & SDL_OPENGL)) {
        surface_flags |= SDL_OPENGL;
    }
    if (window_flags & SDL_WINDOW_RESIZABLE) {
        surface_flags |= SDL_RESIZABLE;
    }
    if (window_flags & SDL_WINDOW_BORDERLESS) {
        surface_flags |= SDL_NOFRAME;
    }

    SDL_VideoFlags = flags;

    /* If we're in OpenGL mode, just create a stub surface and we're done! */
    if (flags & SDL_OPENGL) {
        SDL_VideoContext = SDL_GL_CreateContext(SDL_VideoWindow);
        if (!SDL_VideoContext) {
            return NULL;
        }
        if (SDL_GL_MakeCurrent(SDL_VideoWindow, SDL_VideoContext) < 0) {
            return NULL;
        }
        SDL_VideoSurface =
            SDL_CreateRGBSurfaceFrom(NULL, width, height, bpp, 0, 0, 0, 0, 0);
        if (!SDL_VideoSurface) {
            return NULL;
        }
        SDL_VideoSurface->flags |= surface_flags;
        SDL_PublicSurface = SDL_VideoSurface;
        return SDL_PublicSurface;
    }

    /* Create the screen surface */
    SDL_WindowSurface = SDL_GetWindowSurface(SDL_VideoWindow);
    if (!SDL_WindowSurface) {
        return NULL;
    }

    /* Center the public surface in the window surface */
    SDL_GetWindowSize(SDL_VideoWindow, &window_w, &window_h);
    SDL_VideoViewport.x = (window_w - width)/2;
    SDL_VideoViewport.y = (window_h - height)/2;
    SDL_VideoViewport.w = width;
    SDL_VideoViewport.h = height;

    SDL_VideoSurface = SDL_CreateRGBSurfaceFrom(NULL, 0, 0, 32, 0, 0, 0, 0, 0);
    SDL_VideoSurface->flags |= surface_flags;
    SDL_VideoSurface->flags |= SDL_DONTFREE;
    SDL_FreeFormat(SDL_VideoSurface->format);
    SDL_VideoSurface->format = SDL_WindowSurface->format;
    SDL_VideoSurface->format->refcount++;
    SDL_VideoSurface->w = width;
    SDL_VideoSurface->h = height;
    SDL_VideoSurface->pitch = SDL_WindowSurface->pitch;
    SDL_VideoSurface->pixels = (void *)((Uint8 *)SDL_WindowSurface->pixels +
        SDL_VideoViewport.y * SDL_VideoSurface->pitch +
        SDL_VideoViewport.x  * SDL_VideoSurface->format->BytesPerPixel);
    SDL_SetClipRect(SDL_VideoSurface, NULL);

    /* Create a shadow surface if necessary */
    if ((bpp != SDL_VideoSurface->format->BitsPerPixel)
        && !(flags & SDL_ANYFORMAT)) {
        SDL_ShadowSurface =
            SDL_CreateRGBSurface(0, width, height, bpp, 0, 0, 0, 0);
        if (!SDL_ShadowSurface) {
            return NULL;
        }
        SDL_ShadowSurface->flags |= surface_flags;
        SDL_ShadowSurface->flags |= SDL_DONTFREE;

        /* 8-bit SDL_ShadowSurface surfaces report that they have exclusive palette */
        if (SDL_ShadowSurface->format->palette) {
            SDL_ShadowSurface->flags |= SDL_HWPALETTE;
            SDL_DitherColors(SDL_ShadowSurface->format->palette->colors,
                             SDL_ShadowSurface->format->BitsPerPixel);
        }
        SDL_FillRect(SDL_ShadowSurface, NULL,
            SDL_MapRGB(SDL_ShadowSurface->format, 0, 0, 0));
    }
    SDL_PublicSurface =
        (SDL_ShadowSurface ? SDL_ShadowSurface : SDL_VideoSurface);

    ClearVideoSurface();

    /* We're finally done! */
    return SDL_PublicSurface;
}

SDL_Surface *
SDL_GetVideoSurface(void)
{
    return SDL_PublicSurface;
}

int
SDL_SetAlpha(SDL_Surface * surface, Uint32 flag, Uint8 value)
{
    if (flag & SDL_SRCALPHA) {
        /* According to the docs, value is ignored for alpha surfaces */
        if (surface->format->Amask) {
            value = 0xFF;
        }
        SDL_SetSurfaceAlphaMod(surface, value);
        SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);
    } else {
        SDL_SetSurfaceAlphaMod(surface, 0xFF);
        SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
    }
    SDL_SetSurfaceRLE(surface, (flag & SDL_RLEACCEL));

    return 0;
}

SDL_Surface *
SDL_DisplayFormat(SDL_Surface * surface)
{
    SDL_PixelFormat *format;

    if (!SDL_PublicSurface) {
        SDL_SetError("No video mode has been set");
        return NULL;
    }
    format = SDL_PublicSurface->format;

    /* Set the flags appropriate for copying to display surface */
    return SDL_ConvertSurface(surface, format, SDL_RLEACCEL);
}

SDL_Surface *
SDL_DisplayFormatAlpha(SDL_Surface * surface)
{
    SDL_PixelFormat *vf;
    SDL_PixelFormat *format;
    SDL_Surface *converted;
    /* default to ARGB8888 */
    Uint32 amask = 0xff000000;
    Uint32 rmask = 0x00ff0000;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x000000ff;

    if (!SDL_PublicSurface) {
        SDL_SetError("No video mode has been set");
        return NULL;
    }
    vf = SDL_PublicSurface->format;

    switch (vf->BytesPerPixel) {
    case 2:
        /* For XGY5[56]5, use, AXGY8888, where {X, Y} = {R, B}.
           For anything else (like ARGB4444) it doesn't matter
           since we have no special code for it anyway */
        if ((vf->Rmask == 0x1f) &&
            (vf->Bmask == 0xf800 || vf->Bmask == 0x7c00)) {
            rmask = 0xff;
            bmask = 0xff0000;
        }
        break;

    case 3:
    case 4:
        /* Keep the video format, as long as the high 8 bits are
           unused or alpha */
        if ((vf->Rmask == 0xff) && (vf->Bmask == 0xff0000)) {
            rmask = 0xff;
            bmask = 0xff0000;
        }
        break;

    default:
        /* We have no other optimised formats right now. When/if a new
           optimised alpha format is written, add the converter here */
        break;
    }
    format = SDL_AllocFormat(SDL_MasksToPixelFormatEnum(32, rmask,
                                                            gmask,
                                                            bmask,
                                                            amask));
    if (!format) {
        return NULL;
    }
    converted = SDL_ConvertSurface(surface, format, SDL_RLEACCEL);
    SDL_FreeFormat(format);
    return converted;
}

int
SDL_Flip(SDL_Surface * screen)
{
    SDL_UpdateRect(screen, 0, 0, 0, 0);
    return 0;
}

void
SDL_UpdateRect(SDL_Surface * screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h)
{
    if (screen) {
        SDL_Rect rect;

        /* Fill the rectangle */
        rect.x = (int) x;
        rect.y = (int) y;
        rect.w = (int) (w ? w : screen->w);
        rect.h = (int) (h ? h : screen->h);
        SDL_UpdateRects(screen, 1, &rect);
    }
}

void
SDL_UpdateRects(SDL_Surface * screen, int numrects, SDL_Rect * rects)
{
    int i;

    if (screen == SDL_ShadowSurface) {
        for (i = 0; i < numrects; ++i) {
            SDL_BlitSurface(SDL_ShadowSurface, &rects[i], SDL_VideoSurface,
                            &rects[i]);
        }

        /* Fall through to video surface update */
        screen = SDL_VideoSurface;
    }
    if (screen == SDL_VideoSurface) {
        if (SDL_VideoViewport.x || SDL_VideoViewport.y) {
            SDL_Rect *stackrects = SDL_stack_alloc(SDL_Rect, numrects);
            SDL_Rect *stackrect;
            const SDL_Rect *rect;
            
            /* Offset all the rectangles before updating */
            for (i = 0; i < numrects; ++i) {
                rect = &rects[i];
                stackrect = &stackrects[i];
                stackrect->x = SDL_VideoViewport.x + rect->x;
                stackrect->y = SDL_VideoViewport.y + rect->y;
                stackrect->w = rect->w;
                stackrect->h = rect->h;
            }
            SDL_UpdateWindowSurfaceRects(SDL_VideoWindow, stackrects, numrects);
            SDL_stack_free(stackrects);
        } else {
            SDL_UpdateWindowSurfaceRects(SDL_VideoWindow, rects, numrects);
        }
    }
}

void
SDL_WM_SetCaption(const char *title, const char *icon)
{
    if (wm_title) {
        SDL_free(wm_title);
    }
    if (wm_icon_caption) {
        SDL_free(wm_icon_caption);
    }
    wm_title = title ? SDL_strdup(title) : NULL;
    wm_icon_caption = icon ? SDL_strdup(icon) : NULL;
    SDL_SetWindowTitle(SDL_VideoWindow, wm_title);
}

void
SDL_WM_GetCaption(const char **title, const char **icon)
{
    if (title) {
        *title = wm_title;
    }
    if (icon) {
        *icon = wm_icon_caption;
    }
}

void
SDL_WM_SetIcon(SDL_Surface * icon, Uint8 * mask)
{
    // !!! FIXME: free previous icon?
    SDL_VideoIcon = icon;
    ++SDL_VideoIcon->refcount;
}

int
SDL_WM_IconifyWindow(void)
{
    SDL_MinimizeWindow(SDL_VideoWindow);
    return 0;
}

int
SDL_WM_ToggleFullScreen(SDL_Surface * surface)
{
    int length;
    void *pixels;
    Uint8 *src, *dst;
    int row;
    int window_w;
    int window_h;

    if (!SDL_PublicSurface) {
        SDL_SetError("SDL_SetVideoMode() hasn't been called");
        return 0;
    }

    /* Copy the old bits out */
    length = SDL_PublicSurface->w * SDL_PublicSurface->format->BytesPerPixel;
    pixels = SDL_malloc(SDL_PublicSurface->h * length);
    if (pixels && SDL_PublicSurface->pixels) {
        src = (Uint8*)SDL_PublicSurface->pixels;
        dst = (Uint8*)pixels;
        for (row = 0; row < SDL_PublicSurface->h; ++row) {
            SDL_memcpy(dst, src, length);
            src += SDL_PublicSurface->pitch;
            dst += length;
        }
    }

    /* Do the physical mode switch */
    if (SDL_GetWindowFlags(SDL_VideoWindow) & SDL_WINDOW_FULLSCREEN) {
        if (SDL_SetWindowFullscreen(SDL_VideoWindow, 0) < 0) {
            return 0;
        }
        SDL_PublicSurface->flags &= ~SDL_FULLSCREEN;
    } else {
        if (SDL_SetWindowFullscreen(SDL_VideoWindow, 1) < 0) {
            return 0;
        }
        SDL_PublicSurface->flags |= SDL_FULLSCREEN;
    }

    /* Recreate the screen surface */
    SDL_WindowSurface = SDL_GetWindowSurface(SDL_VideoWindow);
    if (!SDL_WindowSurface) {
        /* We're totally hosed... */
        return 0;
    }

    /* Center the public surface in the window surface */
    SDL_GetWindowSize(SDL_VideoWindow, &window_w, &window_h);
    SDL_VideoViewport.x = (window_w - SDL_VideoSurface->w)/2;
    SDL_VideoViewport.y = (window_h - SDL_VideoSurface->h)/2;
    SDL_VideoViewport.w = SDL_VideoSurface->w;
    SDL_VideoViewport.h = SDL_VideoSurface->h;

    /* Do some shuffling behind the application's back if format changes */
    if (SDL_VideoSurface->format->format != SDL_WindowSurface->format->format) {
        if (SDL_ShadowSurface) {
            if (SDL_ShadowSurface->format->format == SDL_WindowSurface->format->format) {
                /* Whee!  We don't need a shadow surface anymore! */
                SDL_VideoSurface->flags &= ~SDL_DONTFREE;
                SDL_FreeSurface(SDL_VideoSurface);
                SDL_free(SDL_ShadowSurface->pixels);
                SDL_VideoSurface = SDL_ShadowSurface;
                SDL_VideoSurface->flags |= SDL_PREALLOC;
                SDL_ShadowSurface = NULL;
            } else {
                /* No problem, just change the video surface format */
                SDL_FreeFormat(SDL_VideoSurface->format);
                SDL_VideoSurface->format = SDL_WindowSurface->format;
                SDL_VideoSurface->format->refcount++;
                SDL_InvalidateMap(SDL_ShadowSurface->map);
            }
        } else {
            /* We can make the video surface the shadow surface */
            SDL_ShadowSurface = SDL_VideoSurface;
            SDL_ShadowSurface->pitch = SDL_CalculatePitch(SDL_ShadowSurface);
            SDL_ShadowSurface->pixels = SDL_malloc(SDL_ShadowSurface->h * SDL_ShadowSurface->pitch);
            if (!SDL_ShadowSurface->pixels) {
                /* Uh oh, we're hosed */
                SDL_ShadowSurface = NULL;
                return 0;
            }
            SDL_ShadowSurface->flags &= ~SDL_PREALLOC;

            SDL_VideoSurface = SDL_CreateRGBSurfaceFrom(NULL, 0, 0, 32, 0, 0, 0, 0, 0);
            SDL_VideoSurface->flags = SDL_ShadowSurface->flags;
            SDL_VideoSurface->flags |= SDL_PREALLOC;
            SDL_FreeFormat(SDL_VideoSurface->format);
            SDL_VideoSurface->format = SDL_WindowSurface->format;
            SDL_VideoSurface->format->refcount++;
            SDL_VideoSurface->w = SDL_ShadowSurface->w;
            SDL_VideoSurface->h = SDL_ShadowSurface->h;
        }
    }

    /* Update the video surface */
    SDL_VideoSurface->pitch = SDL_WindowSurface->pitch;
    SDL_VideoSurface->pixels = (void *)((Uint8 *)SDL_WindowSurface->pixels +
        SDL_VideoViewport.y * SDL_VideoSurface->pitch +
        SDL_VideoViewport.x  * SDL_VideoSurface->format->BytesPerPixel);
    SDL_SetClipRect(SDL_VideoSurface, NULL);

    /* Copy the old bits back */
    if (pixels) {
        src = (Uint8*)pixels;
        dst = (Uint8*)SDL_PublicSurface->pixels;
        for (row = 0; row < SDL_PublicSurface->h; ++row) {
            SDL_memcpy(dst, src, length);
            src += length;
            dst += SDL_PublicSurface->pitch;
        }
        SDL_Flip(SDL_PublicSurface);
        SDL_free(pixels);
    }

    /* We're done! */
    return 1;
}

SDL_GrabMode
SDL_WM_GrabInput(SDL_GrabMode mode)
{
    if (mode != SDL_GRAB_QUERY) {
        SDL_SetWindowGrab(SDL_VideoWindow, mode);
    }
    return (SDL_GrabMode) SDL_GetWindowGrab(SDL_VideoWindow);
}

void
SDL_WarpMouse(Uint16 x, Uint16 y)
{
    SDL_WarpMouseInWindow(SDL_VideoWindow, x, y);
}

Uint8
SDL_GetAppState(void)
{
    Uint8 state = 0;
    Uint32 flags = 0;

    flags = SDL_GetWindowFlags(SDL_VideoWindow);
    if ((flags & SDL_WINDOW_SHOWN) && !(flags & SDL_WINDOW_MINIMIZED)) {
        state |= SDL_APPACTIVE;
    }
    if (flags & SDL_WINDOW_INPUT_FOCUS) {
        state |= SDL_APPINPUTFOCUS;
    }
    if (flags & SDL_WINDOW_MOUSE_FOCUS) {
        state |= SDL_APPMOUSEFOCUS;
    }
    return state;
}

const SDL_version *
SDL_Linked_Version(void)
{
    static SDL_version version;
    SDL_VERSION(&version);
    return &version;
}

int
SDL_SetPalette(SDL_Surface * surface, int flags, const SDL_Color * colors,
               int firstcolor, int ncolors)
{
    return SDL_SetColors(surface, colors, firstcolor, ncolors);
}

int
SDL_SetColors(SDL_Surface * surface, const SDL_Color * colors, int firstcolor,
              int ncolors)
{
    if (SDL_SetPaletteColors
        (surface->format->palette, colors, firstcolor, ncolors) == 0) {
        return 1;
    } else {
        return 0;
    }
}

int
SDL_GetWMInfo(SDL_SysWMinfo * info)
{
    return SDL_GetWindowWMInfo(SDL_VideoWindow, info);
}

#if 0
void
SDL_MoveCursor(int x, int y)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();

    /* Erase and update the current mouse position */
    if (SHOULD_DRAWCURSOR(SDL_cursorstate)) {
        /* Erase and redraw mouse cursor in new position */
        SDL_LockCursor();
        SDL_EraseCursor(SDL_VideoSurface);
        SDL_cursor->area.x = (x - SDL_cursor->hot_x);
        SDL_cursor->area.y = (y - SDL_cursor->hot_y);
        SDL_DrawCursor(SDL_VideoSurface);
        SDL_UnlockCursor();
    } else if (_this->MoveWMCursor) {
        _this->MoveWMCursor(_this, x, y);
    }
}

/* Keep track of the current cursor colors */
static int palette_changed = 1;
static Uint8 pixels8[2];

void
SDL_CursorPaletteChanged(void)
{
    palette_changed = 1;
}

void
SDL_MouseRect(SDL_Rect * area)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    int clip_diff;

    *area = SDL_cursor->area;
    if (area->x < 0) {
        area->w += area->x;
        area->x = 0;
    }
    if (area->y < 0) {
        area->h += area->y;
        area->y = 0;
    }
    clip_diff = (area->x + area->w) - SDL_VideoSurface->w;
    if (clip_diff > 0) {
        area->w = area->w < clip_diff ? 0 : area->w - clip_diff;
    }
    clip_diff = (area->y + area->h) - SDL_VideoSurface->h;
    if (clip_diff > 0) {
        area->h = area->h < clip_diff ? 0 : area->h - clip_diff;
    }
}

static void
SDL_DrawCursorFast(SDL_Surface * screen, SDL_Rect * area)
{
    const Uint32 pixels[2] = { 0xFFFFFFFF, 0x00000000 };
    int i, w, h;
    Uint8 *data, datab;
    Uint8 *mask, maskb;

    data = SDL_cursor->data + area->y * SDL_cursor->area.w / 8;
    mask = SDL_cursor->mask + area->y * SDL_cursor->area.w / 8;
    switch (screen->format->BytesPerPixel) {

    case 1:
        {
            Uint8 *dst;
            int dstskip;

            if (palette_changed) {
                pixels8[0] =
                    (Uint8) SDL_MapRGB(screen->format, 255, 255, 255);
                pixels8[1] = (Uint8) SDL_MapRGB(screen->format, 0, 0, 0);
                palette_changed = 0;
            }
            dst = (Uint8 *) screen->pixels +
                (SDL_cursor->area.y + area->y) * screen->pitch +
                SDL_cursor->area.x;
            dstskip = screen->pitch - area->w;

            for (h = area->h; h; h--) {
                for (w = area->w / 8; w; w--) {
                    maskb = *mask++;
                    datab = *data++;
                    for (i = 0; i < 8; ++i) {
                        if (maskb & 0x80) {
                            *dst = pixels8[datab >> 7];
                        }
                        maskb <<= 1;
                        datab <<= 1;
                        dst++;
                    }
                }
                dst += dstskip;
            }
        }
        break;

    case 2:
        {
            Uint16 *dst;
            int dstskip;

            dst = (Uint16 *) screen->pixels +
                (SDL_cursor->area.y + area->y) * screen->pitch / 2 +
                SDL_cursor->area.x;
            dstskip = (screen->pitch / 2) - area->w;

            for (h = area->h; h; h--) {
                for (w = area->w / 8; w; w--) {
                    maskb = *mask++;
                    datab = *data++;
                    for (i = 0; i < 8; ++i) {
                        if (maskb & 0x80) {
                            *dst = (Uint16) pixels[datab >> 7];
                        }
                        maskb <<= 1;
                        datab <<= 1;
                        dst++;
                    }
                }
                dst += dstskip;
            }
        }
        break;

    case 3:
        {
            Uint8 *dst;
            int dstskip;

            dst = (Uint8 *) screen->pixels +
                (SDL_cursor->area.y + area->y) * screen->pitch +
                SDL_cursor->area.x * 3;
            dstskip = screen->pitch - area->w * 3;

            for (h = area->h; h; h--) {
                for (w = area->w / 8; w; w--) {
                    maskb = *mask++;
                    datab = *data++;
                    for (i = 0; i < 8; ++i) {
                        if (maskb & 0x80) {
                            SDL_memset(dst, pixels[datab >> 7], 3);
                        }
                        maskb <<= 1;
                        datab <<= 1;
                        dst += 3;
                    }
                }
                dst += dstskip;
            }
        }
        break;

    case 4:
        {
            Uint32 *dst;
            int dstskip;

            dst = (Uint32 *) screen->pixels +
                (SDL_cursor->area.y + area->y) * screen->pitch / 4 +
                SDL_cursor->area.x;
            dstskip = (screen->pitch / 4) - area->w;

            for (h = area->h; h; h--) {
                for (w = area->w / 8; w; w--) {
                    maskb = *mask++;
                    datab = *data++;
                    for (i = 0; i < 8; ++i) {
                        if (maskb & 0x80) {
                            *dst = pixels[datab >> 7];
                        }
                        maskb <<= 1;
                        datab <<= 1;
                        dst++;
                    }
                }
                dst += dstskip;
            }
        }
        break;
    }
}

static void
SDL_DrawCursorSlow(SDL_Surface * screen, SDL_Rect * area)
{
    const Uint32 pixels[2] = { 0xFFFFFF, 0x000000 };
    int h;
    int x, minx, maxx;
    Uint8 *data, datab = 0;
    Uint8 *mask, maskb = 0;
    Uint8 *dst;
    int dstbpp, dstskip;

    data = SDL_cursor->data + area->y * SDL_cursor->area.w / 8;
    mask = SDL_cursor->mask + area->y * SDL_cursor->area.w / 8;
    dstbpp = screen->format->BytesPerPixel;
    dst = (Uint8 *) screen->pixels +
        (SDL_cursor->area.y + area->y) * screen->pitch +
        SDL_cursor->area.x * dstbpp;
    dstskip = screen->pitch - SDL_cursor->area.w * dstbpp;

    minx = area->x;
    maxx = area->x + area->w;
    if (screen->format->BytesPerPixel == 1) {
        if (palette_changed) {
            pixels8[0] = (Uint8) SDL_MapRGB(screen->format, 255, 255, 255);
            pixels8[1] = (Uint8) SDL_MapRGB(screen->format, 0, 0, 0);
            palette_changed = 0;
        }
        for (h = area->h; h; h--) {
            for (x = 0; x < SDL_cursor->area.w; ++x) {
                if ((x % 8) == 0) {
                    maskb = *mask++;
                    datab = *data++;
                }
                if ((x >= minx) && (x < maxx)) {
                    if (maskb & 0x80) {
                        SDL_memset(dst, pixels8[datab >> 7], dstbpp);
                    }
                }
                maskb <<= 1;
                datab <<= 1;
                dst += dstbpp;
            }
            dst += dstskip;
        }
    } else {
        for (h = area->h; h; h--) {
            for (x = 0; x < SDL_cursor->area.w; ++x) {
                if ((x % 8) == 0) {
                    maskb = *mask++;
                    datab = *data++;
                }
                if ((x >= minx) && (x < maxx)) {
                    if (maskb & 0x80) {
                        SDL_memset(dst, pixels[datab >> 7], dstbpp);
                    }
                }
                maskb <<= 1;
                datab <<= 1;
                dst += dstbpp;
            }
            dst += dstskip;
        }
    }
}

/* This handles the ugly work of converting the saved cursor background from
   the pixel format of the shadow surface to that of the video surface.
   This is only necessary when blitting from a shadow surface of a different
   pixel format than the video surface, and using a software rendered cursor.
*/
static void
SDL_ConvertCursorSave(SDL_Surface * screen, int w, int h)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_BlitInfo info;
    SDL_loblit RunBlit;

    /* Make sure we can steal the blit mapping */
    if (screen->map->dst != SDL_VideoSurface) {
        return;
    }

    /* Set up the blit information */
    info.s_pixels = SDL_cursor->save[1];
    info.s_width = w;
    info.s_height = h;
    info.s_skip = 0;
    info.d_pixels = SDL_cursor->save[0];
    info.d_width = w;
    info.d_height = h;
    info.d_skip = 0;
    info.aux_data = screen->map->sw_data->aux_data;
    info.src = screen->format;
    info.table = screen->map->table;
    info.dst = SDL_VideoSurface->format;
    RunBlit = screen->map->sw_data->blit;

    /* Run the actual software blit */
    RunBlit(&info);
}

void
SDL_DrawCursorNoLock(SDL_Surface * screen)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_Rect area;

    /* Get the mouse rectangle, clipped to the screen */
    SDL_MouseRect(&area);
    if ((area.w == 0) || (area.h == 0)) {
        return;
    }

    /* Copy mouse background */
    {
        int w, h, screenbpp;
        Uint8 *src, *dst;

        /* Set up the copy pointers */
        screenbpp = screen->format->BytesPerPixel;
        if ((screen == SDL_VideoSurface) ||
            FORMAT_EQUAL(screen->format, SDL_VideoSurface->format)) {
            dst = SDL_cursor->save[0];
        } else {
            dst = SDL_cursor->save[1];
        }
        src = (Uint8 *) screen->pixels + area.y * screen->pitch +
            area.x * screenbpp;

        /* Perform the copy */
        w = area.w * screenbpp;
        h = area.h;
        while (h--) {
            SDL_memcpy(dst, src, w);
            dst += w;
            src += screen->pitch;
        }
    }

    /* Draw the mouse cursor */
    area.x -= SDL_cursor->area.x;
    area.y -= SDL_cursor->area.y;
    if ((area.x == 0) && (area.w == SDL_cursor->area.w)) {
        SDL_DrawCursorFast(screen, &area);
    } else {
        SDL_DrawCursorSlow(screen, &area);
    }
}

void
SDL_DrawCursor(SDL_Surface * screen)
{
    /* Lock the screen if necessary */
    if (screen == NULL) {
        return;
    }
    if (SDL_MUSTLOCK(screen)) {
        if (SDL_LockSurface(screen) < 0) {
            return;
        }
    }

    SDL_DrawCursorNoLock(screen);

    /* Unlock the screen and update if necessary */
    if (SDL_MUSTLOCK(screen)) {
        SDL_UnlockSurface(screen);
    }
    if (screen->flags & SDL_SCREEN_SURFACE) {
        SDL_VideoDevice *_this = SDL_GetVideoDevice();
        SDL_Window *window;
        SDL_Rect area;

        window = SDL_GetWindowFromSurface(screen);
        if (!window) {
            return;
        }

        SDL_MouseRect(&area);

        if (_this->UpdateWindowSurface) {
            _this->UpdateWindowSurface(_this, window, 1, &area);
        }
    }
}

void
SDL_EraseCursorNoLock(SDL_Surface * screen)
{
    SDL_VideoDevice *_this = SDL_GetVideoDevice();
    SDL_Window *window;
    SDL_Rect area;

    /* Get the window associated with the surface */
    window = SDL_GetWindowFromSurface(screen);
    if (!window || !window->surface) {
        return;
    }

    /* Get the mouse rectangle, clipped to the screen */
    SDL_MouseRect(&area);
    if ((area.w == 0) || (area.h == 0)) {
        return;
    }

    /* Copy mouse background */
    {
        int w, h, screenbpp;
        Uint8 *src, *dst;

        /* Set up the copy pointers */
        screenbpp = screen->format->BytesPerPixel;
        if ((screen->flags & SDL_SCREEN_SURFACE) ||
            FORMAT_EQUAL(screen->format, window->surface->format)) {
            src = SDL_cursor->save[0];
        } else {
            src = SDL_cursor->save[1];
        }
        dst = (Uint8 *) screen->pixels + area.y * screen->pitch +
            area.x * screenbpp;

        /* Perform the copy */
        w = area.w * screenbpp;
        h = area.h;
        while (h--) {
            SDL_memcpy(dst, src, w);
            src += w;
            dst += screen->pitch;
        }

        /* Perform pixel conversion on cursor background */
        if (src > SDL_cursor->save[1]) {
            SDL_ConvertCursorSave(screen, area.w, area.h);
        }
    }
}

void
SDL_EraseCursor(SDL_Surface * screen)
{
    /* Lock the screen if necessary */
    if (screen == NULL) {
        return;
    }
    if (SDL_MUSTLOCK(screen)) {
        if (SDL_LockSurface(screen) < 0) {
            return;
        }
    }

    SDL_EraseCursorNoLock(screen);

    /* Unlock the screen and update if necessary */
    if (SDL_MUSTLOCK(screen)) {
        SDL_UnlockSurface(screen);
    }
    if (screen->flags & SDL_SCREEN_SURFACE) {
        SDL_VideoDevice *_this = SDL_GetVideoDevice();
        SDL_Window *window;
        SDL_Rect area;

        window = SDL_GetWindowFromSurface(screen);
        if (!window) {
            return;
        }

        SDL_MouseRect(&area);

        if (_this->UpdateWindowSurface) {
            _this->UpdateWindowSurface(_this, window, 1, &area);
        }
    }
}

/* Reset the cursor on video mode change
   FIXME:  Keep track of all cursors, and reset them all.
 */
void
SDL_ResetCursor(void)
{
    int savelen;

    if (SDL_cursor) {
        savelen = SDL_cursor->area.w * 4 * SDL_cursor->area.h;
        SDL_cursor->area.x = 0;
        SDL_cursor->area.y = 0;
        SDL_memset(SDL_cursor->save[0], 0, savelen);
    }
}
#endif

struct private_yuvhwdata
{
    SDL_SW_YUVTexture *texture;
    SDL_Surface *display;
    Uint32 display_format;
};

SDL_Overlay *
SDL_CreateYUVOverlay(int w, int h, Uint32 format, SDL_Surface * display)
{
    SDL_Overlay *overlay;
    Uint32 texture_format;
    SDL_SW_YUVTexture *texture;

    if ((display->flags & SDL_OPENGL) == SDL_OPENGL) {
        SDL_SetError("YUV overlays are not supported in OpenGL mode");
        return NULL;
    }

    if (display != SDL_PublicSurface) {
        SDL_SetError("YUV display is only supported on the screen surface");
        return NULL;
    }

    switch (format) {
    case SDL_YV12_OVERLAY:
        texture_format = SDL_PIXELFORMAT_YV12;
        break;
    case SDL_IYUV_OVERLAY:
        texture_format = SDL_PIXELFORMAT_IYUV;
        break;
    case SDL_YUY2_OVERLAY:
        texture_format = SDL_PIXELFORMAT_YUY2;
        break;
    case SDL_UYVY_OVERLAY:
        texture_format = SDL_PIXELFORMAT_UYVY;
        break;
    case SDL_YVYU_OVERLAY:
        texture_format = SDL_PIXELFORMAT_YVYU;
        break;
    default:
        SDL_SetError("Unknown YUV format");
        return NULL;
    }

    overlay = (SDL_Overlay *) SDL_malloc(sizeof(*overlay));
    if (!overlay) {
        SDL_OutOfMemory();
        return NULL;
    }
    SDL_zerop(overlay);

    overlay->hwdata =
        (struct private_yuvhwdata *) SDL_malloc(sizeof(*overlay->hwdata));
    if (!overlay->hwdata) {
        SDL_free(overlay);
        SDL_OutOfMemory();
        return NULL;
    }

    texture = SDL_SW_CreateYUVTexture(texture_format, w, h);
    if (!texture) {
        SDL_free(overlay->hwdata);
        SDL_free(overlay);
        return NULL;
    }
    overlay->hwdata->texture = texture;
    overlay->hwdata->display = NULL;
    overlay->hwdata->display_format = SDL_PIXELFORMAT_UNKNOWN;

    overlay->format = format;
    overlay->w = w;
    overlay->h = h;
    if (format == SDL_YV12_OVERLAY || format == SDL_IYUV_OVERLAY) {
        overlay->planes = 3;
    } else {
        overlay->planes = 1;
    }
    overlay->pitches = texture->pitches;
    overlay->pixels = texture->planes;

    return overlay;
}

int
SDL_LockYUVOverlay(SDL_Overlay * overlay)
{
    SDL_Rect rect;
    void *pixels;
    int pitch;

    if (!overlay) {
        SDL_SetError("Passed a NULL overlay");
        return -1;
    }

    rect.x = 0;
    rect.y = 0;
    rect.w = overlay->w;
    rect.h = overlay->h;

    if (SDL_SW_LockYUVTexture(overlay->hwdata->texture, &rect, &pixels, &pitch) < 0) {
        return -1;
    }

    overlay->pixels[0] = (Uint8 *) pixels;
    overlay->pitches[0] = pitch;
    switch (overlay->format) {
    case SDL_YV12_OVERLAY:
    case SDL_IYUV_OVERLAY:
        overlay->pitches[1] = pitch / 2;
        overlay->pitches[2] = pitch / 2;
        overlay->pixels[1] =
            overlay->pixels[0] + overlay->pitches[0] * overlay->h;
        overlay->pixels[2] =
            overlay->pixels[1] + overlay->pitches[1] * overlay->h / 2;
        break;
    case SDL_YUY2_OVERLAY:
    case SDL_UYVY_OVERLAY:
    case SDL_YVYU_OVERLAY:
        break;
    }
    return 0;
}

void
SDL_UnlockYUVOverlay(SDL_Overlay * overlay)
{
    if (!overlay) {
        return;
    }

    SDL_SW_UnlockYUVTexture(overlay->hwdata->texture);
}

int
SDL_DisplayYUVOverlay(SDL_Overlay * overlay, SDL_Rect * dstrect)
{
    SDL_Surface *display;
    SDL_Rect src_rect;
    SDL_Rect dst_rect;
    void *pixels;

    if (!overlay || !dstrect) {
        SDL_SetError("Passed a NULL overlay or dstrect");
        return -1;
    }

    display = overlay->hwdata->display;
    if (display != SDL_VideoSurface) {
        overlay->hwdata->display = display = SDL_VideoSurface;
        overlay->hwdata->display_format = SDL_MasksToPixelFormatEnum(
                                                display->format->BitsPerPixel,
                                                display->format->Rmask,
                                                display->format->Gmask,
                                                display->format->Bmask,
                                                display->format->Amask);
    }

    src_rect.x = 0;
    src_rect.y = 0;
    src_rect.w = overlay->w;
    src_rect.h = overlay->h;

    if (!SDL_IntersectRect(&display->clip_rect, dstrect, &dst_rect)) {
        return 0;
    }
     
    pixels = (void *)((Uint8 *)display->pixels +
                        dst_rect.y * display->pitch +
                        dst_rect.x * display->format->BytesPerPixel);

    if (SDL_SW_CopyYUVToRGB(overlay->hwdata->texture, &src_rect,
                            overlay->hwdata->display_format,
                            dst_rect.w, dst_rect.h,
                            pixels, display->pitch) < 0) {
        return -1;
    }
    SDL_UpdateWindowSurface(SDL_VideoWindow);
    return 0;
}

void
SDL_FreeYUVOverlay(SDL_Overlay * overlay)
{
    if (!overlay) {
        return;
    }
    if (overlay->hwdata) {
        if (overlay->hwdata->texture) {
            SDL_SW_DestroyYUVTexture(overlay->hwdata->texture);
        }
        SDL_free(overlay->hwdata);
    }
    SDL_free(overlay);
}

void
SDL_GL_SwapBuffers(void)
{
    SDL_GL_SwapWindow(SDL_VideoWindow);
}

int
SDL_SetGamma(float red, float green, float blue)
{
    Uint16 red_ramp[256];
    Uint16 green_ramp[256];
    Uint16 blue_ramp[256];

    SDL_CalculateGammaRamp(red, red_ramp);
    if (green == red) {
        SDL_memcpy(green_ramp, red_ramp, sizeof(red_ramp));
    } else {
        SDL_CalculateGammaRamp(green, green_ramp);
    }
    if (blue == red) {
        SDL_memcpy(blue_ramp, red_ramp, sizeof(red_ramp));
    } else {
        SDL_CalculateGammaRamp(blue, blue_ramp);
    }
    return SDL_SetWindowGammaRamp(SDL_VideoWindow, red_ramp, green_ramp, blue_ramp);
}

int
SDL_SetGammaRamp(const Uint16 * red, const Uint16 * green, const Uint16 * blue)
{
    return SDL_SetWindowGammaRamp(SDL_VideoWindow, red, green, blue);
}

int
SDL_GetGammaRamp(Uint16 * red, Uint16 * green, Uint16 * blue)
{
    return SDL_GetWindowGammaRamp(SDL_VideoWindow, red, green, blue);
}

int
SDL_EnableKeyRepeat(int delay, int interval)
{
    return 0;
}

void
SDL_GetKeyRepeat(int *delay, int *interval)
{
    if (delay) {
        *delay = SDL_DEFAULT_REPEAT_DELAY;
    }
    if (interval) {
        *interval = SDL_DEFAULT_REPEAT_INTERVAL;
    }
}

int
SDL_EnableUNICODE(int enable)
{
    int previous = SDL_enabled_UNICODE;

    switch (enable) {
    case 1:
        SDL_enabled_UNICODE = 1;
        SDL_StartTextInput();
        break;
    case 0:
        SDL_enabled_UNICODE = 0;
        SDL_StopTextInput();
        break;
    }
    return previous;
}

static Uint32
SDL_SetTimerCallback(Uint32 interval, void* param)
{
    return ((SDL_OldTimerCallback)param)(interval);
}

int
SDL_SetTimer(Uint32 interval, SDL_OldTimerCallback callback)
{
    static SDL_TimerID compat_timer;

    if (compat_timer) {
        SDL20_RemoveTimer(compat_timer);
        compat_timer = 0;
    }

    if (interval && callback) {
        compat_timer = SDL20_AddTimer(interval, SDL_SetTimerCallback, callback);
        if (!compat_timer) {
            return -1;
        }
    }
    return 0;
}

int
SDL_putenv(const char *_var)
{
    char *ptr = NULL;
    char *var = SDL_strdup(_var);
    if (var == NULL) {
        return -1;  /* we don't set errno. */
    }

    ptr = SDL_strchr(var, '=');
    if (ptr == NULL) {
        SDL_free(var);
        return -1;
    }

    *ptr = '\0';  /* split the string into name and value. */
    SDL_setenv(var, ptr + 1, 1);
    SDL_free(var);
    return 0;
}



/* CD-ROM support is gone from SDL 2.0, so just have stubs that fail. */

typedef void *SDL12_CD;  /* close enough.  :) */
typedef int SDL12_CDstatus;  /* close enough.  :) */

static int
CDUnsupported(void)
{
    SDL_SetError("CD interface is unsupported");
    return -1;
}

int
SDL_CDNumDrives(void)
{
    return 0;  /* !!! FIXME: should return -1 without SDL_INIT_CDROM */
}

const char *SDL_CDName(int drive) { CDUnsupported(); return NULL; }
SDL12_CD * SDL_CDOpen(int drive) { CDUnsupported(); return NULL; }
SDL12_CDstatus SDL_CDStatus(SDL_CD *cdrom) { return CDUnsupported(); }
int SDL_CDPlayTracks(SDL12_CD *cdrom, int start_track, int start_frame, int ntracks, int nframes) { return CDUnsupported(); }
int SDL_CDPlay(SDL12_CD *cdrom, int start, int length) { return CDUnsupported(); }
int SDL_CDPause(SDL12_CD *cdrom) { return CDUnsupported(); }
int SDL_CDResume(SDL12_CD *cdrom) { return CDUnsupported(); }
int SDL_CDStop(SDL12_CD *cdrom) { return CDUnsupported(); }
int SDL_CDEject(SDL12_CD *cdrom) { return CDUnsupported(); }
void SDL_CDClose(SDL12_CD *cdrom) {}

/* !!! FIXME: Removed from 2.0; do nothing. We can't even report failure. */
void SDL_KillThread(SDL_Thread *thread) {}

/* This changed from an opaque pointer to an int in 2.0. */
typedef struct _SDL12_TimerID *SDL12_TimerID;
SDL_COMPILE_TIME_ASSERT(timer, sizeof(SDL12_TimerID) >= sizeof(SDL_TimerID));

SDL12_TimerID
SDL_AddTimer(Uint32 interval, SDL_NewTimerCallback callback, void *param)
{
    return (SDL12_TimerID) ((size_t) SDL20_AddTimer(interval, callback, param));
}

SDL_bool
SDL_RemoveTimer(SDL12_TimerID id)
{
    return SDL20_RemoveTimer((SDL_TimerID) ((size_t)id));
}


typedef struct SDL12_RWops {
    int (SDLCALL *seek)(struct SDL_RWops *context, int offset, int whence);
    int (SDLCALL *read)(struct SDL_RWops *context, void *ptr, int size, int maxnum);
    int (SDLCALL *write)(struct SDL_RWops *context, const void *ptr, int size, int num);
    int (SDLCALL *close)(struct SDL_RWops *context);
    Uint32 type;
    void *padding[8];
    SDL_RWops *rwops20;
} SDL12_RWops;


SDL12_RWops *
SDL_AllocRW(void)
{
    SDL12_RWops *rwops = (SDL12_RWops *) SDL_malloc(sizeof (SDL12_RWops));
    if (!rwops)
        SDL20_OutOfMemory();
    return rwops;
}

void
SDL_FreeRW(SDL12_RWops *rwops12)
{
    SDL_free(rwops12);
}

static int SDLCALL
WrapRWops_seek(struct SDL12_RWops *rwops12, int offset, int whence)
{
    return rwops12->rwops20->seek(rwops12->rwops20, offset, whence);
}

static int SDLCALL
WrapRWops_read(struct SDL12_RWops *rwops12, void *ptr, int size, int maxnum)
{
    return rwops12->rwops20->read(rwops12->rwops20, ptr, size, maxnum);
}

static int SDLCALL
WrapRWops_write(struct SDL12_RWops *rwops12, const void *ptr, int size, int num)
{
    return rwops12->rwops20->write(rwops12->rwops20, ptr, size, num);
}

static int SDLCALL
WrapRWops_close(struct SDL12_RWops *rwops12)
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
WrapRWops(SDL12_RWops *rwops12, SDL_RWops *rwops20)
{
    if (!rwops20)
    {
        SDL_FreeRW(rwops12);
        return NULL;
    }
    SDL_zerop(rwops12);
    rwops12->type = rwops20->type;
    rwops12->rwops20 = rwops20;
    rwops12->seek = WrapRWops_seek;
    rwops12->read = WrapRWops_read;
    rwops12->write = WrapRWops_write;
    rwops12->close = WrapRWops_close;
}

SDL12_RWops *
SDL_RWFromFile(const char *file, const char *mode)
{
    SDL12_RWops *rwops12 = SDL_AllocRW();
    return rwops12 ? WrapRWops(rwops12, SDL20_RWFromFile(file, mode)) : NULL;
}

SDL12_RWops *
SDL_RWFromFP(FILE *io, int autoclose)
{
    SDL12_RWops *rwops12 = SDL_AllocRW();
    return rwops12 ? WrapRWops(rwops12, SDL20_RWFromFP(io, autoclose)) : NULL;
}

SDL12_RWops *
SDL_RWFromMem(void *mem, int size)
{
    SDL12_RWops *rwops12 = SDL_AllocRW();
    return rwops12 ? WrapRWops(rwops12, SDL20_RWFromMem(mem, size)) : NULL;
}

SDL12_RWops *
SDL_RWFromConstMem(const void *mem, int size)
{
    SDL12_RWops *rwops12 = SDL_AllocRW();
    return rwops12 ? WrapRWops(rwops12, SDL20_RWFromConstMem(mem, size)) : NULL;
}

#define READ_AND_BYTESWAP(endian, bits) \
    Uint##bits SDL_Read##endian##bits(SDL12_RWops *rwops12) { \
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
    int SDL_Write##endian##bits(SDL12_RWops *rwops12, Uint##endian##bits val) { \
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

/* Things that _should_ be binary compatible pass right through... */
#define SDL20_SYM(rc,fn,params,args,ret)
#define SDL20_SYM_PASSTHROUGH(rc,fn,params,args,ret) \
    rc SDL_##fn params { ret SDL20_##fn args; }
#include "SDL20_syms.h"
#undef SDL20_SYM_PASSTHROUGH
#undef SDL20_SYM

/* vi: set ts=4 sw=4 expandtab: */
