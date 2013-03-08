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

#include <stdarg.h>

//#include "video/SDL_sysvideo.h"
//#include "video/SDL_pixels_c.h"
//#include "render/SDL_yuv_sw_c.h"

// !!! IMPLEMENT_ME SDL_ConvertSurface
// !!! IMPLEMENT_ME SDL_CreateCursor
// !!! IMPLEMENT_ME SDL_CreateThread
// !!! IMPLEMENT_ME SDL_EventState
// !!! IMPLEMENT_ME SDL_FillRect
// !!! IMPLEMENT_ME SDL_FreeCursor
// !!! IMPLEMENT_ME SDL_GL_GetAttribute
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
// !!! IMPLEMENT_ME SDL_GetRelativeMouseState
// !!! IMPLEMENT_ME SDL_LockSurface
// !!! IMPLEMENT_ME SDL_LowerBlit
// !!! IMPLEMENT_ME SDL_PeepEvents
// !!! IMPLEMENT_ME SDL_PollEvent
// !!! IMPLEMENT_ME SDL_PumpEvents
// !!! IMPLEMENT_ME SDL_PushEvent
// !!! IMPLEMENT_ME SDL_SetClipRect
// !!! IMPLEMENT_ME SDL_SetColorKey
// !!! IMPLEMENT_ME SDL_SetCursor
// !!! IMPLEMENT_ME SDL_SetEventFilter
// !!! IMPLEMENT_ME SDL_SetModState
// !!! IMPLEMENT_ME SDL_ShowCursor
// !!! IMPLEMENT_ME SDL_SoftStretch
// !!! IMPLEMENT_ME SDL_UnlockSurface
// !!! IMPLEMENT_ME SDL_UpperBlit
// !!! IMPLEMENT_ME SDL_WaitEvent
// !!! IMPLEMENT_ME X11_KeyToUnicode


#define SDL20_SYM(rc,fn,params,args,ret) \
    typedef rc (*SDL20_##fn##_t) params; \
    static SDL20_##fn##_t SDL20_##fn = NULL;
#define SDL20_SYM_PASSTHROUGH(rc,fn,params,args,ret) \
    SDL20_SYM(rc,fn,params,args,ret)
#include "SDL20_syms.h"
#undef SDL20_SYM_PASSTHROUGH
#undef SDL20_SYM

typedef void (*SDL20_SetError_t)(const char *fmt, ...);
static SDL20_SetError_t SDL20_SetError = NULL;



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


static SDL12_VideoInfo VideoInfo;
static SDL_Window *VideoWindow = NULL;
static SDL_Surface *WindowSurface = NULL;
static SDL_Surface *VideoSurface = NULL;
static SDL_Surface *ShadowSurface = NULL;
static SDL_Surface *PublicSurface = NULL;
static SDL_GLContext *VideoContext = NULL;
static Uint32 VideoFlags = 0;
static SDL_Rect VideoViewport;
static char *WindowTitle = NULL;
static char *WindowIconTitle = NULL;
static SDL_Surface *VideoIcon;
static int EnabledUnicode = 0;
static int VideoDisplayIndex = 0;
static int CDRomInit = 0;

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
    SDL20_SetError = NULL;
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
        SDL20_SetError = (SDL20_SetError_t) LoadSDL20Symbol("SDL_SetError", &okay);
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

static int
DoSDLInit(const int justsubs, Uint32 sdl12flags)
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
    #undef SETFLAG

    // There's no CDROM in 2.0, but we'll just pretend it succeeded.
    if (sdl12flags & SDL12_INIT_CDROM)
        CDRomInit = 1;

    // !!! FIXME: do something about SDL12_INIT_EVENTTHREAD

    rc = justsubs ? SDL20_InitSubSystem(sdl20flags) : SDL20_Init(sdl20flags);
    if ((rc == 0) && (sdl20flags & SDL_INIT_VIDEO))
        VideoDisplayIndex = GetVideoDisplay();
    return rc;
}

int
SDL_InitSubSystem(Uint32 sdl12flags)
{
    return DoSDLInit(1, sdl12flags);
}

int
SDL_Init(Uint32 sdl12flags)
{
    return DoSDLInit(0, sdl12flags);
}

Uint32
SDL_WasInit(Uint32 sdl12flags)
{
    // !!! FIXME: this is cut and pasted several places.
    Uint32 sdl20flags = 0;
    Uint32 extraflags = 0;

    #define SETFLAG(x) if (sdl12flags & SDL12_INIT_##flag) sdl20flags |= SDL_INIT_##flag)
    SETFLAG(TIMER);
    SETFLAG(AUDIO);
    SETFLAG(VIDEO);
    SETFLAG(JOYSTICK);
    SETFLAG(NOPARACHUTE);
    #undef SETFLAG

    if ((sdl12flags & SDL12_INIT_CDROM) && (CDRomInit))
        extraflags |= SDL12_INIT_CDROM;

    // !!! FIXME: do something about SDL12_INIT_EVENTTHREAD

    // !!! FIXME: convert back to 1.2
    return SDL20_WasInit(sdl20flags) | extraflags;
}

