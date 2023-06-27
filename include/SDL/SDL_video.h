/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2022 Sam Lantinga <slouken@libsdl.org>

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

/* These headers are from sdl12-compat, and are intended to give just enough
functionality to let you build an SDL-1.2-based project without having the
real SDL-1.2 available to you. */

#ifndef _SDL_video_h
#define _SDL_video_h

#include "SDL_stdinc.h"
#include "SDL_error.h"
#include "SDL_rwops.h"

#include "begin_code.h"

#define SDL_ALPHA_OPAQUE 255
#define SDL_ALPHA_TRANSPARENT 0

typedef struct SDL_Rect
{
    Sint16 x;
    Sint16 y;
    Uint16 w;
    Uint16 h;
} SDL_Rect;

typedef struct SDL_Color
{
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 unused;
} SDL_Color;

#define SDL_Colour  SDL_Color

typedef struct SDL_Palette
{
    int ncolors;
    SDL_Color *colors;
} SDL_Palette;

typedef struct SDL_PixelFormat
{
    SDL_Palette *palette;
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
} SDL_PixelFormat;

typedef struct SDL_Surface
{
    Uint32 flags;
    SDL_PixelFormat *format;
    int w;
    int h;
    Uint16 pitch;
    void *pixels;
    int offset;
    struct private_hwdata *hwdata;
    SDL_Rect clip_rect;
    Uint32 unused1;
    Uint32 locked;
    void *map;
    unsigned int format_version;
    int refcount;
} SDL_Surface;

typedef int (*SDL_blit)(struct SDL_Surface *src, SDL_Rect *srcrect, struct SDL_Surface *dst, SDL_Rect *dstrect);

typedef struct SDL_VideoInfo
{
    Uint32 hw_available:1;
    Uint32 wm_available:1;
    Uint32 UnusedBits1:6;
    Uint32 UnusedBits2:1;
    Uint32 blit_hw:1;
    Uint32 blit_hw_CC:1;
    Uint32 blit_hw_A:1;
    Uint32 blit_sw:1;
    Uint32 blit_sw_CC:1;
    Uint32 blit_sw_A:1;
    Uint32 blit_fill:1;
    Uint32 UnusedBits3:16;
    Uint32 video_mem;
    SDL_PixelFormat *vfmt;
    int current_w;
    int current_h;
} SDL_VideoInfo;

typedef struct SDL_Overlay
{
    Uint32 format;
    int w;
    int h;
    int planes;
    Uint16 *pitches;
    Uint8 **pixels;
    void *hwfuncs;
    void *hwdata;
    Uint32 hw_overlay:1;
    Uint32 UnusedBits:31;
} SDL_Overlay;


typedef enum SDL_GLattr
{
    SDL_GL_RED_SIZE,
    SDL_GL_GREEN_SIZE,
    SDL_GL_BLUE_SIZE,
    SDL_GL_ALPHA_SIZE,
    SDL_GL_BUFFER_SIZE,
    SDL_GL_DOUBLEBUFFER,
    SDL_GL_DEPTH_SIZE,
    SDL_GL_STENCIL_SIZE,
    SDL_GL_ACCUM_RED_SIZE,
    SDL_GL_ACCUM_GREEN_SIZE,
    SDL_GL_ACCUM_BLUE_SIZE,
    SDL_GL_ACCUM_ALPHA_SIZE,
    SDL_GL_STEREO,
    SDL_GL_MULTISAMPLEBUFFERS,
    SDL_GL_MULTISAMPLESAMPLES,
    SDL_GL_ACCELERATED_VISUAL,
    SDL_GL_SWAP_CONTROL
} SDL_GLattr;

typedef enum SDL_GrabMode
{
    SDL_GRAB_QUERY = -1,
    SDL_GRAB_OFF = 0,
    SDL_GRAB_ON = 1,
    SDL_GRAB_FULLSCREEN
} SDL_GrabMode;