void
SDL_QuitSubSystem(Uint32 sdl12flags)
{
    Uint32 sdl20flags = 0;

    #define SETFLAG(x) if (sdl12flags & SDL12_INIT_##flag) sdl20flags |= SDL_INIT_##flag)
    SETFLAG(TIMER);
    SETFLAG(AUDIO);
    SETFLAG(VIDEO);
    SETFLAG(JOYSTICK);
    SETFLAG(NOPARACHUTE);
    // There's no CDROM in 2.0, but we'll just pretend it succeeded.
    #undef SETFLAG

    if (sdl12flags & SDL12_INIT_CDROM)
        CDRomInit = 0;

    // !!! FIXME: reset a bunch of other global variables too.
    if (sdl12flags & SDL12_INIT_VIDEO) {
        SDL20_FreeFormat(VideoInfo.vfmt);
        SDL_zero(VideoInfo);
    }

    // !!! FIXME: do something about SDL12_INIT_EVENTTHREAD
    SDL20_QuitSubSystem(sdl20flags);

    // !!! FIXME: UnloadSDL20() ?
}

void
SDL_Quit(void)
{
    // !!! FIXME: reset a bunch of other global variables too.
    SDL20_FreeFormat(VideoInfo.vfmt);
    SDL_zero(VideoInfo);
    CDRomInit = 0;
    SDL20_Quit();
    UnloadSDL20();
}

void
SDL_SetError(const char *fmt, ...)
{
    char *str = NULL;
    va_list ap;
    va_start(ap, fmt);
    vasprintf(&str, fmt, ap);
    va_end(ap);
    if (!str)
        SDL20_OutOfMemory();
    else
    {
        SDL20_SetError("%s", str);
        free(str);
    }
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

typedef struct SDL12_Surface
{
    Uint32 flags;
    SDL12_PixelFormat *format;
    int w;
    int h;
    Uint16 pitch;
    void *pixels;
    int offset;
    void *hwdata;
    SDL_Rect clip_rect;
    Uint32 unused1;
    Uint32 locked;
    void *blitmap;
    unsigned int format_version;
    int refcount;
} SDL12_Surface;

SDL12_Surface *
Surface20to12(SDL_Surface *surface20)
{
    SDL12_Surface *surface12 = NULL;
    SDL12_Palette *palette12 = NULL;
    SDL12_PixelFormat *format12 = NULL;
    UInt32 flags = 0;

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

    SDL_zerop(palette12);
    palette12->ncolors = surface20->palette->ncolors;
    palette12->colors = surface20->palette->colors;

    SDL_zerop(format12);
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
    /* !!! FIXME: format12->colorkey; */
    /* !!! FIXME: format12->alpha; */

    SDL_zerop(surface12);
    flags = surface20->flags;
    #define MAPSURFACEFLAGS(fl) { if (surface20->flags & SDL_##fl) { surface12->flags |= SDL12_##fl; flags &= ~SDL_##fl; } }
    MAPSURFACEFLAGS(PREALLOC);
    MAPSURFACEFLAGS(RLEACCEL);
    MAPSURFACEFLAGS(DONTFREE);
    #undef MAPSURFACEFLAGS
    assert(flags == 0);  /* non-zero if there's a flag we didn't map. */

    surface12->format = format12;
    surface12->w = surface20->w;
    surface12->h = surface20->h;
    surface12->pitch = (Uint16) surface20->pitch;  /* !!! FIXME: make sure this fits in a Uint16 */
    surface12->pixels = surface20->pixels;
    surface12->offset = 0;
    surface12->hwdata = surface20;
    SDL20_memcpy(&surface12->clip_rect, &surface20->clip_rect, sizeof (SDL_Rect));
    surface12->refcount = surface20->refcount;

    return surface12;

failed:
    SDL20_free(surface12);
    SDL20_free(palette12);
    SDL20_free(format12);
    return NULL;
}

SDL12_Surface *
SDL_CreateRGBSurface(Uint32 sdl12flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    SDL_Surface *surface20 = SDL20_CreateRGBSurface(0, width, height, depth, Rmask, Gmask, Bmask, Amask);
    SDL12_Surface *surface12 = Surface20to12(surface20);
    if (!surface12) {
        SDL20_FreeSurface(surface20);
        return NULL;
    }

    assert(surface12->flags == 0);  // shouldn't have prealloc, rleaccel, or dontfree.
    return surface12;
}

SDL12_Surface *
SDL_CreateRGBSurfaceFrom(void *pixels, int width, int height, int depth, int pitch, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    SDL_Surface *surface20 = SDL20_CreateRGBSurfaceFrom(pixels, width, height, depth, Rmask, Gmask, Bmask, Amask);
    SDL12_Surface *surface12 = Surface20to12(surface20);
    if (!surface12) {
        SDL20_FreeSurface(surface20);
        return NULL;
    }

    assert(surface12->flags == SDL12_PREALLOC);  // should _only_ have prealloc.
    return surface12;
}

void SDL_FreeSurface(SDL12_Surface *surface12)
{
    if (surface12) {
        SDL20_FreeSurface((SDL_Surface *) surface12->hwdata);
        if (surface12->format) {
            SDL20_free(surface12->format->palette);
            SDL20_free(surface12->format);
        }
        SDL20_free(surface12);
    }
}

static SDL_PixelFormat *
PixelFormat12to20(SDL_PixelFormat *format20, SDL_Palette *palette20, SDL12_PixelFormat *format12)
{
    palette20->ncolors = format12->palette->ncolors;
    palette20->colors = format12->palette->colors;
    palette20->version = 1;
    palette20->refcount = 1;
    format20->format = SDL_MasksToPixelFormatEnum(format12->BitsPerPixel, format12->Rmask, format12->Gmask, format12->Bmask, format12->Amask);
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

Uint32
SDL_MapRGB(const SDL12_PixelFormat *format12, Uint8 r, Uint8 g, Uint8 b)
{
    /* This is probably way slower than apps expect. */
    SDL_PixelFormat format20;
    SDL_Palette palette20;
    return SDL20_MapRGB(PixelFormat12to20(&format20, &palette20, format12), r, g, b);
}

Uint32
SDL_MapRGBA(const SDL12_PixelFormat *format12, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    /* This is probably way slower than apps expect. */
    SDL_PixelFormat format20;
    SDL_Palette palette20;
    return SDL20_MapRGBA(PixelFormat12to20(&format20, &palette20, format12), r, g, b, a);
}

void
SDL_GetRGB(Uint32 pixel, const SDL12_PixelFormat *format12, Uint8 *r, Uint8 *g, Uint8 *b)
{
    /* This is probably way slower than apps expect. */
    SDL_PixelFormat format20;
    SDL_Palette palette20;
    return SDL20_GetRGB(pixel, PixelFormat12to20(&format20, &palette20, format12), r, g, b);
}

void
SDL_GetRGBA(Uint32 pixel, const SDL12_PixelFormat *format12, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a)
{
    /* This is probably way slower than apps expect. */
    SDL_PixelFormat format20;
    SDL_Palette palette20;
    return SDL20_GetRGB(pixel, PixelFormat12to20(&format20, &palette20, format12), r, g, b, a);
}

const SDL12_VideoInfo *
SDL_GetVideoInfo(void)
{
    SDL_DisplayMode mode;

    if (!VideoInfo.vfmt && SDL20_GetDesktopDisplayMode(VideoDisplayIndex, &mode) == 0) {
        VideoInfo.vfmt = SDL20_AllocFormat(mode.format);
        VideoInfo.current_w = mode.w;
        VideoInfo.current_h = mode.h;
        // !!! FIXME
        //VideoInfo.wm_available = 1;
        //VideoInfo.video_mem = 1024 * 256;
    }
    return &VideoInfo;
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

SDL_Rect **
SDL_ListModes(const SDL12_PixelFormat *format, Uint32 flags)
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
        format = VideoInfo.vfmt;
    }

    /* !!! FIXME: Memory leak */
    nmodes = 0;
    modes = NULL;
    for (i = 0; i < SDL20_GetNumDisplayModes(VideoDisplayIndex); ++i) {
        SDL_DisplayMode mode;
        int bpp;

        SDL20_GetDisplayMode(VideoDisplayIndex, i, &mode);
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

        modes = SDL20_realloc(modes, (nmodes + 2) * sizeof(*modes));
        if (!modes) {
            return NULL;
        }
        modes[nmodes] = (SDL_Rect *) SDL20_malloc(sizeof(SDL_Rect));
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

/* !!! FIXME: don't need a filter, just do this in the SDL_PumpEvents() implementation. */
static int
SDL_CompatEventFilter(void *userdata, SDL_Event * event)
{
    SDL_Event fake;

    switch (event->type) {
    case SDL_WINDOWEVENT:
        switch (event->window.event) {
        case SDL_WINDOWEVENT_EXPOSED:
            if (!SDL20_HasEvent(SDL_VIDEOEXPOSE)) {
                fake.type = SDL_VIDEOEXPOSE;
                SDL20_PushEvent(&fake);
            }
            break;
        case SDL_WINDOWEVENT_RESIZED:
            SDL_FlushEvent(SDL_VIDEORESIZE);
            /* We don't want to expose that the window width and height will
               be different if we don't get the desired fullscreen mode.
            */
            if (VideoWindow && !(SDL_GetWindowFlags(VideoWindow) & SDL_WINDOW_FULLSCREEN)) {
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
            event->motion.x -= VideoViewport.x;
            event->motion.y -= VideoViewport.y;
            break;
        }
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        {
            event->button.x -= VideoViewport.x;
            event->button.y -= VideoViewport.y;
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
    int display = VideoDisplayIndex;
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
    if (ShadowSurface) {
        SDL20_FillRect(ShadowSurface, NULL,
            SDL20_MapRGB(ShadowSurface->format, 0, 0, 0));
    }
    SDL20_FillRect(WindowSurface, NULL, 0);
    SDL20_UpdateWindowSurface(VideoWindow);
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
        SDL20_EnableScreenSaver();
    } else {
        SDL20_DisableScreenSaver();
    }
}

static int
ResizeVideoMode(int width, int height, int bpp, Uint32 flags)
{
    int w, h;

    /* We can't resize something we don't have... */
    if (!VideoSurface) {
        return -1;
    }

    /* We probably have to recreate the window in fullscreen mode */
    if (flags & SDL_FULLSCREEN) {
        return -1;
    }

    /* I don't think there's any change we can gracefully make in flags */
    if (flags != VideoFlags) {
        return -1;
    }
    if (bpp != VideoSurface->format->BitsPerPixel) {
        return -1;
    }

    /* Resize the window */
    SDL_GetWindowSize(VideoWindow, &w, &h);
    if (w != width || h != height) {
        SDL_SetWindowSize(VideoWindow, width, height);
    }

    /* If we're in OpenGL mode, just resize the stub surface and we're done! */
    if (flags & SDL_OPENGL) {
        VideoSurface->w = width;
        VideoSurface->h = height;
        return 0;
    }

    WindowSurface = SDL_GetWindowSurface(VideoWindow);
    if (!WindowSurface) {
        return -1;
    }
    if (VideoSurface->format != WindowSurface->format) {
        return -1;
    }
    VideoSurface->w = width;
    VideoSurface->h = height;
    VideoSurface->pixels = WindowSurface->pixels;
    VideoSurface->pitch = WindowSurface->pitch;
    SDL_SetClipRect(VideoSurface, NULL);

    if (ShadowSurface) {
        ShadowSurface->w = width;
        ShadowSurface->h = height;
        ShadowSurface->pitch = SDL_CalculatePitch(ShadowSurface);
        ShadowSurface->pixels =
            SDL_realloc(ShadowSurface->pixels,
                        ShadowSurface->h * ShadowSurface->pitch);
        SDL_SetClipRect(ShadowSurface, NULL);
        SDL_InvalidateMap(ShadowSurface->map);
    } else {
        PublicSurface = VideoSurface;
    }

    ClearVideoSurface();

    return 0;
}

SDL_Surface *
SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags)
{
    SDL_DisplayMode desktop_mode;
    int display = VideoDisplayIndex;
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
    if (ResizeVideoMode(width, height, bpp, flags) == 0) {
        return PublicSurface;
    }

    /* Destroy existing window */
    PublicSurface = NULL;
    if (ShadowSurface) {
        ShadowSurface->flags &= ~SDL_DONTFREE;
        SDL_FreeSurface(ShadowSurface);
        ShadowSurface = NULL;
    }
    if (VideoSurface) {
        VideoSurface->flags &= ~SDL_DONTFREE;
        SDL_FreeSurface(VideoSurface);
        VideoSurface = NULL;
    }
    if (VideoContext) {
        /* SDL_GL_MakeCurrent(0, NULL); *//* Doesn't do anything */
        SDL_GL_DeleteContext(VideoContext);
        VideoContext = NULL;
    }
    if (VideoWindow) {
        SDL_GetWindowPosition(VideoWindow, &window_x, &window_y);
        SDL_DestroyWindow(VideoWindow);
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
    VideoWindow =
        SDL_CreateWindow(WindowTitle, window_x, window_y, width, height,
                         window_flags);
    if (!VideoWindow) {
        return NULL;
    }
    SDL_SetWindowIcon(VideoWindow, VideoIcon);

    SetupScreenSaver(flags);

    window_flags = SDL_GetWindowFlags(VideoWindow);
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

    VideoFlags = flags;

    /* If we're in OpenGL mode, just create a stub surface and we're done! */
    if (flags & SDL_OPENGL) {
        VideoContext = SDL_GL_CreateContext(VideoWindow);
        if (!VideoContext) {
            return NULL;
        }
        if (SDL_GL_MakeCurrent(VideoWindow, VideoContext) < 0) {
            return NULL;
        }
        VideoSurface =
            SDL_CreateRGBSurfaceFrom(NULL, width, height, bpp, 0, 0, 0, 0, 0);
        if (!VideoSurface) {
            return NULL;
        }
        VideoSurface->flags |= surface_flags;
        PublicSurface = VideoSurface;
        return PublicSurface;
    }

    /* Create the screen surface */
    WindowSurface = SDL_GetWindowSurface(VideoWindow);
    if (!WindowSurface) {
        return NULL;
    }

    /* Center the public surface in the window surface */
    SDL_GetWindowSize(VideoWindow, &window_w, &window_h);
    VideoViewport.x = (window_w - width)/2;
    VideoViewport.y = (window_h - height)/2;
    VideoViewport.w = width;
    VideoViewport.h = height;

    VideoSurface = SDL_CreateRGBSurfaceFrom(NULL, 0, 0, 32, 0, 0, 0, 0, 0);
    VideoSurface->flags |= surface_flags;
    VideoSurface->flags |= SDL_DONTFREE;
    SDL_FreeFormat(VideoSurface->format);
    VideoSurface->format = WindowSurface->format;
    VideoSurface->format->refcount++;
    VideoSurface->w = width;
    VideoSurface->h = height;
    VideoSurface->pitch = WindowSurface->pitch;
    VideoSurface->pixels = (void *)((Uint8 *)WindowSurface->pixels +
        VideoViewport.y * VideoSurface->pitch +
        VideoViewport.x  * VideoSurface->format->BytesPerPixel);
    SDL_SetClipRect(VideoSurface, NULL);

    /* Create a shadow surface if necessary */
    if ((bpp != VideoSurface->format->BitsPerPixel)
        && !(flags & SDL_ANYFORMAT)) {
        ShadowSurface =
            SDL_CreateRGBSurface(0, width, height, bpp, 0, 0, 0, 0);
        if (!ShadowSurface) {
            return NULL;
        }
        ShadowSurface->flags |= surface_flags;
        ShadowSurface->flags |= SDL_DONTFREE;

        /* 8-bit ShadowSurface surfaces report that they have exclusive palette */
        if (ShadowSurface->format->palette) {
            ShadowSurface->flags |= SDL_HWPALETTE;
            SDL_DitherColors(ShadowSurface->format->palette->colors,
                             ShadowSurface->format->BitsPerPixel);
        }
        SDL_FillRect(ShadowSurface, NULL,
            SDL_MapRGB(ShadowSurface->format, 0, 0, 0));
    }
    PublicSurface =
        (ShadowSurface ? ShadowSurface : VideoSurface);

    ClearVideoSurface();

    /* We're finally done! */
    return PublicSurface;
}

SDL_Surface *
SDL_GetVideoSurface(void)
{
    return PublicSurface;
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

    if (!PublicSurface) {
        SDL_SetError("No video mode has been set");
        return NULL;
    }
    format = PublicSurface->format;

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

    if (!PublicSurface) {
        SDL_SetError("No video mode has been set");
        return NULL;
    }
    vf = PublicSurface->format;

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

    if (screen == ShadowSurface) {
        for (i = 0; i < numrects; ++i) {
            SDL_BlitSurface(ShadowSurface, &rects[i], VideoSurface,
                            &rects[i]);
        }

        /* Fall through to video surface update */
        screen = VideoSurface;
    }
    if (screen == VideoSurface) {
        if (VideoViewport.x || VideoViewport.y) {
            SDL_Rect *stackrects = SDL_stack_alloc(SDL_Rect, numrects);
            SDL_Rect *stackrect;
            const SDL_Rect *rect;
            
            /* Offset all the rectangles before updating */
            for (i = 0; i < numrects; ++i) {
                rect = &rects[i];
                stackrect = &stackrects[i];
                stackrect->x = VideoViewport.x + rect->x;
                stackrect->y = VideoViewport.y + rect->y;
                stackrect->w = rect->w;
                stackrect->h = rect->h;
            }
            SDL_UpdateWindowSurfaceRects(VideoWindow, stackrects, numrects);
            SDL_stack_free(stackrects);
        } else {
            SDL_UpdateWindowSurfaceRects(VideoWindow, rects, numrects);
        }
    }
}

void
SDL_WM_SetCaption(const char *title, const char *icon)
{
    if (WindowTitle) {
        SDL_free(WindowTitle);
    }
    if (WindowIconTitle) {
        SDL_free(WindowIconTitle);
    }
    WindowTitle = title ? SDL_strdup(title) : NULL;
    WindowIconTitle = icon ? SDL_strdup(icon) : NULL;
    SDL_SetWindowTitle(VideoWindow, WindowTitle);
}

void
SDL_WM_GetCaption(const char **title, const char **icon)
{
    if (title) {
        *title = WindowTitle;
    }
    if (icon) {
        *icon = WindowIconTitle;
    }
}

void
SDL_WM_SetIcon(SDL_Surface * icon, Uint8 * mask)
{
    // !!! FIXME: free previous icon?
    VideoIcon = icon;
    ++VideoIcon->refcount;
}

int
SDL_WM_IconifyWindow(void)
{
    SDL_MinimizeWindow(VideoWindow);
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

    if (!PublicSurface) {
        SDL_SetError("SDL_SetVideoMode() hasn't been called");
        return 0;
    }

    /* Copy the old bits out */
    length = PublicSurface->w * PublicSurface->format->BytesPerPixel;
    pixels = SDL_malloc(PublicSurface->h * length);
    if (pixels && PublicSurface->pixels) {
        src = (Uint8*)PublicSurface->pixels;
        dst = (Uint8*)pixels;
        for (row = 0; row < PublicSurface->h; ++row) {
            SDL_memcpy(dst, src, length);
            src += PublicSurface->pitch;
            dst += length;
        }
    }

    /* Do the physical mode switch */
    if (SDL_GetWindowFlags(VideoWindow) & SDL_WINDOW_FULLSCREEN) {
        if (SDL_SetWindowFullscreen(VideoWindow, 0) < 0) {
            return 0;
        }
        PublicSurface->flags &= ~SDL_FULLSCREEN;
    } else {
        if (SDL_SetWindowFullscreen(VideoWindow, 1) < 0) {
            return 0;
        }
        PublicSurface->flags |= SDL_FULLSCREEN;
    }

    /* Recreate the screen surface */
    WindowSurface = SDL_GetWindowSurface(VideoWindow);
    if (!WindowSurface) {
        /* We're totally hosed... */
        return 0;
    }

    /* Center the public surface in the window surface */
    SDL_GetWindowSize(VideoWindow, &window_w, &window_h);
    VideoViewport.x = (window_w - VideoSurface->w)/2;
    VideoViewport.y = (window_h - VideoSurface->h)/2;
    VideoViewport.w = VideoSurface->w;
    VideoViewport.h = VideoSurface->h;

    /* Do some shuffling behind the application's back if format changes */
    if (VideoSurface->format->format != WindowSurface->format->format) {
        if (ShadowSurface) {
            if (ShadowSurface->format->format == WindowSurface->format->format) {
                /* Whee!  We don't need a shadow surface anymore! */
                VideoSurface->flags &= ~SDL_DONTFREE;
                SDL_FreeSurface(VideoSurface);
                SDL_free(ShadowSurface->pixels);
                VideoSurface = ShadowSurface;
                VideoSurface->flags |= SDL_PREALLOC;
                ShadowSurface = NULL;
            } else {
                /* No problem, just change the video surface format */
                SDL_FreeFormat(VideoSurface->format);
                VideoSurface->format = WindowSurface->format;
                VideoSurface->format->refcount++;
                SDL_InvalidateMap(ShadowSurface->map);
            }
        } else {
            /* We can make the video surface the shadow surface */
            ShadowSurface = VideoSurface;
            ShadowSurface->pitch = SDL_CalculatePitch(ShadowSurface);
            ShadowSurface->pixels = SDL_malloc(ShadowSurface->h * ShadowSurface->pitch);
            if (!ShadowSurface->pixels) {
                /* Uh oh, we're hosed */
                ShadowSurface = NULL;
                return 0;
            }
            ShadowSurface->flags &= ~SDL_PREALLOC;

            VideoSurface = SDL_CreateRGBSurfaceFrom(NULL, 0, 0, 32, 0, 0, 0, 0, 0);
            VideoSurface->flags = ShadowSurface->flags;
            VideoSurface->flags |= SDL_PREALLOC;
            SDL_FreeFormat(VideoSurface->format);
            VideoSurface->format = WindowSurface->format;
            VideoSurface->format->refcount++;
            VideoSurface->w = ShadowSurface->w;
            VideoSurface->h = ShadowSurface->h;
        }
    }

    /* Update the video surface */
    VideoSurface->pitch = WindowSurface->pitch;
    VideoSurface->pixels = (void *)((Uint8 *)WindowSurface->pixels +
        VideoViewport.y * VideoSurface->pitch +
        VideoViewport.x  * VideoSurface->format->BytesPerPixel);
    SDL_SetClipRect(VideoSurface, NULL);

    /* Copy the old bits back */
    if (pixels) {
        src = (Uint8*)pixels;
        dst = (Uint8*)PublicSurface->pixels;
        for (row = 0; row < PublicSurface->h; ++row) {
            SDL_memcpy(dst, src, length);
            src += length;
            dst += PublicSurface->pitch;
        }
        SDL_Flip(PublicSurface);
        SDL_free(pixels);
    }

    /* We're done! */
    return 1;
}

SDL_GrabMode
SDL_WM_GrabInput(SDL_GrabMode mode)
{
    if (mode != SDL_GRAB_QUERY) {
        SDL_SetWindowGrab(VideoWindow, mode);
    }
    return (SDL_GrabMode) SDL_GetWindowGrab(VideoWindow);
}

void
SDL_WarpMouse(Uint16 x, Uint16 y)
{
    SDL_WarpMouseInWindow(VideoWindow, x, y);
}

Uint8
SDL_GetAppState(void)
{
    Uint8 state = 0;
    Uint32 flags = 0;

    flags = SDL_GetWindowFlags(VideoWindow);
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
    return SDL_GetWindowWMInfo(VideoWindow, info);
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
        SDL_EraseCursor(VideoSurface);
        SDL_cursor->area.x = (x - SDL_cursor->hot_x);
        SDL_cursor->area.y = (y - SDL_cursor->hot_y);
        SDL_DrawCursor(VideoSurface);
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
    clip_diff = (area->x + area->w) - VideoSurface->w;
    if (clip_diff > 0) {
        area->w = area->w < clip_diff ? 0 : area->w - clip_diff;
    }
    clip_diff = (area->y + area->h) - VideoSurface->h;
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
    if (screen->map->dst != VideoSurface) {
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
    info.dst = VideoSurface->format;
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
        if ((screen == VideoSurface) ||
            FORMAT_EQUAL(screen->format, VideoSurface->format)) {
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

    if (display != PublicSurface) {
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
    if (display != VideoSurface) {
        overlay->hwdata->display = display = VideoSurface;
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
    SDL_UpdateWindowSurface(VideoWindow);
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
    SDL_GL_SwapWindow(VideoWindow);
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
    return SDL_SetWindowGammaRamp(VideoWindow, red_ramp, green_ramp, blue_ramp);
}

int
SDL_SetGammaRamp(const Uint16 * red, const Uint16 * green, const Uint16 * blue)
{
    return SDL_SetWindowGammaRamp(VideoWindow, red, green, blue);
}

int
SDL_GetGammaRamp(Uint16 * red, Uint16 * green, Uint16 * blue)
{
    return SDL_GetWindowGammaRamp(VideoWindow, red, green, blue);
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
    int previous = EnabledUnicode;

    switch (enable) {
    case 1:
        EnabledUnicode = 1;
        SDL_StartTextInput();
        break;
    case 0:
        EnabledUnicode = 0;
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
RWops12to20_seek(struct SDL12_RWops *rwops12, int offset, int whence)
{
    return rwops12->rwops20->seek(rwops12->rwops20, offset, whence);
}

static int SDLCALL
RWops12to20_read(struct SDL12_RWops *rwops12, void *ptr, int size, int maxnum)
{
    return rwops12->rwops20->read(rwops12->rwops20, ptr, size, maxnum);
}

static int SDLCALL
RWops12to20_write(struct SDL12_RWops *rwops12, const void *ptr, int size, int num)
{
    return rwops12->rwops20->write(rwops12->rwops20, ptr, size, num);
}

static int SDLCALL
RWops12to20_close(struct SDL12_RWops *rwops12)
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
RWops12to20(SDL12_RWops *rwops12, SDL_RWops *rwops20)
{
    if (!rwops20)
    {
        SDL_FreeRW(rwops12);
        return NULL;
    }
    SDL_zerop(rwops12);
    rwops12->type = rwops20->type;
    rwops12->rwops20 = rwops20;
    rwops12->seek = RWops12to20_seek;
    rwops12->read = RWops12to20_read;
    rwops12->write = RWops12to20_write;
    rwops12->close = RWops12to20_close;
    return rwops12;
}

SDL12_RWops *
SDL_RWFromFile(const char *file, const char *mode)
{
    SDL12_RWops *rwops12 = SDL_AllocRW();
    return rwops12 ? RWops12to20(rwops12, SDL20_RWFromFile(file, mode)) : NULL;
}

SDL12_RWops *
SDL_RWFromFP(FILE *io, int autoclose)
{
    SDL12_RWops *rwops12 = SDL_AllocRW();
    return rwops12 ? RWops12to20(rwops12, SDL20_RWFromFP(io, autoclose)) : NULL;
}

SDL12_RWops *
SDL_RWFromMem(void *mem, int size)
{
    SDL12_RWops *rwops12 = SDL_AllocRW();
    return rwops12 ? RWops12to20(rwops12, SDL20_RWFromMem(mem, size)) : NULL;
}

SDL12_RWops *
SDL_RWFromConstMem(const void *mem, int size)
{
    SDL12_RWops *rwops12 = SDL_AllocRW();
    return rwops12 ? RWops12to20(rwops12, SDL20_RWFromConstMem(mem, size)) : NULL;
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


static Sint64 SDLCALL
RWops20to12_size(struct SDL_RWops *rwops20)
{
    SDL12_RWops *rwops12 = (SDL12_RWops *) rwops20->hidden.unknown.data1;
    int size = rwops20->hidden.unknown.data2;
    int pos;

    if (size != -1)
        return size;

    pos = rwops12->seek(rwops12, 0, SEEK_CUR);
    if (pos == -1)
        return -1;

    size = (Sint64) rwops->seek(rwops12, 0, SEEK_END);
    if (size == -1)
        return -1;

    rwops->seek(rwops12, pos, SEEK_SET);  /* !!! FIXME: and if this fails? */
    rwops20->hidden.unknown.data2 = size;
    return size;
}

static Sint64
RWops20to12_seek(struct SDL_RWops *rwops20, Sint64 offset, int whence)
{
    /* !!! FIXME: fail if (offset) is too big */
    SDL12_RWops *rwops12 = (SDL12_RWops *) rwops20->hidden.unknown.data1;
    return (Sint64) rwops12->seek(rwops12, (int) offset, whence);
}

static size_t SDLCALL
RWops20to12_read(struct SDL_RWops *rwops20, void *ptr, size_t size, size_t maxnum)
{
    /* !!! FIXME: fail if (size) or (maxnum) is too big */
    SDL12_RWops *rwops12 = (SDL12_RWops *) rwops20->hidden.unknown.data1;
    return (size_t) rwops12->read(rwops12, ptr, (int) size, (int) maxnum);
}

static size_t SDLCALL
RWops20to12_write(struct SDL_RWops *rwops20, const void *ptr, size_t size, size_t num)
{
    /* !!! FIXME: fail if (size) or (maxnum) is too big */
    SDL12_RWops *rwops12 = (SDL12_RWops *) rwops20->hidden.unknown.data1;
    return (size_t) rwops12->write(rwops12, ptr, (int) size, (int) num);
}

static int SDLCALL
RWops20to12_close(struct SDL_RWops *rwops20)
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

static SDL12_RWops *
RWops20to12(SDL12_RWops *rwops12)
{
    SDL20_RWops *rwops20;

    if (!rwops12)
        return NULL;

    rwops20 = SDL20_AllocRW();
    if (!rwops20)
        return NULL;

    SDL_zerop(rwops20);
    rwops20->type = rwops12->type;
    rwops20->hidden.unknown.data1 = rwops12;
    rwops20->hidden.unknown.data2 = -1;  /* cached size of stream */
    rwops20->size = RWops20to12_size;
    rwops20->seek = RWops20to12_seek;
    rwops20->read = RWops20to12_read;
    rwops20->write = RWops20to12_write;
    rwops20->close = RWops20to12_close;
    return rwops20;
}

SDL12_Surface *
SDL_LoadBMP_RW(SDL12_RWops *rwops12, int freerwops12)
{
    SDL_RWops *rwops20 = RWops20to12(rwops12);
    SDL_Surface *retval = SDL20_LoadBMP_RW(rwops20, freerwops12);
    if (!freerwops12)  /* free our wrapper if SDL2 didn't close it. */
        SDL20_FreeRW(rwops20);
    // !!! FIXME: wrap surface.
    return retval;
}

int
SDL_SaveBMP_RW(SDL12_Surface *surface, SDL12_RWops *rwops12, int freerwops12)
{
    // !!! FIXME: wrap surface.
    SDL_RWops *rwops20 = RWops20to12(rwops12);
    const int retval = SDL20_SaveBMP_RW(surface, rwops20, freerwops12);
    if (!freerwops12)  /* free our wrapper if SDL2 didn't close it. */
        SDL20_FreeRW(rwops20);
    return retval;
}

SDL_AudioSpec *
SDL_LoadWAV_RW(SDL12_RWops *rwops12, int freerwops12,
               SDL_AudioSpec *spec, Uint8 **buf, Uint32 *len)
{
    SDL_RWops *rwops20 = RWops20to12(rwops12);
    SDL_AudioSpec *retval = SDL20_LoadWAV_RW(rwops20, freerwops12, spec, buf, len);
    if (!freerwops12)  /* free our wrapper if SDL2 didn't close it. */
        SDL20_FreeRW(rwops20);
    return retval;
}

/* vi: set ts=4 sw=4 expandtab: */