extern DECLSPEC int SDLCALL SDL_VideoInit(const char *driver_name, Uint32 flags);
extern DECLSPEC void SDLCALL SDL_VideoQuit(void);
extern DECLSPEC char * SDLCALL SDL_VideoDriverName(char *namebuf, int maxlen);
extern DECLSPEC SDL_Surface * SDLCALL SDL_GetVideoSurface(void);
extern DECLSPEC const SDL_VideoInfo * SDLCALL SDL_GetVideoInfo(void);
extern DECLSPEC int SDLCALL SDL_VideoModeOK(int width, int height, int bpp, Uint32 flags);
extern DECLSPEC SDL_Rect ** SDLCALL SDL_ListModes(SDL_PixelFormat *format, Uint32 flags);
extern DECLSPEC SDL_Surface * SDLCALL SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags);
extern DECLSPEC void SDLCALL SDL_UpdateRects(SDL_Surface *screen, int numrects, SDL_Rect *rects);
extern DECLSPEC void SDLCALL SDL_UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h);
extern DECLSPEC int SDLCALL SDL_Flip(SDL_Surface *screen);
extern DECLSPEC int SDLCALL SDL_SetGamma(float red, float green, float blue);
extern DECLSPEC int SDLCALL SDL_SetGammaRamp(const Uint16 *red, const Uint16 *green, const Uint16 *blue);
extern DECLSPEC int SDLCALL SDL_GetGammaRamp(Uint16 *red, Uint16 *green, Uint16 *blue);
extern DECLSPEC int SDLCALL SDL_SetColors(SDL_Surface *surface, SDL_Color *colors, int firstcolor, int ncolors);
extern DECLSPEC int SDLCALL SDL_SetPalette(SDL_Surface *surface, int flags, SDL_Color *colors, int firstcolor, int ncolors);
extern DECLSPEC Uint32 SDLCALL SDL_MapRGB(const SDL_PixelFormat * const format, const Uint8 r, const Uint8 g, const Uint8 b);
extern DECLSPEC Uint32 SDLCALL SDL_MapRGBA(const SDL_PixelFormat * const format, const Uint8 r, const Uint8 g, const Uint8 b, const Uint8 a);
extern DECLSPEC void SDLCALL SDL_GetRGB(Uint32 pixel, const SDL_PixelFormat * const fmt, Uint8 *r, Uint8 *g, Uint8 *b);
extern DECLSPEC void SDLCALL SDL_GetRGBA(Uint32 pixel, const SDL_PixelFormat * const fmt, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a);
extern DECLSPEC SDL_Surface * SDLCALL SDL_CreateRGBSurface(Uint32 flags, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
extern DECLSPEC SDL_Surface * SDLCALL SDL_CreateRGBSurfaceFrom(void *pixels, int width, int height, int depth, int pitch, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
extern DECLSPEC void SDLCALL SDL_FreeSurface(SDL_Surface *surface);
extern DECLSPEC int SDLCALL SDL_LockSurface(SDL_Surface *surface);
extern DECLSPEC void SDLCALL SDL_UnlockSurface(SDL_Surface *surface);
extern DECLSPEC SDL_Surface * SDLCALL SDL_LoadBMP_RW(SDL_RWops *src, int freesrc);
extern DECLSPEC int SDLCALL SDL_SaveBMP_RW(SDL_Surface *surface, SDL_RWops *dst, int freedst);
extern DECLSPEC int SDLCALL SDL_SetColorKey(SDL_Surface *surface, Uint32 flag, Uint32 key);
extern DECLSPEC int SDLCALL SDL_SetAlpha(SDL_Surface *surface, Uint32 flag, Uint8 alpha);
extern DECLSPEC SDL_bool SDLCALL SDL_SetClipRect(SDL_Surface *surface, const SDL_Rect *rect);
extern DECLSPEC void SDLCALL SDL_GetClipRect(SDL_Surface *surface, SDL_Rect *rect);
extern DECLSPEC SDL_Surface * SDLCALL SDL_ConvertSurface(SDL_Surface *src, SDL_PixelFormat *fmt, Uint32 flags);
extern DECLSPEC int SDLCALL SDL_UpperBlit(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect);
extern DECLSPEC int SDLCALL SDL_LowerBlit(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect);
extern DECLSPEC int SDLCALL SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color);
extern DECLSPEC SDL_Surface * SDLCALL SDL_DisplayFormat(SDL_Surface *surface);
extern DECLSPEC SDL_Surface * SDLCALL SDL_DisplayFormatAlpha(SDL_Surface *surface);
extern DECLSPEC SDL_Overlay * SDLCALL SDL_CreateYUVOverlay(int width, int height, Uint32 format, SDL_Surface *display);
extern DECLSPEC int SDLCALL SDL_LockYUVOverlay(SDL_Overlay *overlay);
extern DECLSPEC void SDLCALL SDL_UnlockYUVOverlay(SDL_Overlay *overlay);
extern DECLSPEC int SDLCALL SDL_DisplayYUVOverlay(SDL_Overlay *overlay, SDL_Rect *dstrect);
extern DECLSPEC void SDLCALL SDL_FreeYUVOverlay(SDL_Overlay *overlay);
extern DECLSPEC int SDLCALL SDL_GL_LoadLibrary(const char *path);
extern DECLSPEC void * SDLCALL SDL_GL_GetProcAddress(const char* proc);
extern DECLSPEC int SDLCALL SDL_GL_SetAttribute(SDL_GLattr attr, int value);
extern DECLSPEC int SDLCALL SDL_GL_GetAttribute(SDL_GLattr attr, int* value);
extern DECLSPEC void SDLCALL SDL_GL_SwapBuffers(void);
extern DECLSPEC void SDLCALL SDL_GL_UpdateRects(int numrects, SDL_Rect* rects);
extern DECLSPEC void SDLCALL SDL_GL_Lock(void);
extern DECLSPEC void SDLCALL SDL_GL_Unlock(void);
extern DECLSPEC void SDLCALL SDL_WM_SetCaption(const char *title, const char *icon);
extern DECLSPEC void SDLCALL SDL_WM_GetCaption(char **title, char **icon);
extern DECLSPEC void SDLCALL SDL_WM_SetIcon(SDL_Surface *icon, Uint8 *mask);
extern DECLSPEC int SDLCALL SDL_WM_IconifyWindow(void);
extern DECLSPEC int SDLCALL SDL_WM_ToggleFullScreen(SDL_Surface *surface);
extern DECLSPEC SDL_GrabMode SDLCALL SDL_WM_GrabInput(SDL_GrabMode mode);
extern DECLSPEC int SDLCALL SDL_SoftStretch(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect);

#define SDL_SWSURFACE 0x00000000
#define SDL_HWSURFACE 0x00000001
#define SDL_ASYNCBLIT 0x00000004

#define SDL_ANYFORMAT 0x10000000
#define SDL_HWPALETTE 0x20000000
#define SDL_DOUBLEBUF 0x40000000
#define SDL_TRIPLEBUF 0x40000100
#define SDL_FULLSCREEN 0x80000000
#define SDL_OPENGL 0x00000002
#define SDL_OPENGLBLIT 0x0000000A
#define SDL_RESIZABLE 0x00000010
#define SDL_NOFRAME 0x00000020

#define SDL_HWACCEL 0x00000100
#define SDL_SRCCOLORKEY 0x00001000
#define SDL_RLEACCELOK 0x00002000
#define SDL_RLEACCEL 0x00004000
#define SDL_SRCALPHA 0x00010000
#define SDL_PREALLOC 0x01000000

#define SDL_MUSTLOCK(surface) (surface->offset || ((surface->flags & (SDL_HWSURFACE|SDL_ASYNCBLIT|SDL_RLEACCEL)) != 0))

#define SDL_YV12_OVERLAY 0x32315659
#define SDL_IYUV_OVERLAY 0x56555949
#define SDL_YUY2_OVERLAY 0x32595559
#define SDL_UYVY_OVERLAY 0x59565955
#define SDL_YVYU_OVERLAY 0x55595659

#define SDL_LOGPAL 0x01
#define SDL_PHYSPAL 0x02

#define SDL_AllocSurface SDL_CreateRGBSurface
#define SDL_BlitSurface SDL_UpperBlit

#define SDL_LoadBMP(file) SDL_LoadBMP_RW(SDL_RWFromFile(file, "rb"), 1)
#define SDL_SaveBMP(surface, file) SDL_SaveBMP_RW(surface, SDL_RWFromFile(file, "wb"), 1)

#include "close_code.h"

#endif

