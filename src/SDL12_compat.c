/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2021 Sam Lantinga <slouken@libsdl.org>

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

#include "SDL20_include_wrapper.h"

/*
 * We report the library version as 1.2.$(SDL12_COMPAT_VERSION). This number
 * should be way ahead of what SDL-1.2 Classic would report, so apps can
 * decide if they're running under the compat layer, if they really care.
 */
#define SDL12_COMPAT_VERSION 61

#include <stdarg.h>
#include <limits.h>
#include <stddef.h>
#if defined(_MSC_VER) && (_MSC_VER < 1600)
/* intptr_t already handled by stddef.h. */
#else
#include <stdint.h>
#endif

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#include <windows.h>
#else
#include <stdio.h> /* fprintf(), etc. */
#include <stdlib.h>    /* for abort() */
#include <string.h>
#endif

/* mingw headers may define these ... */
#undef strtod
#undef strcasecmp
#undef strncasecmp
#undef snprintf
#undef vsnprintf

#define SDL_BlitSurface SDL_UpperBlit

#ifdef __linux__
#include <unistd.h> /* for readlink() */
#endif

#if defined(__unix__) || defined(__APPLE__)
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif
#define SDL12_MAXPATH PATH_MAX
#elif defined _WIN32
#define SDL12_MAXPATH MAX_PATH
#elif defined __OS2__
#define SDL12_MAXPATH CCHMAXPATH
#else
#define SDL12_MAXPATH 1024
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* on x86 Linux builds, we have the public entry points force stack alignment to 16 bytes
   on entry. This won't be a massive performance hit, but it might help extremely old
   binaries that want to call into SDL to not crash in hard-to-diagnose ways. It's not a
   panacea to the stack alignment problem, but it might help a little.

   The force_align_arg_pointer attribute requires gcc >= 4.2.x. */
#if defined(__clang__)
#define HAVE_FORCE_ALIGN_ARG_POINTER
#elif defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 2))
#define HAVE_FORCE_ALIGN_ARG_POINTER
#endif
#if defined(__linux__) && defined(__i386__) && defined(HAVE_FORCE_ALIGN_ARG_POINTER)
#define FORCEALIGNATTR __attribute__((force_align_arg_pointer))
#else
#define FORCEALIGNATTR
#endif

#define DECLSPEC12 DECLSPEC FORCEALIGNATTR

#define SDL20_SYM(rc,fn,params,args,ret) \
    typedef rc (SDLCALL *SDL20_##fn##_t) params; \
    static SDL20_##fn##_t SDL20_##fn = NULL;
#include "SDL20_syms.h"

/* Things that _should_ be binary compatible pass right through... */
#define SDL20_SYM_PASSTHROUGH(rc,fn,params,args,ret) \
    DECLSPEC12 rc SDLCALL SDL_##fn params { ret SDL20_##fn args; }
#include "SDL20_syms.h"


/* these are macros (etc) in the SDL headers, so make our own. */
#define SDL20_OutOfMemory() SDL20_Error(SDL_ENOMEM)
#define SDL20_Unsupported() SDL20_Error(SDL_UNSUPPORTED)
#define SDL20_InvalidParamError(param) SDL20_SetError("Parameter '%s' is invalid", (param))
#define SDL20_zero(x) SDL20_memset(&(x), 0, sizeof((x)))
#define SDL20_zerop(x) SDL20_memset((x), 0, sizeof(*(x)))
#define SDL_ReportAssertion SDL20_ReportAssertion

/* From SDL2.0's SDL_bits.h: a force-inlined function. */
#if defined(__WATCOMC__) && defined(__386__)
extern __inline int _SDL20_bsr_watcom(Uint32);
#pragma aux _SDL20_bsr_watcom = \
    "bsr eax, eax" \
    parm [eax] nomemory \
    value [eax] \
    modify exact [eax] nomemory;
#endif

SDL_FORCE_INLINE int
SDL20_MostSignificantBitIndex32(Uint32 x)
{
#if defined(__GNUC__) && (__GNUC__ >= 4 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4))
    /* Count Leading Zeroes builtin in GCC.
     * http://gcc.gnu.org/onlinedocs/gcc-4.3.4/gcc/Other-Builtins.html
     */
    if (x == 0) {
        return -1;
    }
    return 31 - __builtin_clz(x);
#elif defined(__WATCOMC__) && defined(__386__)
    if (x == 0) {
        return -1;
    }
    return _SDL20_bsr_watcom(x);
#elif defined(_MSC_VER)
    unsigned long index;
    if (_BitScanReverse(&index, x)) {
        return index;
    }
    return -1;
#else
    /* Based off of Bit Twiddling Hacks by Sean Eron Anderson
     * <seander@cs.stanford.edu>, released in the public domain.
     * http://graphics.stanford.edu/~seander/bithacks.html#IntegerLog
     */
    const Uint32 b[] = {0x2, 0xC, 0xF0, 0xFF00, 0xFFFF0000};
    const int    S[] = {1, 2, 4, 8, 16};

    int msbIndex = 0;
    int i;

    if (x == 0) {
        return -1;
    }

    for (i = 4; i >= 0; i--)
    {
        if (x & b[i])
        {
            x >>= S[i];
            msbIndex |= S[i];
        }
    }

    return msbIndex;
#endif
}

/* SDL_truncf needs SDL >=2.0.14, so copy it here. */
static float SDL20_truncf(float x)
{
    return (x < 0.0f) ? (float)SDL20_ceil(x) : (float)SDL20_floor(x);
}

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

#define SDL12_LOGPAL 1
#define SDL12_PHYSPAL 2

#ifndef SDL_SIMD_ALIGNED
#define SDL_SIMD_ALIGNED 0x00000008
#endif

typedef struct SDL12_Rect
{
    Sint16 x;
    Sint16 y;
    Uint16 w;
    Uint16 h;
} SDL12_Rect;

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
    SDL12_Rect clip_rect;
    Uint32 unused1;
    Uint32 locked;
    void *blitmap;
    unsigned int format_version;
    int refcount;
} SDL12_Surface;

#define SDL12_YV12_OVERLAY 0x32315659
#define SDL12_IYUV_OVERLAY 0x56555949
#define SDL12_YUY2_OVERLAY 0x32595559
#define SDL12_UYVY_OVERLAY 0x59565955
#define SDL12_YVYU_OVERLAY 0x55595659

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

typedef struct SDL12_YUVData  /* internal struct, not part of public SDL 1.2 API */
{
    SDL_Texture *texture20;
    SDL_bool dirty;
    Uint8 *pixelbuf;
    Uint8 *pixels[3];
    Uint16 pitches[3];
} SDL12_YUVData;

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
    SDL12_PixelFormat *vfmt;
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
    SDLK12_UNKNOWN = 0,
    SDLK12_FIRST = 0,
    SDLK12_BACKSPACE = 8,
    SDLK12_TAB = 9,
    SDLK12_CLEAR = 12,
    SDLK12_RETURN = 13,
    SDLK12_PAUSE = 19,
    SDLK12_ESCAPE = 27,
    SDLK12_SPACE = 32,
    SDLK12_EXCLAIM = 33,
    SDLK12_QUOTEDBL = 34,
    SDLK12_HASH = 35,
    SDLK12_DOLLAR = 36,
    SDLK12_AMPERSAND = 38,
    SDLK12_QUOTE = 39,
    SDLK12_LEFTPAREN = 40,
    SDLK12_RIGHTPAREN = 41,
    SDLK12_ASTERISK = 42,
    SDLK12_PLUS = 43,
    SDLK12_COMMA = 44,
    SDLK12_MINUS = 45,
    SDLK12_PERIOD = 46,
    SDLK12_SLASH = 47,
    SDLK12_0 = 48,
    SDLK12_1 = 49,
    SDLK12_2 = 50,
    SDLK12_3 = 51,
    SDLK12_4 = 52,
    SDLK12_5 = 53,
    SDLK12_6 = 54,
    SDLK12_7 = 55,
    SDLK12_8 = 56,
    SDLK12_9 = 57,
    SDLK12_COLON = 58,
    SDLK12_SEMICOLON = 59,
    SDLK12_LESS = 60,
    SDLK12_EQUALS = 61,
    SDLK12_GREATER = 62,
    SDLK12_QUESTION = 63,
    SDLK12_AT = 64,
    SDLK12_LEFTBRACKET = 91,
    SDLK12_BACKSLASH = 92,
    SDLK12_RIGHTBRACKET = 93,
    SDLK12_CARET = 94,
    SDLK12_UNDERSCORE = 95,
    SDLK12_BACKQUOTE = 96,
    SDLK12_a = 97,
    SDLK12_b = 98,
    SDLK12_c = 99,
    SDLK12_d = 100,
    SDLK12_e = 101,
    SDLK12_f = 102,
    SDLK12_g = 103,
    SDLK12_h = 104,
    SDLK12_i = 105,
    SDLK12_j = 106,
    SDLK12_k = 107,
    SDLK12_l = 108,
    SDLK12_m = 109,
    SDLK12_n = 110,
    SDLK12_o = 111,
    SDLK12_p = 112,
    SDLK12_q = 113,
    SDLK12_r = 114,
    SDLK12_s = 115,
    SDLK12_t = 116,
    SDLK12_u = 117,
    SDLK12_v = 118,
    SDLK12_w = 119,
    SDLK12_x = 120,
    SDLK12_y = 121,
    SDLK12_z = 122,
    SDLK12_DELETE = 127,
    SDLK12_WORLD_0 = 160,
    SDLK12_WORLD_1 = 161,
    SDLK12_WORLD_2 = 162,
    SDLK12_WORLD_3 = 163,
    SDLK12_WORLD_4 = 164,
    SDLK12_WORLD_5 = 165,
    SDLK12_WORLD_6 = 166,
    SDLK12_WORLD_7 = 167,
    SDLK12_WORLD_8 = 168,
    SDLK12_WORLD_9 = 169,
    SDLK12_WORLD_10 = 170,
    SDLK12_WORLD_11 = 171,
    SDLK12_WORLD_12 = 172,
    SDLK12_WORLD_13 = 173,
    SDLK12_WORLD_14 = 174,
    SDLK12_WORLD_15 = 175,
    SDLK12_WORLD_16 = 176,
    SDLK12_WORLD_17 = 177,
    SDLK12_WORLD_18 = 178,
    SDLK12_WORLD_19 = 179,
    SDLK12_WORLD_20 = 180,
    SDLK12_WORLD_21 = 181,
    SDLK12_WORLD_22 = 182,
    SDLK12_WORLD_23 = 183,
    SDLK12_WORLD_24 = 184,
    SDLK12_WORLD_25 = 185,
    SDLK12_WORLD_26 = 186,
    SDLK12_WORLD_27 = 187,
    SDLK12_WORLD_28 = 188,
    SDLK12_WORLD_29 = 189,
    SDLK12_WORLD_30 = 190,
    SDLK12_WORLD_31 = 191,
    SDLK12_WORLD_32 = 192,
    SDLK12_WORLD_33 = 193,
    SDLK12_WORLD_34 = 194,
    SDLK12_WORLD_35 = 195,
    SDLK12_WORLD_36 = 196,
    SDLK12_WORLD_37 = 197,
    SDLK12_WORLD_38 = 198,
    SDLK12_WORLD_39 = 199,
    SDLK12_WORLD_40 = 200,
    SDLK12_WORLD_41 = 201,
    SDLK12_WORLD_42 = 202,
    SDLK12_WORLD_43 = 203,
    SDLK12_WORLD_44 = 204,
    SDLK12_WORLD_45 = 205,
    SDLK12_WORLD_46 = 206,
    SDLK12_WORLD_47 = 207,
    SDLK12_WORLD_48 = 208,
    SDLK12_WORLD_49 = 209,
    SDLK12_WORLD_50 = 210,
    SDLK12_WORLD_51 = 211,
    SDLK12_WORLD_52 = 212,
    SDLK12_WORLD_53 = 213,
    SDLK12_WORLD_54 = 214,
    SDLK12_WORLD_55 = 215,
    SDLK12_WORLD_56 = 216,
    SDLK12_WORLD_57 = 217,
    SDLK12_WORLD_58 = 218,
    SDLK12_WORLD_59 = 219,
    SDLK12_WORLD_60 = 220,
    SDLK12_WORLD_61 = 221,
    SDLK12_WORLD_62 = 222,
    SDLK12_WORLD_63 = 223,
    SDLK12_WORLD_64 = 224,
    SDLK12_WORLD_65 = 225,
    SDLK12_WORLD_66 = 226,
    SDLK12_WORLD_67 = 227,
    SDLK12_WORLD_68 = 228,
    SDLK12_WORLD_69 = 229,
    SDLK12_WORLD_70 = 230,
    SDLK12_WORLD_71 = 231,
    SDLK12_WORLD_72 = 232,
    SDLK12_WORLD_73 = 233,
    SDLK12_WORLD_74 = 234,
    SDLK12_WORLD_75 = 235,
    SDLK12_WORLD_76 = 236,
    SDLK12_WORLD_77 = 237,
    SDLK12_WORLD_78 = 238,
    SDLK12_WORLD_79 = 239,
    SDLK12_WORLD_80 = 240,
    SDLK12_WORLD_81 = 241,
    SDLK12_WORLD_82 = 242,
    SDLK12_WORLD_83 = 243,
    SDLK12_WORLD_84 = 244,
    SDLK12_WORLD_85 = 245,
    SDLK12_WORLD_86 = 246,
    SDLK12_WORLD_87 = 247,
    SDLK12_WORLD_88 = 248,
    SDLK12_WORLD_89 = 249,
    SDLK12_WORLD_90 = 250,
    SDLK12_WORLD_91 = 251,
    SDLK12_WORLD_92 = 252,
    SDLK12_WORLD_93 = 253,
    SDLK12_WORLD_94 = 254,
    SDLK12_WORLD_95 = 255,
    SDLK12_KP0 = 256,
    SDLK12_KP1 = 257,
    SDLK12_KP2 = 258,
    SDLK12_KP3 = 259,
    SDLK12_KP4 = 260,
    SDLK12_KP5 = 261,
    SDLK12_KP6 = 262,
    SDLK12_KP7 = 263,
    SDLK12_KP8 = 264,
    SDLK12_KP9 = 265,
    SDLK12_KP_PERIOD = 266,
    SDLK12_KP_DIVIDE = 267,
    SDLK12_KP_MULTIPLY = 268,
    SDLK12_KP_MINUS = 269,
    SDLK12_KP_PLUS = 270,
    SDLK12_KP_ENTER = 271,
    SDLK12_KP_EQUALS = 272,
    SDLK12_UP = 273,
    SDLK12_DOWN = 274,
    SDLK12_RIGHT = 275,
    SDLK12_LEFT = 276,
    SDLK12_INSERT = 277,
    SDLK12_HOME = 278,
    SDLK12_END = 279,
    SDLK12_PAGEUP = 280,
    SDLK12_PAGEDOWN = 281,
    SDLK12_F1 = 282,
    SDLK12_F2 = 283,
    SDLK12_F3 = 284,
    SDLK12_F4 = 285,
    SDLK12_F5 = 286,
    SDLK12_F6 = 287,
    SDLK12_F7 = 288,
    SDLK12_F8 = 289,
    SDLK12_F9 = 290,
    SDLK12_F10 = 291,
    SDLK12_F11 = 292,
    SDLK12_F12 = 293,
    SDLK12_F13 = 294,
    SDLK12_F14 = 295,
    SDLK12_F15 = 296,
    SDLK12_NUMLOCK = 300,
    SDLK12_CAPSLOCK = 301,
    SDLK12_SCROLLOCK = 302,
    SDLK12_RSHIFT = 303,
    SDLK12_LSHIFT = 304,
    SDLK12_RCTRL = 305,
    SDLK12_LCTRL = 306,
    SDLK12_RALT = 307,
    SDLK12_LALT = 308,
    SDLK12_RMETA = 309,
    SDLK12_LMETA = 310,
    SDLK12_LSUPER = 311,
    SDLK12_RSUPER = 312,
    SDLK12_MODE = 313,
    SDLK12_COMPOSE = 314,
    SDLK12_HELP = 315,
    SDLK12_PRINT = 316,
    SDLK12_SYSREQ = 317,
    SDLK12_BREAK = 318,
    SDLK12_MENU = 319,
    SDLK12_POWER = 320,
    SDLK12_EURO = 321,
    SDLK12_UNDO = 322,
    SDLK12_LAST
} SDL12Key;

typedef enum
{
    KMOD12_NONE  = 0x0000,
    KMOD12_LSHIFT = 0x0001,
    KMOD12_RSHIFT = 0x0002,
    KMOD12_LCTRL = 0x0040,
    KMOD12_RCTRL = 0x0080,
    KMOD12_LALT = 0x0100,
    KMOD12_RALT = 0x0200,
    KMOD12_LMETA = 0x0400,
    KMOD12_RMETA = 0x0800,
    KMOD12_NUM = 0x1000,
    KMOD12_CAPS = 0x2000,
    KMOD12_MODE = 0x4000,
    KMOD12_RESERVED = 0x8000
} SDL12Mod;

typedef struct SDL12_keysym
{
    Uint8 scancode;
    SDL12Key sym;
    unsigned int mod; /* SDL12Mod */
    Uint16 unicode;
} SDL12_keysym;

#define SDL12_RELEASED 0
#define SDL12_PRESSED  1

#if defined(SDL_VIDEO_DRIVER_X11)  /* SDL_VIDEO_DRIVER_X11 refers to the SDL2 headers. */
typedef enum  /* this is only used for 1.2 X11 syswm */
{
    SDL12_SYSWM_X11
} SDL12_SYSWM_TYPE;
#endif

typedef struct SDL12_SysWMmsg
{
    SDL_version version;
#if defined(_WIN32)
    HWND hwnd;
    UINT msg;
    WPARAM wParam;
    LPARAM lParam;
#elif defined(SDL_VIDEO_DRIVER_X11)
    SDL12_SYSWM_TYPE subsystem;
    union { XEvent xevent; } event;
#else
    int data;  /* unused at the moment. */
#endif
} SDL12_SysWMmsg;

typedef struct SDL12_SysWMinfo
{
    SDL_version version;
#if defined(_WIN32)
    HWND window;
    HGLRC hglrc;
#elif defined(SDL_VIDEO_DRIVER_X11)
    SDL12_SYSWM_TYPE subsystem;
    union {
        struct {
            Display *display;
            Window window;
            void (*lock_func)(void);
            void (*unlock_func)(void);
            Window fswindow;
            Window wmwindow;
            Display *gfxdisplay;
        } x11;
    } info;
#else
    int data;  /* unused at the moment. */
#endif
} SDL12_SysWMinfo;

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
    SDL12_keysym keysym;
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
    SDL12_SysWMmsg *msg;
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
static int SDLCALL EventFilter20to12(void *data, SDL_Event *event20);

typedef Uint32 (SDLCALL *SDL12_TimerCallback)(Uint32 interval);
typedef SDL_TimerCallback SDL12_NewTimerCallback;

typedef struct
{
    SDL12_Rect area;
    Sint16 hot_x;
    Sint16 hot_y;
    Uint8 *data;
    Uint8 *mask;
    Uint8 *save[2];
    SDL_Cursor *wm_cursor;  /* the real SDL 1.2 has an opaque pointer to a platform-specific cursor here. */
} SDL12_Cursor;

typedef enum
{
    SDL12_GRAB_QUERY = -1,
    SDL12_GRAB_OFF = 0,
    SDL12_GRAB_ON = 1
} SDL12_GrabMode;

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


typedef enum
{
    SDL12_CD_TRAYEMPTY,
    SDL12_CD_STOPPED,
    SDL12_CD_PLAYING,
    SDL12_CD_PAUSED,
    SDL12_CD_ERROR = -1
} SDL12_CDstatus;

typedef struct
{
    Uint8 id;
    Uint8 type;
    Uint16 unused;
    Uint32 length;
    Uint32 offset;
} SDL12_CDtrack;

typedef struct
{
    int id;
    SDL12_CDstatus status;
    int numtracks;
    int cur_track;
    int cur_frame;
    SDL12_CDtrack track[100];  /* in 1.2, this was SDL_MAX_TRACKS+1 */
} SDL12_CD;

typedef struct
{
    Uint32 format;
    int nummodes;
    SDL12_Rect *modeslist12;
    SDL12_Rect **modes12;  /* ptrs to each item in modeslist, for SDL_ListModes() */
} VideoModeList;

typedef struct
{
    char *name;
    SDL_atomic_t refcount;
    SDL_JoystickID instance_id;
    union {
        SDL_Joystick *joystick;
        SDL_GameController *controller;
    } dev;
} SDL12_Joystick;


struct SDL12_AudioCVT;
typedef void (SDLCALL *SDL12_AudioCVTFilter)(struct SDL12_AudioCVT *cvt, Uint16 format);

/* this is identical to SDL2, except SDL2 forced the structure packing in
   some instances, so we can't passthrough without converting the struct. :( */
typedef struct SDL12_AudioCVT
{
    int needed;
    Uint16 src_format;
    Uint16 dst_format;
    double rate_incr;
    Uint8 *buf;
    int len;
    int len_cvt;
    int len_mult;
    double len_ratio;
    SDL12_AudioCVTFilter filters[10];
    int filter_index;
} SDL12_AudioCVT;

#include "SDL_opengl.h"
#include "SDL_opengl_glext.h"


#define OPENGL_SYM(ext,rc,fn,params,args,ret) typedef rc (GLAPIENTRY *openglfn_##fn##_t) params;
#include "SDL20_syms.h"

typedef struct OpenGLEntryPoints
{
    SDL_bool SUPPORTS_Core;

    #define OPENGL_EXT(name) SDL_bool SUPPORTS_##name;
    #include "SDL20_syms.h"
    #define OPENGL_SYM(ext,rc,fn,params,args,ret) openglfn_##fn##_t fn;
    #include "SDL20_syms.h"
} OpenGLEntryPoints;

typedef struct QueuedOverlayItem
{
    SDL12_Overlay *overlay12;
    SDL12_Rect dstrect12;
    struct QueuedOverlayItem *next;
} QueuedOverlayItem;

typedef struct SDL12_TimerID_Data
{
    SDL_TimerID timer_id;
    SDL12_NewTimerCallback callback;
    void *param;
    struct SDL12_TimerID_Data *next;
    struct SDL12_TimerID_Data *prev;
} SDL12_TimerID_Data;

/* This changed from an opaque pointer to an int in 2.0. */
typedef SDL12_TimerID_Data *SDL12_TimerID;

#define SDL12_MAXEVENTS 128
typedef struct EventQueueType
{
    SDL12_SysWMmsg syswm_msg;  /* save space for a copy of this in case we use it. */
    SDL12_Event event12;
    struct EventQueueType *next;
} EventQueueType;


static Uint32 InitializedSubsystems20 = 0;
static Uint32 LinkedSDL2VersionInt = 0;
static SDL_bool IsDummyVideo = SDL_FALSE;
static VideoModeList *VideoModes = NULL;
static int VideoModesCount = 0;  /* this counts items in VideoModeList, not total video modes. */
static SDL12_VideoInfo VideoInfo12;
static SDL12_Palette VideoInfoPalette12;
static SDL12_PixelFormat VideoInfoVfmt12;
static SDL_PixelFormat *VideoInfoVfmt20 = NULL;
static SDL_bool VideoWindowGrabbed = SDL_FALSE;
static SDL_bool VideoCursorHidden = SDL_FALSE;
static SDL_bool SetVideoModeInProgress = SDL_FALSE;
static SDL_Window *VideoWindow20 = NULL;
static SDL_Renderer *VideoRenderer20 = NULL;
static SDL_mutex *VideoRendererLock = NULL;
static SDL_Texture *VideoTexture20 = NULL;
static SDL12_Surface *VideoSurface12 = NULL;
static SDL_Palette *VideoPhysicalPalette20 = NULL;
static Uint32 VideoSurfacePresentTicks = 0;
static Uint32 VideoSurfaceLastPresentTicks = 0;
static SDL_Surface *VideoConvertSurface20 = NULL;
static SDL_GLContext VideoGLContext20 = NULL;
static QueuedOverlayItem QueuedDisplayOverlays;  /* the head node */
static QueuedOverlayItem *QueuedDisplayOverlaysTail = &QueuedDisplayOverlays;
static char *WindowTitle = NULL;
static char *WindowIconTitle = NULL;
static SDL_Surface *VideoIcon20 = NULL;
static int EnabledUnicode = 0;
static Uint32 KeyRepeatNextTicks = 0;
static Uint32 KeyRepeatDelay = 0;
static Uint32 KeyRepeatInterval = 0;
static SDL12_Event KeyRepeatEvent;
/* Windows SDL1.2 never uses translated keyboard layouts for compatibility with
DirectInput, which didn't support them. Other platforms (MacOS, Linux) seem to,
but with varying levels of bugginess. So default to Translated Layouts on
all non-Windows platforms (but have an option to change this, as many apps are
buggy with non-US layouts, or provide their own keyboard layout translation,
such as DOSBox. */
#if !defined(_WIN32)
static SDL_bool TranslateKeyboardLayout = SDL_TRUE;
#else
static SDL_bool TranslateKeyboardLayout = SDL_FALSE;
#endif
static int VideoDisplayIndex = 0;
static SDL_bool SupportSysWM = SDL_FALSE;
static SDL_bool EventThreadEnabled = SDL_FALSE;
static SDL_bool CDRomInit = SDL_FALSE;
static char *CDRomPath = NULL;
static SDL12_CD *CDRomDevice = NULL;
static SDL12_EventFilter EventFilter12 = NULL;
static SDL12_Cursor *CurrentCursor12 = NULL;
static Uint8 EventStates[SDL12_NUMEVENTS];
static int SwapInterval = 0;
static SDL_bool JoysticksAreGameControllers = SDL_FALSE;
static SDL12_Joystick *JoystickList = NULL;
static int NumJoysticks = 0;
static Uint8 KeyState[SDLK12_LAST];
static SDL_bool MouseInputIsRelative = SDL_FALSE;
static SDL_Point MousePosition = { 0, 0 };
static struct { /* SDL_FPoint */
    float x, y;
} MouseRelativeRemainder = { 0.f, 0.f };
static SDL_bool UseMouseRelativeScaling = SDL_FALSE;
static OpenGLEntryPoints OpenGLFuncs;
static int OpenGLBlitLockCount = 0;
static GLuint OpenGLBlitTexture = 0;
static SDL_bool WantDebugLogging = SDL_FALSE;
static SDL_bool WantScaleMethodNearest = SDL_FALSE;
static SDL_bool WantOpenGLScaling = SDL_FALSE;
static int OpenGLLogicalScalingWidth = 0;
static int OpenGLLogicalScalingHeight = 0;
static GLuint OpenGLLogicalScalingFBO = 0;
static GLuint OpenGLLogicalScalingColor = 0;
static GLuint OpenGLLogicalScalingDepth = 0;
static int OpenGLLogicalScalingSamples = 0;
static GLuint OpenGLLogicalScalingMultisampleFBO = 0;
static GLuint OpenGLLogicalScalingMultisampleColor = 0;
static GLuint OpenGLLogicalScalingMultisampleDepth = 0;
static GLuint OpenGLCurrentReadFBO = 0;
static GLuint OpenGLCurrentDrawFBO = 0;
static SDL_bool ForceGLSwapBufferContext = SDL_FALSE;
static SDL12_TimerID AddedTimers = NULL;  /* we'll protect this with EventQueueMutex for laziness/convenience. */
static SDL_mutex *EventQueueMutex = NULL;
static EventQueueType EventQueuePool[SDL12_MAXEVENTS];
static EventQueueType *EventQueueHead = NULL;
static EventQueueType *EventQueueTail = NULL;
static EventQueueType *EventQueueAvailable = NULL;
static unsigned long SetVideoModeThread = 0;
static SDL_bool VideoSurfaceUpdatedInBackgroundThread = SDL_FALSE;
static SDL_bool AllowThreadedDraws = SDL_FALSE;
static SDL_bool AllowThreadedPumps = SDL_FALSE;
static SDL_bool WantCompatibilityAudioCVT = SDL_FALSE;


/* This is a KEYDOWN event which is being held for a follow-up TEXTINPUT */
static SDL12_Event PendingKeydownEvent;

/* SDL_atoi() before SDL2-2.0.17 is non-compliant */
static SDL_INLINE int SDLCALL
SDL20_atoi(const char *str)
{
    return SDL20_strtol(str, NULL, 10);
}

/* Obviously we can't use SDL_LoadObject() to load SDL2.  :)  */
static char loaderror[256];
#if defined(_WIN32)
    #define DIRSEP "\\"
    #define SDL20_LIBNAME "SDL2.dll"
    /* require SDL2 >= 2.0.12 for SDL_CreateThread binary compatibility */
    #define SDL20_REQUIRED_VER SDL_VERSIONNUM(2,0,12)
    static HMODULE Loaded_SDL20 = NULL;
    #define LoadSDL20Library() ((Loaded_SDL20 = LoadLibraryA(SDL20_LIBNAME)) != NULL)
    #define LookupSDL20Sym(sym) (void *)GetProcAddress(Loaded_SDL20, sym)
    #define CloseSDL20Library() { if (Loaded_SDL20) { FreeLibrary(Loaded_SDL20); Loaded_SDL20 = NULL; } }
    #define strcpy_fn  lstrcpyA
    #define sprintf_fn wsprintfA
#elif defined(__OS2__)
    #include <os2.h>
    #define DIRSEP "\\"
    #define SDL20_LIBNAME "SDL2.dll"
    #define SDL20_LIBNAME2 "SDL2"       /* if loading from LIBPATH */
    #define SDL20_REQUIRED_VER SDL_VERSIONNUM(2,0,7)
    #define strcpy_fn  strcpy
    #define sprintf_fn sprintf
    static HMODULE Loaded_SDL20 = NULLHANDLE;
    static SDL_bool LoadSDL20Library(void) {
        char err[256];
        if (DosLoadModule(err, sizeof(err), SDL20_LIBNAME, &Loaded_SDL20) != 0) {
            if (DosLoadModule(err, sizeof(err), SDL20_LIBNAME2, &Loaded_SDL20) != 0) {
                return SDL_FALSE;
            }
        }
        return SDL_TRUE;
    }
    static void *LookupSDL20Sym (const char *symname) {
        PFN fp;
        if (DosQueryProcAddr(Loaded_SDL20, 0, symname, &fp) != 0) return NULL;
        return (void *)fp;
    }
    static void CloseSDL20Library(void) {
        if (Loaded_SDL20 != NULLHANDLE) {
            DosFreeModule(Loaded_SDL20);
            Loaded_SDL20 = NULLHANDLE;
        }
    }
#elif defined(__APPLE__)
    #include <dlfcn.h>
    #include <pwd.h>
    #include <unistd.h>
    #define SDL20_LIBNAME "libSDL2-2.0.0.dylib"
    /* SDL2 cmake'ry is (was?) messy: */
    #define SDL20_LIBNAME2 "libSDL2-2.0.dylib"
    #define SDL20_FRAMEWORK "SDL2.framework/Versions/A/SDL2"
    #define SDL20_REQUIRED_VER SDL_VERSIONNUM(2,0,7)
    #define strcpy_fn  strcpy
    #define sprintf_fn sprintf
    static void *Loaded_SDL20 = NULL;
    #define LookupSDL20Sym(sym) dlsym(Loaded_SDL20, sym)
    #define CloseSDL20Library() { if (Loaded_SDL20) { dlclose(Loaded_SDL20); Loaded_SDL20 = NULL; } }
    static SDL_bool LoadSDL20Library(void) {
        /* I don't know if this is the _right_ order to try, but this seems reasonable */
        static const char * const dylib_locations[] = {
            "@loader_path/" SDL20_LIBNAME, /* MyApp.app/Contents/MacOS/libSDL2-2.0.0.dylib */
            "@loader_path/" SDL20_LIBNAME2, /* MyApp.app/Contents/MacOS/libSDL2-2.0.dylib */
            "@loader_path/../Frameworks/" SDL20_FRAMEWORK, /* MyApp.app/Contents/Frameworks/SDL2.framework */
            "@executable_path/" SDL20_LIBNAME, /* MyApp.app/Contents/MacOS/libSDL2-2.0.0.dylib */
            "@executable_path/" SDL20_LIBNAME2, /* MyApp.app/Contents/MacOS/libSDL2-2.0.dylib */
            "@executable_path/../Frameworks/" SDL20_FRAMEWORK, /* MyApp.app/Contents/Frameworks/SDL2.framework */
            NULL,  /* /Users/username/Library/Frameworks/SDL2.framework */
            "/Library/Frameworks" SDL20_FRAMEWORK, /* /Library/Frameworks/SDL2.framework */
            SDL20_LIBNAME, /* oh well, anywhere the system can see the .dylib (/usr/local/lib or whatever) */
            SDL20_LIBNAME2
        };

        int i;
        for (i = 0; i < (int) SDL_arraysize(dylib_locations); i++) {
            const char *location = dylib_locations[i];
            if (location) {
                Loaded_SDL20 = dlopen(location, RTLD_LOCAL|RTLD_NOW);
            } else { /* hack to mean "try homedir" */
                const char *homedir = NULL;
                struct passwd *pwent = getpwuid(getuid());
                if (pwent) {
                    homedir = pwent->pw_dir;
                }
                if (!homedir) {
                    homedir = getenv("HOME");
                }
                if (homedir) {
                    char framework[512];
                    const int rc = snprintf(framework, sizeof (framework), "%s/Library/Frameworks/" SDL20_FRAMEWORK, homedir);
                    if ((rc > 0) && (rc < (int) sizeof(framework))) {
                        Loaded_SDL20 = dlopen(framework, RTLD_LOCAL|RTLD_NOW);
                    }
                }
            }

            if (Loaded_SDL20) {
                return SDL_TRUE;
            }
        }

        return SDL_FALSE; /* didn't find it anywhere reasonable. :( */
    }
#elif defined(__unix__)
    #include <dlfcn.h>
    #define SDL20_LIBNAME "libSDL2-2.0.so.0"
    #define SDL20_REQUIRED_VER SDL_VERSIONNUM(2,0,7)
    static void *Loaded_SDL20 = NULL;
    #define LoadSDL20Library() ((Loaded_SDL20 = dlopen(SDL20_LIBNAME, RTLD_LOCAL|RTLD_NOW)) != NULL)
    #define LookupSDL20Sym(sym) dlsym(Loaded_SDL20, sym)
    #define CloseSDL20Library() { if (Loaded_SDL20) { dlclose(Loaded_SDL20); Loaded_SDL20 = NULL; } }
    #define strcpy_fn  strcpy
    #define sprintf_fn sprintf
#else
    #error Please define your platform.
#endif

#ifndef DIRSEP
#define DIRSEP "/"
#endif

static void *
LoadSDL20Symbol(const char *fn, int *okay)
{
    void *retval = NULL;
    if (*okay) { /* only bother trying if we haven't previously failed. */
        retval = LookupSDL20Sym(fn);
        if (retval == NULL) {
            sprintf_fn(loaderror, "%s missing in SDL2 library.", fn);
            *okay = 0;
        }
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

typedef struct QuirkEntryType
{
    const char *exe_name;
    const char *hint_name;
    const char *hint_value;
} QuirkEntryType;

static QuirkEntryType quirks[] = {
#if defined(__unix__)
    /* Awesomenauts uses Cg, and does weird things with the GL context. */
    {"Awesomenauts.bin.x86", "SDL12COMPAT_OPENGL_SCALING", "0"},
    {"Awesomenauts.bin.x86", "SDL12COMPAT_FORCE_GL_SWAPBUFFER_CONTEXT", "1"},

    /* Braid uses Cg, which uses glXGetProcAddress(). */
    {"braid", "SDL12COMPAT_OPENGL_SCALING", "0"},

    /* GOG's DOSBox builds have architecture-specific filenames. */
    {"dosbox", "SDL12COMPAT_USE_KEYBOARD_LAYOUT", "0"},
    {"dosbox_i686", "SDL12COMPAT_USE_KEYBOARD_LAYOUT", "0"},
    {"dosbox_x86_64", "SDL12COMPAT_USE_KEYBOARD_LAYOUT", "0"},

    /* Tucnak's 1.2 target wants to render and run its event loop from
       background threads, which upsets OpenGL. Force software renderer and
       x11. If you want Wayland, etc, use tuknak's existing SDL2 target. */
    {"tucnak", "SDL_VIDEODRIVER", "x11"},
    {"tucnak", "SDL_RENDER_DRIVER", "software"},
    {"tucnak", "SDL_FRAMEBUFFER_ACCELERATION", "false"},

    /* looks for X11 display and does unnecessary X11 things. Causes problems with SDL2/x11. */
    /* also has some issue with audio conversion I didn't look more closely at. */
    {"fillets", "SDL12COMPAT_ALLOW_SYSWM", "0"},
    {"fillets", "SDL12COMPAT_COMPATIBILITY_AUDIOCVT", "1"},

    /* doesn't render with GL scaling enabled */
    {"scorched3d", "SDL12COMPAT_OPENGL_SCALING", "0"},
    {"scorched3dc", "SDL12COMPAT_OPENGL_SCALING", "0"},

    /* boswars has a bug where SDL_AudioCVT must not require extra buffer space. See Issue #232. */
    {"boswars", "SDL12COMPAT_COMPATIBILITY_AUDIOCVT", "1"},

    /* The 32-bit Steam build only of Multiwinia Quits but doesn't re-Init */
    {"multiwinia.bin.x86", "SDL12COMPAT_NO_QUIT_VIDEO", "1"}
#else
    /* TODO: Add any quirks needed for this system. */

    /* A dummy entry to keep compilers happy. */
    {"", "", "0"}
#endif
};

#ifdef __linux__
static void OS_GetExeName(char *buf, const unsigned maxpath) {
    buf[0] = '\0';
    readlink("/proc/self/exe", buf, maxpath);
}
#elif defined(_WIN32)
static void OS_GetExeName(char *buf, const unsigned maxpath) {
    buf[0] = '\0';
    GetModuleFileNameA(NULL, buf, maxpath);
}
#elif defined(__OS2__)
static void OS_GetExeName(char *buf, const unsigned maxpath) {
    PPIB pib;
    DosGetInfoBlocks(NULL, &pib);
    buf[0] = '\0';
    DosQueryModuleName(pib->pib_hmte, maxpath, buf);
}
#elif defined(__APPLE__) || defined(__FREEBSD__)
static void OS_GetExeName(char *buf, const unsigned maxpath) {
    const char *progname = getprogname();
    if (progname != NULL) {
        strlcpy(buf, progname, maxpath);
    } else {
        buf[0] = '\0';
    }
}
#else
#warning Please implement this for your platform.
static void OS_GetExeName(char *buf, const unsigned maxpath) {
    buf[0] = '\0';
    (void)maxpath;
}
#endif

static const char *
SDL12Compat_GetExeName(void)
{
    static const char *exename = NULL;
    if (exename == NULL) {
        static char path_buf[SDL12_MAXPATH];
        static char *base_path;
        OS_GetExeName(path_buf, SDL12_MAXPATH);
        base_path = SDL20_strrchr(path_buf, *DIRSEP);
        if (base_path) {
            /* We have a '\\' component. */
            exename = base_path + 1;
        } else {
            /* No slashes, return the whole module filanem. */
            exename = path_buf;
        }
    }
    return exename;
}

static const char *
SDL12Compat_GetHint(const char *name)
{
    return SDL20_getenv(name);
}

static SDL_bool
SDL12Compat_GetHintBoolean(const char *name, SDL_bool default_value)
{
    const char *val = SDL12Compat_GetHint(name);

    if (!val) {
        return default_value;
    }

    return (SDL20_atoi(val) != 0) ? SDL_TRUE : SDL_FALSE;
}

static float
SDL12Compat_GetHintFloat(const char *name, float default_value)
{
    const char *val = SDL12Compat_GetHint(name);

    if (!val) {
        return default_value;
    }

    return (float) SDL20_atof(val);
}

static void
SDL12Compat_ApplyQuirks(SDL_bool force_x11)
{
    const char *exe_name = SDL12Compat_GetExeName();
    int i;

    if (WantDebugLogging) {
        SDL20_Log("This app appears to be named '%s'", exe_name);
    }

    #ifdef __linux__
    if (force_x11) {
        const char *videodriver_env = SDL20_getenv("SDL_VIDEODRIVER");
        if (videodriver_env && (SDL20_strcmp(videodriver_env, "x11") != 0)) {
            if (WantDebugLogging) {
                SDL20_Log("This app looks like it requires X11, but the SDL_VIDEODRIVER environment variable is set to \"%s\". If you have issues, try setting SDL_VIDEODRIVER=x11", videodriver_env);
            }
        } else {
            if (WantDebugLogging) {
                SDL20_Log("sdl12-compat: We are forcing this app to use X11, because it probably talks to an X server directly, outside of SDL. If possible, this app should be fixed, to be compatible with Wayland, etc.");
            }
            SDL20_setenv("SDL_VIDEODRIVER", "x11", 1);
        }
    }
    #endif

    if (*exe_name == '\0') {
        return;
    }
    for (i = 0; i < (int) SDL_arraysize(quirks); i++) {
        if (!SDL20_strcmp(exe_name, quirks[i].exe_name)) {
            if (!SDL20_getenv(quirks[i].hint_name)) {
                if (WantDebugLogging) {
                    SDL20_Log("Applying compatibility quirk %s=\"%s\" for \"%s\"", quirks[i].hint_name, quirks[i].hint_value, exe_name);
                }
                SDL20_setenv(quirks[i].hint_name, quirks[i].hint_value, 1);
            } else {
                if (WantDebugLogging) {
                    SDL20_Log("Not applying compatibility quirk %s=\"%s\" for \"%s\" due to environment variable override (\"%s\")\n",
                            quirks[i].hint_name, quirks[i].hint_value, exe_name, SDL20_getenv(quirks[i].hint_name));
                }
            }
        }
    }
}

static int
LoadSDL20(void)
{
    int okay = 1;
    if (!Loaded_SDL20) {
        SDL_bool force_x11 = SDL_FALSE;

        #ifdef __linux__
        void *global_symbols = dlopen(NULL, RTLD_LOCAL|RTLD_NOW);

        /* Use linked libraries to detect what quirks we are likely to need */
        if (global_symbols != NULL) {
            if (dlsym(global_symbols, "glxewInit") != NULL) {  /* GLEW (e.g. Frogatto, SLUDGE) */
                force_x11 = SDL_TRUE;
            } else if (dlsym(global_symbols, "cgGLEnableProgramProfiles") != NULL) {  /* NVIDIA Cg (e.g. Awesomenauts, Braid) */
                force_x11 = SDL_TRUE;
            } else if (dlsym(global_symbols, "_Z7ssgInitv") != NULL) {  /* ::ssgInit(void) in plib (e.g. crrcsim) */
                force_x11 = SDL_TRUE;
            }
            dlclose(global_symbols);
        }
        #endif

        okay = LoadSDL20Library();
        if (!okay) {
            strcpy_fn(loaderror, "Failed loading SDL2 library.");
        } else {
            #define SDL20_SYM(rc,fn,params,args,ret) SDL20_##fn = (SDL20_##fn##_t) LoadSDL20Symbol("SDL_" #fn, &okay);
            #include "SDL20_syms.h"
            if (okay) {
                SDL_version v;
                SDL20_GetVersion(&v);
                LinkedSDL2VersionInt = SDL_VERSIONNUM(v.major, v.minor, v.patch);
                okay = (LinkedSDL2VersionInt >= SDL20_REQUIRED_VER);
                if (!okay) {
                    sprintf_fn(loaderror, "SDL2 %d.%d.%d library is too old.", v.major, v.minor, v.patch);
                } else {
                    WantDebugLogging = SDL12Compat_GetHintBoolean("SDL12COMPAT_DEBUG_LOGGING", SDL_FALSE);
                    if (WantDebugLogging) {
                        #if defined(__DATE__) && defined(__TIME__)
                        SDL20_Log("sdl12-compat 1.2.%d, built on " __DATE__ " at " __TIME__ ", talking to SDL2 %d.%d.%d", SDL12_COMPAT_VERSION, v.major, v.minor, v.patch);
                        #else
                        SDL20_Log("sdl12-compat 1.2.%d, talking to SDL2 %d.%d.%d", SDL12_COMPAT_VERSION, v.major, v.minor, v.patch);
                        #endif
                    }
                    SDL12Compat_ApplyQuirks(force_x11);  /* Apply and maybe print a list of any enabled quirks. */
                }
            }
            if (!okay) {
                UnloadSDL20();
            }
        }
    }
    return okay;
}

#if defined(_WIN32)
static void error_dialog(const char *errorMsg)
{
    MessageBoxA(NULL, errorMsg, "Error", MB_OK | MB_SETFOREGROUND | MB_ICONSTOP);
}
#elif defined(__APPLE__)
extern void error_dialog(const char *errorMsg);
#else
static void error_dialog(const char *errorMsg)
{
    fprintf(stderr, "%s\n", errorMsg);
}
#endif

#if defined(__GNUC__) && !defined(_WIN32)
static void dllinit(void) __attribute__((constructor));
static void dllinit(void)
{
    if (!LoadSDL20()) {
        error_dialog(loaderror);
        abort();
    }
}
static void dllquit(void) __attribute__((destructor));
static void dllquit(void)
{
    UnloadSDL20();
}

#elif defined(_WIN32) && (defined(_MSC_VER) || defined(__MINGW32__) || defined(__WATCOMC__))
#if defined(_MSC_VER) && !defined(__FLTUSED__)
#define __FLTUSED__
__declspec(selectany) int _fltused = 1;
#endif
#if defined(__MINGW32__)
#define _DllMainCRTStartup DllMainCRTStartup
#endif
#if defined(__WATCOMC__)
#define _DllMainCRTStartup LibMain
#endif
BOOL WINAPI _DllMainCRTStartup(HANDLE dllhandle, DWORD reason, LPVOID reserved)
{
    (void) dllhandle;
    (void) reserved;
    switch (reason) {
    case DLL_PROCESS_DETACH:
        UnloadSDL20();
        break;

    case DLL_PROCESS_ATTACH: /* init once for each new process */
        if (!LoadSDL20()) {
            error_dialog(loaderror);
            #if 0
            TerminateProcess(GetCurrentProcess(), 42);
            ExitProcess(42);
            #endif
            return FALSE;
        }
        break;

    case DLL_THREAD_ATTACH: /* thread-specific init. */
    case DLL_THREAD_DETACH: /* thread-specific cleanup */
        break;
    }
    return TRUE;
}

#elif defined(__WATCOMC__) && defined(__OS2__)
unsigned _System LibMain(unsigned hmod, unsigned termination)
{
    (void)hmod;
    if (termination) {
        UnloadSDL20();
    } else if (!LoadSDL20()) {
        error_dialog(loaderror);
        return 0;
    }
    return 1;
}

#else
    #error Please define an init procedure for your platform.
#endif

#ifdef _WIN32
/* SDL_main functions:
 * SDL2 doesn't define SDL_MAIN_NEEDED for _WIN32,
 * therefore no need to call SDL_SetMainReady().
 */
DECLSPEC12 void SDLCALL
SDL_SetModuleHandle(void *handle)
{
    (void) handle;/* handled internally by SDL2 - nothing to do.. */
}
#endif

DECLSPEC12 const SDL_version * SDLCALL
SDL_Linked_Version(void)
{
    static const SDL_version version = { 1, 2, SDL12_COMPAT_VERSION };
    return &version;
}

DECLSPEC12 int SDLCALL
SDL_sscanf(const char *text, const char *fmt, ...)
{
    int retval;
    va_list ap;
    va_start(ap, fmt);
    retval = (int) SDL20_vsscanf(text, fmt, ap);
    va_end(ap);
    return retval;
}

DECLSPEC12 int SDLCALL
SDL_snprintf(char *text, size_t maxlen, const char *fmt, ...)
{
    int retval;
    va_list ap;
    va_start(ap, fmt);
    retval = (int) SDL20_vsnprintf(text, maxlen, fmt, ap);
    va_end(ap);
    return retval;
}

DECLSPEC12 void * SDLCALL
SDL_revcpy(void *_dst, const void *_src, size_t len)
{
    if (len > 0) {
        Uint8 *dst = (((Uint8 *) _dst) + len) - 1;
        const Uint8 *src = (((const Uint8 *) _src) + len) - 1;
        size_t i;
        for (i = 0; i < len; i++, src--, dst--) {
            *dst = *src;
        }
    }
    return _dst;
}


/* SDL2 doesn't have MMXExt / 3dNowExt. */
#if defined(__GNUC__) && defined(__i386__)
#define cpuid(func, a, b, c, d) \
    __asm__ __volatile__ ( \
"        pushl %%ebx        \n" \
"        xorl %%ecx,%%ecx   \n" \
"        cpuid              \n" \
"        movl %%ebx, %%esi  \n" \
"        popl %%ebx         \n" : \
            "=a" (a), "=S" (b), "=c" (c), "=d" (d) : "a" (func))
#elif defined(__GNUC__) && defined(__x86_64__)
#define cpuid(func, a, b, c, d) \
    __asm__ __volatile__ ( \
"        pushq %%rbx        \n" \
"        xorq %%rcx,%%rcx   \n" \
"        cpuid              \n" \
"        movq %%rbx, %%rsi  \n" \
"        popq %%rbx         \n" : \
            "=a" (a), "=S" (b), "=c" (c), "=d" (d) : "a" (func))
#elif (defined(_MSC_VER) && defined(_M_IX86)) || defined(__WATCOMC__)
#define cpuid(func, a, b, c, d) \
    __asm { \
        __asm mov eax, func \
        __asm xor ecx, ecx \
        __asm cpuid \
        __asm mov a, eax \
        __asm mov b, ebx \
        __asm mov c, ecx \
        __asm mov d, edx \
}
#elif defined(_MSC_VER) && defined(_M_X64)
#include <intrin.h>
#define cpuid(func, a, b, c, d) \
{ \
    int CPUInfo[4]; \
    __cpuid(CPUInfo, func); \
    a = CPUInfo[0]; \
    b = CPUInfo[1]; \
    c = CPUInfo[2]; \
    d = CPUInfo[3]; \
}
#else
#define cpuid(func, a, b, c, d) \
    do { a = b = c = d = 0; (void) a; (void) b; (void) c; (void) d; } while (0)
#endif

static int cpu_ext_features = -1;
static int get_cpu_ext_features(void) {
    if (cpu_ext_features < 0) {
        cpu_ext_features = 0;
        if (SDL20_HasMMX()) {
            int a, b, c, d;
            cpuid(0x80000000, a, b, c, d);
            if (a >= 0x80000001) {
                cpuid(0x80000001, a, b, c, d);
                cpu_ext_features = d;
            }
        }
    }
    return cpu_ext_features;
}

DECLSPEC12 SDL_bool SDLCALL
SDL_HasMMXExt(void)
{
    return (get_cpu_ext_features() & 0x00400000)? SDL_TRUE : SDL_FALSE;
}

DECLSPEC12 SDL_bool SDLCALL
SDL_Has3DNowExt(void)
{
    return (get_cpu_ext_features() & 0x40000000)? SDL_TRUE : SDL_FALSE;
}


/* SDL 1.2 has no concept of joystick hotplug, so you only have
   access to sticks seen during SDL_Init() and if one of them is
   unplugged, all you can do is fail to open it, or stop reporting
   new input.

   Also, we optionally let the app see SDL2 Game Controllers as
   SDL 1.2 joysticks, which gives them a stable button/axis layout
   and can deal with other hardware quirks.
*/

static SDL_bool
BogusJoystick(SDL12_Joystick *stick12)
{
    const int device_index = (int) (stick12 - JoystickList);
    if (!stick12 || (device_index < 0) || (device_index >= NumJoysticks)) {
        SDL20_SetError("Invalid SDL_Joystick");
        return SDL_TRUE;
    }
    return SDL_FALSE;
}

static SDL_bool
BogusJoystickIndex(const int device_index)
{
    if ((device_index < 0) || (device_index >= NumJoysticks)) {
        SDL20_SetError("Invalid SDL_Joystick");
        return SDL_TRUE;
    }
    return SDL_FALSE;
}

static int
FindJoystick12IndexByInstanceId(const SDL_JoystickID instance_id)
{
    int i;
    for (i = 0; i < NumJoysticks; i++) {
        if (JoystickList[i].instance_id == instance_id) {
            return (SDL20_AtomicGet(&JoystickList[i].refcount) > 0) ? i : -1;
        }
    }
    return -1;
}

static void
Init12Joystick(void)
{
    int numsticks20;
    int i;

    JoysticksAreGameControllers = SDL12Compat_GetHintBoolean("SDL12COMPAT_USE_GAME_CONTROLLERS", SDL_FALSE);
    NumJoysticks = 0;

    SDL20_LockJoysticks();

    numsticks20 = SDL20_NumJoysticks();

    if (numsticks20 > 255) {
        numsticks20 = 255;  /* it has to fit in a Uint8 for the joystick events. */
    }

    JoystickList = (SDL12_Joystick *) ((numsticks20 > 0) ? SDL20_calloc(numsticks20, sizeof (SDL12_Joystick)) : NULL);
    if (JoystickList != NULL) {
        for (i = 0; i < numsticks20; i++) {
            const char *name;
            SDL_bool opened;

            if (JoysticksAreGameControllers && !SDL20_IsGameController(i)) {
                continue;
            }

            name = JoysticksAreGameControllers ? SDL20_GameControllerNameForIndex(i) : SDL20_JoystickNameForIndex(i);
            if (!name) {
                name = JoysticksAreGameControllers ? "Generic SDL2 Game Controller" : "Generic SDL2 Joystick";
            }

            JoystickList[NumJoysticks].name = SDL20_strdup(name);
            if (!JoystickList[NumJoysticks].name) {
                continue;
            }

            if (JoysticksAreGameControllers) {
                JoystickList[NumJoysticks].dev.controller = SDL20_GameControllerOpen(i);
                opened = JoystickList[NumJoysticks].dev.controller != NULL ? SDL_TRUE : SDL_FALSE;
            } else {
                JoystickList[NumJoysticks].dev.joystick = SDL20_JoystickOpen(i);
                opened = JoystickList[NumJoysticks].dev.joystick != NULL ? SDL_TRUE : SDL_FALSE;
            }

            if (!opened) {
                SDL20_free(JoystickList[NumJoysticks].name);
                JoystickList[NumJoysticks].name = NULL;
            }

            JoystickList[NumJoysticks].instance_id = SDL20_JoystickGetDeviceInstanceID(i);

            NumJoysticks++;
        }
    }

    SDL20_UnlockJoysticks();

    if ((NumJoysticks == 0) && (JoystickList)) {
        SDL20_free(JoystickList);
        JoystickList = NULL;
    } else if (NumJoysticks < numsticks20) {  /* shrink the array if possible. */
        void *ptr = SDL20_realloc(JoystickList, sizeof (*JoystickList) * NumJoysticks);
        if (ptr) {
            JoystickList = (SDL12_Joystick *) ptr;
        }
    }
}

static void
Quit12Joystick(void)
{
    int i;
    for (i = 0; i < NumJoysticks; i++) {
        SDL12_Joystick *stick12 = &JoystickList[i];
        if (JoysticksAreGameControllers) {
            SDL20_GameControllerClose(stick12->dev.controller);
        } else {
            SDL20_JoystickClose(stick12->dev.joystick);
        }
        SDL20_free(stick12->name);
    }

    SDL20_free(JoystickList);
    JoystickList = NULL;
    NumJoysticks = 0;
}

DECLSPEC12 int SDLCALL
SDL_NumJoysticks(void)
{
    return NumJoysticks;
}

DECLSPEC12 int SDLCALL
SDL_JoystickNumAxes(SDL12_Joystick *stick12)
{
    if (BogusJoystick(stick12)) {
        return -1;
    }
    return JoysticksAreGameControllers ? (SDL_CONTROLLER_AXIS_MAX + 1) : SDL20_JoystickNumAxes(stick12->dev.joystick);
}

DECLSPEC12 int SDLCALL
SDL_JoystickNumBalls(SDL12_Joystick *stick12)
{
    if (BogusJoystick(stick12)) {
        return -1;
    }
    return JoysticksAreGameControllers ? 0 : SDL20_JoystickNumBalls(stick12->dev.joystick);
}

DECLSPEC12 int SDLCALL
SDL_JoystickNumHats(SDL12_Joystick *stick12)
{
    if (BogusJoystick(stick12)) {
        return -1;
    }
    return JoysticksAreGameControllers ? 0 : SDL20_JoystickNumHats(stick12->dev.joystick);
}

DECLSPEC12 int SDLCALL
SDL_JoystickNumButtons(SDL12_Joystick *stick12)
{
    if (BogusJoystick(stick12)) {
        return -1;
    }
    return JoysticksAreGameControllers ? (SDL_CONTROLLER_BUTTON_MAX + 1) : SDL20_JoystickNumButtons(stick12->dev.joystick);
}

DECLSPEC12 void SDLCALL
SDL_JoystickUpdate(void)
{
    if (JoysticksAreGameControllers) {
        SDL20_GameControllerUpdate();
    } else {
        SDL20_JoystickUpdate();
    }
}

DECLSPEC12 int SDLCALL
SDL_JoystickEventState(int state)
{
    switch (state) {
        case SDL_DISABLE:
        case SDL_ENABLE:
            if (JoysticksAreGameControllers) {
                SDL20_JoystickEventState(state);
                return SDL20_GameControllerEventState(state);
            }
            SDL20_GameControllerEventState(state);
            return SDL20_JoystickEventState(state);
        default:  /* everything else is treated as SDL_QUERY */
            /* we turn joystick and controller event state off together, so we only have to query one. */
            return SDL20_JoystickEventState(SDL_QUERY);
    }
}

DECLSPEC12 Sint16 SDLCALL
SDL_JoystickGetAxis(SDL12_Joystick *stick12, int axis)
{
    if (BogusJoystick(stick12)) {
        return 0;
    }
    return JoysticksAreGameControllers ? SDL20_GameControllerGetAxis(stick12->dev.controller, axis) : SDL20_JoystickGetAxis(stick12->dev.joystick, axis);
}

DECLSPEC12 Uint8 SDLCALL
SDL_JoystickGetHat(SDL12_Joystick *stick12, int hat)
{
    if (BogusJoystick(stick12)) {
        return 0;
    }
    return JoysticksAreGameControllers ? 0 : SDL20_JoystickGetHat(stick12->dev.joystick, hat);
}

DECLSPEC12 int SDLCALL
SDL_JoystickGetBall(SDL12_Joystick *stick12, int ball, int *dx, int *dy)
{
    if (BogusJoystick(stick12)) {
        return 0;
    } else if (JoysticksAreGameControllers) {
        if (dx) { *dx = 0; }
        if (dy) { *dy = 0; }
        return SDL20_SetError("No joystick balls available");
    }
    return SDL20_JoystickGetBall(stick12->dev.joystick, ball, dx, dy);
}

DECLSPEC12 Uint8 SDLCALL
SDL_JoystickGetButton(SDL12_Joystick *stick12, int button)
{
    if (BogusJoystick(stick12)) {
        return 0;
    }
    return JoysticksAreGameControllers ? SDL20_GameControllerGetButton(stick12->dev.controller, button) : SDL20_JoystickGetButton(stick12->dev.joystick, button);
}

DECLSPEC12 SDL12_Joystick * SDLCALL
SDL_JoystickOpen(int device_index)
{
    if (BogusJoystickIndex(device_index)) {
        return NULL;
    }

    /* multiple opens just increments a refcount and returns the same object in SDL 1.2 Classic. */
    SDL20_AtomicAdd(&JoystickList[device_index].refcount, 1);
    return &JoystickList[device_index];
}

DECLSPEC12 void SDLCALL
SDL_JoystickClose(SDL12_Joystick *stick12)
{
    if (!BogusJoystick(stick12)) {
        /* we don't actually close anything here, just drop the refcount. */
        if (SDL20_AtomicAdd(&stick12->refcount, -1) == 0) {
            SDL20_AtomicAdd(&stick12->refcount, 1);  /* whoops, wasn't open, bounce it back to zero. */
        }
    }
}

DECLSPEC12 const char * SDLCALL
SDL_JoystickName(int device_index)
{
    return BogusJoystickIndex(device_index) ? NULL : JoystickList[device_index].name;
}

DECLSPEC12 int SDLCALL
SDL_JoystickIndex(SDL12_Joystick *stick12)
{
    return BogusJoystick(stick12) ? -1 : (int) (stick12 - JoystickList);
}

DECLSPEC12 int SDLCALL
SDL_JoystickOpened(int device_index)
{
    if (BogusJoystickIndex(device_index)) {
        return 0;  /* SDL 1.2 classic doesn't return an error here, either. */
    }
    return SDL20_AtomicGet(&JoystickList[device_index].refcount) ? 1 : 0;
}

static SDL_PixelFormat *
PixelFormat12to20(SDL_PixelFormat *format20, SDL_Palette *palette20, const SDL12_PixelFormat *format12)
{
    if (format12->palette) {
        palette20->ncolors = format12->palette->ncolors;
        palette20->colors = format12->palette->colors;
        palette20->version = 1;
        palette20->refcount = 1;
        format20->palette = palette20;
    } else {
        format20->palette = NULL;
    }

    format20->format = SDL20_MasksToPixelFormatEnum(format12->BitsPerPixel, format12->Rmask, format12->Gmask, format12->Bmask, format12->Amask);
    format20->BitsPerPixel = format12->BitsPerPixel;
    format20->BytesPerPixel = format12->BytesPerPixel;

    /* Paletted surfaces shouldn't have masks in SDL 2.0 */
    if (format12->palette) {
        format20->Rmask = 0;
        format20->Gmask = 0;
        format20->Bmask = 0;
        format20->Amask = 0;
        format20->Rloss = 8;
        format20->Gloss = 8;
        format20->Bloss = 8;
        format20->Aloss = 8;
        format20->Rshift = 0;
        format20->Gshift = 0;
        format20->Bshift = 0;
        format20->Ashift = 0;
    } else {
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
    }
    format20->refcount = 1;
    format20->next = NULL;
    return format20;
}

static SDL12_PixelFormat *
PixelFormat20to12(SDL12_PixelFormat *format12, SDL12_Palette *palette12, const SDL_PixelFormat *format20)
{
    if (format20->palette) {
        palette12->ncolors = format20->palette->ncolors;
        palette12->colors = format20->palette->colors;
        format12->palette = palette12;
    } else {
        format12->palette = NULL;
    }

    format12->BitsPerPixel = format20->BitsPerPixel;
    format12->BytesPerPixel = format20->BytesPerPixel;
    format12->Rloss = format20->Rloss;
    format12->Gloss = format20->Gloss;
    format12->Bloss = format20->Bloss;
    format12->Aloss = format20->Aloss;
    format12->Rshift = format20->Rshift;
    format12->Gshift = format20->Gshift;
    format12->Bshift = format20->Bshift;
    format12->Ashift = format20->Ashift;
    format12->Rmask = format20->Rmask;
    format12->Gmask = format20->Gmask;
    format12->Bmask = format20->Bmask;
    format12->Amask = format20->Amask;
    format12->colorkey = 0; /* this is a surface, not pixelformat, properties in SDL2. */
    format12->alpha = 255;  /* this is a surface, not pixelformat, properties in SDL2. */
    return format12;
}

static int
GetVideoDisplay(void)
{
    const char *variable;
    variable = SDL20_getenv("SDL_VIDEO_FULLSCREEN_DISPLAY");
    if (!variable) {
        variable = SDL20_getenv("SDL_VIDEO_FULLSCREEN_HEAD");
    }
    if (variable) {
        int preferred_display = SDL20_atoi(variable);
        
        if (preferred_display < 0 || preferred_display >= SDL20_GetNumVideoDisplays()) {
            return 0;
        }

        return SDL20_atoi(variable);
    } else {
        return 0;
    }
}

/* returns true if mode1 should sort before mode2 */
static int
VidModeSizeGreater(SDL12_Rect *mode1, SDL12_Rect *mode2)
{
    if (mode1->w > mode2->w) {
        return 1;
    } else if (mode2->w > mode1->w) {
        return 0;
    } else {
        return (mode1->h > mode2->h);
    }
}

static int
AddVidModeToList(VideoModeList *vmode, SDL12_Rect *mode, const Uint16 maxw, const Uint16 maxh)
{
    void *ptr = NULL;
    int i;

    if ( (maxw && (mode->w > maxw)) || (maxh && (mode->h > maxh)) ) {
        return 0;   /* clamp this one out as too big. */
    }

    /* make sure we don't have this one already (with a different refresh rate, etc). */
    for (i = 0; i < vmode->nummodes; i++) {
        if ((vmode->modeslist12[i].w == mode->w) && (vmode->modeslist12[i].h == mode->h)) {
            break;
        }
    }

    if (i < vmode->nummodes) {
        return 0;  /* already have this one. */
    }

    ptr = SDL20_realloc(vmode->modeslist12, sizeof (SDL12_Rect) * (vmode->nummodes + 1));
    if (ptr == NULL) {
        return SDL20_OutOfMemory();
    }
    vmode->modeslist12 = (SDL12_Rect *) ptr;

    vmode->modeslist12[vmode->nummodes] = *mode;

    vmode->nummodes++;

    return 0;
}

/* A list of fake video modes which are included. */
static SDL12_Rect fake_modes[] = {
    { 0, 0, 7680, 4320 },
    { 0, 0, 6144, 3160 },
    { 0, 0, 5120, 2880 },
    { 0, 0, 4096, 2304 },
    { 0, 0, 3840, 2160 },
    { 0, 0, 3200, 1800 },
    { 0, 0, 2880, 1600 },
    { 0, 0, 2560, 1600 },
    { 0, 0, 2048, 1536 },
    { 0, 0, 1920, 1440 },
    { 0, 0, 1920, 1200 },
    { 0, 0, 1920, 1080 },
    { 0, 0, 1680, 1050 },
    { 0, 0, 1600, 1200 },
    { 0, 0, 1600, 900 },
    { 0, 0, 1440, 1080 },
    { 0, 0, 1440, 900 },
    { 0, 0, 1400, 1050 },
    { 0, 0, 1368, 768 },
    { 0, 0, 1280, 1024 },
    { 0, 0, 1280, 960 },
    { 0, 0, 1280, 800 },
    { 0, 0, 1280, 720 },
    { 0, 0, 1152, 864 },
    { 0, 0, 1024, 768 },
    { 0, 0, 864, 486 },
    { 0, 0, 800, 600 },
    { 0, 0, 720, 480 },
    { 0, 0, 640, 480 }
};

/* This sets up VideoModes and VideoModesCount. You end up with arrays by pixel
    format, each with a value that 1.2's SDL_ListModes() can return. */
static int
Init12VidModes(void)
{
    const int total = SDL20_GetNumDisplayModes(VideoDisplayIndex);
    const char *maxmodestr;
    VideoModeList *vmode = NULL;
    void *ptr = NULL;
    int i, j;
    SDL12_Rect prev_mode = { 0, 0, 0, 0 }, current_mode = { 0, 0, 0, 0 };
    /* We only want to enable fake modes if OpenGL Logical Scaling is enabled. */
    const SDL_bool use_fake_modes = SDL12Compat_GetHintBoolean("SDL12COMPAT_OPENGL_SCALING", SDL_TRUE);
    Uint16 maxw = 0;
    Uint16 maxh = 0;

    if (VideoModesCount > 0) {
        return 0;  /* already did this. */
    }

    WantOpenGLScaling = use_fake_modes;

    SDL_assert(VideoModes == NULL);

    maxmodestr = SDL12Compat_GetHint("SDL12COMPAT_MAX_VIDMODE");
    if (maxmodestr) {
        unsigned int w = 0, h = 0;
        SDL_sscanf(maxmodestr, "%ux%u", &w, &h);
        if (w > 0xFFFF) w = 0xFFFF;
        if (h > 0xFFFF) h = 0xFFFF;
        maxw = w;
        maxh = h;
    }

    for (i = 0; i < total; ++i) {
        SDL_DisplayMode mode;

        if (SDL20_GetDisplayMode(VideoDisplayIndex, i, &mode) < 0) {
            continue;
        }

        if ((mode.w == 0) && (mode.h == 0)) {
            /* SDL2 has a bug in its dummy driver before 2.0.16 that causes it to report a bogus video mode. */
            if (IsDummyVideo && (LinkedSDL2VersionInt <= SDL_VERSIONNUM(2, 0, 15))) {
                mode.w = 1024;
                mode.h = 768;
                mode.format = SDL_PIXELFORMAT_RGB888;
            }
        }

        if ((mode.w <= 0) || (mode.h <= 0)) {
            continue;  /* bogus mode for whatever reason, ignore it. */
        }

        if (mode.w > 65535 || mode.h > 65535) {
            continue;  /* can't fit to 16-bits for SDL12_Rect */
        }

        if (!vmode || (mode.format != vmode->format)) {  /* SDL20_GetDisplayMode() sorts on bpp first. We know when to change arrays. */
            ptr = (VideoModeList *) SDL20_realloc(VideoModes, sizeof (VideoModeList) * (VideoModesCount+1));
            if (!ptr) {
                return SDL20_OutOfMemory();
            }
            VideoModes = (VideoModeList *) ptr;
            vmode = &VideoModes[VideoModesCount];
            vmode->format = mode.format;
            vmode->nummodes = 0;
            vmode->modeslist12 = NULL;
            vmode->modes12 = NULL;
            VideoModesCount++;
        }

        current_mode.w = mode.w;
        current_mode.h = mode.h;

        /* Attempt to add all of the fake modes. */
        if (use_fake_modes) {
            for (j = 0; j < (int) SDL_arraysize(fake_modes); ++j) {
                if (VidModeSizeGreater(&prev_mode, &fake_modes[j]) && VidModeSizeGreater(&fake_modes[j], &current_mode)) {
                    if (AddVidModeToList(vmode, &fake_modes[j], maxw, maxh)) {
                        return SDL20_OutOfMemory();
                    }
                }
            }
        }

        if (AddVidModeToList(vmode, &current_mode, maxw, maxh)) {
            return SDL20_OutOfMemory();
        }

        prev_mode.w = mode.w;
        prev_mode.h = mode.h;
    }

    /* we need to try to add fake modes to the end of the list once there are no more real modes */
    if (use_fake_modes) {
        for (i = 0; i < (int) SDL_arraysize(fake_modes); ++i) {
            if (VidModeSizeGreater(&prev_mode, &fake_modes[i])) {
                if (AddVidModeToList(vmode, &fake_modes[i], maxw, maxh)) {
                    return SDL20_OutOfMemory();
                }
            }
        }
    }

    /* link up modes12 for SDL_ListModes()'s use... */
    for (i = 0, vmode = VideoModes; i < VideoModesCount; i++, vmode++) {
        const int nummodes = vmode->nummodes;
        vmode->modes12 = (SDL12_Rect **) SDL20_calloc(sizeof (SDL12_Rect *), nummodes + 1);
        if (vmode->modes12 == NULL) {
            return SDL20_OutOfMemory();
        }
        for (j = 0; j < nummodes; j++) {
            vmode->modes12[j] = &vmode->modeslist12[j];
        }
    }

    QueuedDisplayOverlays.next = NULL;
    QueuedDisplayOverlaysTail = &QueuedDisplayOverlays;

    return 0;
}

/* we should have a default cursor */
#include "default_cursor.h"
DECLSPEC12 void SDLCALL SDL_FreeCursor(SDL12_Cursor *);

static int
HasWmAvailable(const char *driver)
{
    /* This is not perfect, but this emcompasses everything that SDL 2.22.0
       offers. Most things are console or framebuffer targets, so it's
       easier to list things that are actual GUI user interfaces here. */
    static const char * const gui_targets[] = {
        #ifdef _WIN32
        "windows", "winrt",
        #endif
        #ifdef __APPLE__
        "cocoa",
        #endif
        #ifdef __OS2__
        "DIVE", "VMAN",
        #endif
        #ifdef __HAIKU__
        "haiku",
        #endif
        #ifdef __QNX__
        "qnx",  /* I _think_ this has a window manager... */
        #endif
        "x11", "wayland"  /* just assume anything can MAYBE have these, even if they wouldn't. */
    };
    int i;

    for (i = 0; i < (int) SDL_arraysize(gui_targets); i++) {
        if (SDL20_strcasecmp(driver, gui_targets[i]) == 0) {
            return 1;
        }
    }

    return 0;
}

DECLSPEC12 int SDLCALL SDL_EnableKeyRepeat(int delay, int interval);

static int
Init12Video(void)
{
    const char *driver = SDL20_GetCurrentVideoDriver();
    const char *scale_method_env = SDL12Compat_GetHint("SDL12COMPAT_SCALE_METHOD");
    SDL_DisplayMode mode;
    int i;

    AllowThreadedDraws = SDL12Compat_GetHintBoolean("SDL12COMPAT_ALLOW_THREADED_DRAWS", SDL_TRUE);
    AllowThreadedPumps = SDL12Compat_GetHintBoolean("SDL12COMPAT_ALLOW_THREADED_PUMPS", SDL_TRUE);

    WantScaleMethodNearest = (scale_method_env && !SDL20_strcmp(scale_method_env, "nearest")) ? SDL_TRUE : SDL_FALSE;

    /* Only override this if the env var is set, as the default is platform-specific. */
    TranslateKeyboardLayout = SDL12Compat_GetHintBoolean("SDL12COMPAT_USE_KEYBOARD_LAYOUT", TranslateKeyboardLayout);

    IsDummyVideo = ((driver != NULL) && (SDL20_strcmp(driver, "dummy") == 0)) ? SDL_TRUE : SDL_FALSE;

    EventQueueMutex = SDL20_CreateMutex();
    if (EventQueueMutex == NULL) {
        return -1;
    }

    for (i = 0; i < SDL12_MAXEVENTS-1; i++) {
        EventQueuePool[i].next = &EventQueuePool[i+1];
    }
    EventQueuePool[SDL12_MAXEVENTS-1].next = NULL;

    EventQueueHead = EventQueueTail = NULL;
    EventQueueAvailable = EventQueuePool;

    SDL20_memset(&PendingKeydownEvent, 0, sizeof(SDL12_Event));

    SDL20_memset(EventStates, SDL_ENABLE, sizeof (EventStates)); /* on by default */

    EventStates[SDL12_SYSWMEVENT] = SDL_IGNORE;  /* off by default. */
    SDL20_EventState(SDL_SYSWMEVENT, SDL_IGNORE);

#if defined(SDL_VIDEO_DRIVER_WINDOWS)
    SupportSysWM = (SDL20_strcmp(driver, "windows") == 0) ? SDL_TRUE : SDL_FALSE;
#elif defined(SDL_VIDEO_DRIVER_X11)
    SupportSysWM = (SDL20_strcmp(driver, "x11") == 0) ? SDL_TRUE : SDL_FALSE;
#else
    SupportSysWM = SDL_FALSE;
#endif

    if (!SDL12Compat_GetHintBoolean("SDL12COMPAT_ALLOW_SYSWM", SDL_TRUE)) {
        SupportSysWM = SDL_FALSE;
    }

    SDL_EnableKeyRepeat(0, 0);

    SDL20_DelEventWatch(EventFilter20to12, NULL);
    SDL20_AddEventWatch(EventFilter20to12, NULL);

    VideoDisplayIndex = GetVideoDisplay();
    SwapInterval = 0;

    VideoWindowGrabbed = SDL_FALSE;
    VideoCursorHidden = SDL_FALSE;
    SDL20_ShowCursor(1);

    if (Init12VidModes() < 0) {
        return -1;
    }

    SDL20_StopTextInput();

    if (SDL20_GetDesktopDisplayMode(VideoDisplayIndex, &mode) == 0) {
        VideoInfoVfmt20 = SDL20_AllocFormat(mode.format);
        VideoInfo12.vfmt = PixelFormat20to12(&VideoInfoVfmt12, &VideoInfoPalette12, VideoInfoVfmt20);
        VideoInfo12.current_w = mode.w;
        VideoInfo12.current_h = mode.h;
        VideoInfo12.wm_available = HasWmAvailable(driver);
        VideoInfo12.video_mem = 1024 * 256;  /* good enough. */
    }

    return 0;
}

DECLSPEC12 int SDLCALL
SDL_VideoInit(const char *driver, Uint32 flags)
{
    int retval;

    (void) flags;  /* unused. */

    retval = SDL20_VideoInit(driver);
    if (retval != -1) {
        retval = Init12Video();
        if (retval == -1) {
            SDL20_VideoQuit();
        }
    }
    return retval;
}

static void
Init12Audio(void)
{
    WantCompatibilityAudioCVT = SDL12Compat_GetHintBoolean("SDL12COMPAT_COMPATIBILITY_AUDIOCVT", SDL_FALSE);
}


static void InitializeCDSubsystem(void);
static void QuitCDSubsystem(void);


DECLSPEC12 int SDLCALL
SDL_InitSubSystem(Uint32 sdl12flags)
{
    Uint32 sdl20flags = 0;
    int rc;

#ifdef __WINDOWS__
    /* DOSBox (and probably other things), try to force the "windib" video
       backend, but it doesn't exist in SDL2. Force to "windows" instead. */
    const char *origvidenv = NULL;
    const char *env = SDL20_getenv("SDL_VIDEODRIVER");
    if (env) {
        if (SDL20_strcmp(env, "windib") == 0) {
            origvidenv = "windib";
            SDL20_setenv("SDL_VIDEODRIVER", "windows", 1);
        } else
        if (SDL20_strcmp(env, "directx") == 0) {
            origvidenv = "directx";
            SDL20_setenv("SDL_VIDEODRIVER", "windows", 1);
        }
    }
#endif

#ifdef __MACOSX__
    extern void sdl12_compat_macos_init(void);
    sdl12_compat_macos_init();
#endif

    /* note that currently we ignore SDL12_INIT_NOPARACHUTE, since
       there _isn't_ a parachute in SDL2, and mostly it was meant to deal
       with X11's XVidMode leaving the display resolution busted if the app
       didn't call SDL_Quit() before leaving...but most sdl12-compat cases
       (except OpenGL rendering without logical scaling) use
       FULLSCREEN_DESKTOP for fullscreen modes, and modern X11 doesn't
       have this problem, so we're ignoring the parachute until a reasonable
       need arises. */

    #define SETFLAG(flag) { \
        const Uint32 f12 = SDL12_INIT_##flag; \
        const Uint32 f20 = SDL_INIT_##flag; \
        if ((sdl12flags & f12) && ((InitializedSubsystems20 & f20) == 0)) { \
            sdl20flags |= f20; \
        } \
    }

    SETFLAG(TIMER);
    SETFLAG(AUDIO);
    SETFLAG(VIDEO);
    SETFLAG(JOYSTICK);
    SETFLAG(NOPARACHUTE);
    #undef SETFLAG

    /* There's no CDROM in 2.0, but we fake it. */
    if (sdl12flags & SDL12_INIT_CDROM) {
        /* this never reports failure, even if there's a legit problem. You just won't see any drives. */
        InitializeCDSubsystem();
    }

    rc = SDL20_Init(sdl20flags);
    if ((rc == 0) && (sdl20flags & SDL_INIT_VIDEO)) {
        if (Init12Video() < 0) {
            rc = -1;
        }

        /* SDL_INIT_EVENTTHREAD takes effect when SDL_INIT_VIDEO is also set */
        EventThreadEnabled = (sdl12flags & SDL12_INIT_EVENTTHREAD) ? SDL_TRUE : SDL_FALSE;
    }

#ifdef __WINDOWS__
    if (origvidenv) {  /* set this back to minimize surprise state changes. */
        SDL20_setenv("SDL_VIDEODRIVER", origvidenv, 1);
    }
#endif

    if ((rc == 0) && (sdl20flags & SDL_INIT_AUDIO)) {
        Init12Audio();
    }

    if ((rc == 0) && (sdl20flags & SDL_INIT_JOYSTICK)) {
        Init12Joystick();  /* if this fails, we just won't report any sticks. */
    }

    InitializedSubsystems20 |= sdl20flags;

    return rc;
}

DECLSPEC12 int SDLCALL
SDL_Init(Uint32 sdl12flags)
{
    return SDL_InitSubSystem(sdl12flags);   /* there's no difference betwee Init and InitSubSystem in SDL2. */
}


static void
InitFlags12to20(const Uint32 flags12, Uint32 *_flags20, Uint32 *_extraflags)
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


DECLSPEC12 Uint32 SDLCALL
SDL_WasInit(Uint32 sdl12flags)
{
    Uint32 sdl20flags, extraflags;
    InitFlags12to20(sdl12flags, &sdl20flags, &extraflags);

    return InitFlags20to12(SDL20_WasInit(sdl20flags)) | extraflags;
}


static SDL12_Surface *EndVidModeCreate(void);
static void
Quit12Video(void)
{
    int i;

    SDL_EnableKeyRepeat(0, 0);

    SDL20_FreeSurface(VideoIcon20);
    VideoIcon20 = NULL;

    EndVidModeCreate(); /* clean-up static data. */

    for (i = 0; i < VideoModesCount; i++) {
        SDL20_free(VideoModes[i].modeslist12);
        SDL20_free(VideoModes[i].modes12);
    }
    SDL20_free(VideoModes);

    SDL20_FreeFormat(VideoInfoVfmt20);
    SDL20_zero(VideoInfo12);

    VideoInfoVfmt20 = NULL;
    EventFilter12 = NULL;
    EventQueueAvailable = EventQueueHead = EventQueueTail = NULL;
    SDL20_memset(&PendingKeydownEvent, 0, sizeof(SDL12_Event));
    SDL_FreeCursor(CurrentCursor12);
    VideoModes = NULL;
    VideoModesCount = 0;

    AllowThreadedDraws = SDL_FALSE;
    AllowThreadedPumps = SDL_FALSE;

    if (EventQueueMutex) {
        SDL20_DestroyMutex(EventQueueMutex);
        EventQueueMutex = NULL;
    }

    /* Shutdown the fake event thread. */
    EventThreadEnabled = SDL_FALSE;
}

DECLSPEC12 void SDLCALL
SDL_QuitSubSystem(Uint32 sdl12flags)
{
    Uint32 sdl20flags, extraflags;

    /* Some games (notably the Steam build of Multiwinia), will
     * SDL_Quit(SDL_INIT_VIDEO) on resolution change, and never call
     * SDL_Init() again before creating their new window.
     */
    if (SDL12Compat_GetHintBoolean("SDL12COMPAT_NO_QUIT_VIDEO", SDL_FALSE)) {
        sdl12flags &= ~SDL12_INIT_VIDEO;
    }
    InitFlags12to20(sdl12flags, &sdl20flags, &extraflags);

    if (extraflags & SDL12_INIT_CDROM) {
        QuitCDSubsystem();
    }

    if (sdl12flags & SDL12_INIT_AUDIO) {
        SDL_CloseAudio();
    }

    if (sdl12flags & SDL12_INIT_VIDEO) {
        Quit12Video();
    }

    if (sdl12flags & SDL12_INIT_JOYSTICK) {
        Quit12Joystick();
    }

    SDL20_QuitSubSystem(sdl20flags);

    if ((SDL20_WasInit(0) == 0) && (!CDRomInit)) {
        SDL20_Quit();
    }

    InitializedSubsystems20 &= ~sdl20flags;
    InitializedSubsystems20 &= ~SDL_INIT_NOPARACHUTE;  /* SDL2 accepts this flag but ignores it. */
}

DECLSPEC12 void SDLCALL
SDL_Quit(void)
{
    SDL_QuitSubSystem(SDL_WasInit(0) | SDL12_INIT_CDROM);
    SDL_assert((InitializedSubsystems20 == 0) || (SDL12Compat_GetHintBoolean("SDL12COMPAT_NO_QUIT_VIDEO", SDL_FALSE) && (InitializedSubsystems20 == SDL_INIT_VIDEO)));
}

DECLSPEC12 void SDLCALL
SDL_Error(SDL_errorcode error)
{
    SDL20_Error(error);
}

DECLSPEC12 void SDLCALL
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
    if (!str) {
        SDL20_OutOfMemory();
    } else {
        va_start(ap, fmt);
        SDL20_vsnprintf(str, len + 1, fmt, ap);
        va_end(ap);
        SDL20_SetError("%s", str);
        SDL20_free(str);
    }
}

DECLSPEC12 const char * SDLCALL
SDL_GetError(void)
{
    if (SDL20_GetError == NULL) {
        static const char noload_errstr[] = "SDL2 library isn't loaded.";
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

DECLSPEC12 const char * SDLCALL
SDL_AudioDriverName(char *namebuf, int maxlen)
{
    return GetDriverName(SDL20_GetCurrentAudioDriver(), namebuf, maxlen);
}

DECLSPEC12 const char * SDLCALL
SDL_VideoDriverName(char *namebuf, int maxlen)
{
#ifdef __WINDOWS__
    const char *val = SDL20_getenv("SDL_VIDEODRIVER");
    if (val) {
        /* give them back what they requested: */
        if (SDL20_strcmp(val,  "windib") == 0  ||
            SDL20_strcmp(val, "directx") == 0) {
            return GetDriverName(val, namebuf, maxlen);
        }
    }
    val = SDL20_GetCurrentVideoDriver();
    if (val && SDL20_strcmp(val, "windows") == 0) {
        /* Windows apps may use SDL_VideoDriverName() to check accelerated
         * vs unaccelerated display by using directx and windib.
         * https://github.com/libsdl-org/sdl12-compat/issues/223 */
        val = "directx";
    }
    return GetDriverName(val, namebuf, maxlen);
#else
    return GetDriverName(SDL20_GetCurrentVideoDriver(), namebuf, maxlen);
#endif
}


/* you MUST hold EventQueueMutex before calling this! */
static int
SDL_PollEvent_locked(SDL12_Event *event12)
{
    EventQueueType *next;

    SDL_PumpEvents();  /* this will run our filter and build our 1.2 queue. */

    if (EventQueueHead == NULL) {
        return 0;  /* no events at the moment. */
    }

    if (event12 != NULL) {
        SDL20_memcpy(event12, &EventQueueHead->event12, sizeof (SDL12_Event));
        next = EventQueueHead->next;
        EventQueueHead->next = EventQueueAvailable;
        EventQueueAvailable = EventQueueHead;
        EventQueueHead = next;
        if (!next) {
            EventQueueTail = NULL;
        }
    }

    return 1;
}

DECLSPEC12 int SDLCALL
SDL_PollEvent(SDL12_Event *event12)
{
    int retval;

    if (!EventQueueMutex) {
        return 0;
    }

    SDL20_LockMutex(EventQueueMutex);
    retval = SDL_PollEvent_locked(event12);
    SDL20_UnlockMutex(EventQueueMutex);

    return retval;
}

/* you MUST hold EventQueueMutex before calling this! */
static int
SDL_PushEvent_locked(SDL12_Event *event12)
{
    EventQueueType *item = EventQueueAvailable;
    if (item == NULL) {
        return -1;  /* no space available at the moment. */
    }

    EventQueueAvailable = item->next;
    if (EventQueueTail) {
        EventQueueTail->next = item;
        EventQueueTail = item;
    } else {
        EventQueueHead = EventQueueTail = item;
    }
    item->next = NULL;

    SDL20_memcpy(&item->event12, event12, sizeof (SDL12_Event));

    if (event12->type == SDL12_SYSWMEVENT) {  /* make a copy of the data here */
        SDL20_memcpy(&item->syswm_msg, event12->syswm.msg, sizeof (SDL12_SysWMmsg));
        item->event12.syswm.msg = &item->syswm_msg;
    }

    return 0;
}

DECLSPEC12 int SDLCALL
SDL_PushEvent(SDL12_Event *event12)
{
    int retval;

    if (!EventQueueMutex) {
        return SDL20_SetError("SDL not initialized");
    }

    SDL20_LockMutex(EventQueueMutex);
    retval = SDL_PushEvent_locked(event12);
    SDL20_UnlockMutex(EventQueueMutex);

    return retval;
}


/* you MUST hold EventQueueMutex before calling this! */
static int
SDL_PeepEvents_locked(SDL12_Event *events12, int numevents, SDL_eventaction action, Uint32 mask)
{
    SDL12_Event dummy_event;

    /* We don't actually implement an event thread in sdl12-compat, but some
     * games will only call SDL_PeepEvents(), which doesn't otherwise pump
     * events, and get stuck when they've consumed all the events.
     *
     * Just pumping the event loop here simulates an event thread well enough
     * for most things.
     */
    if (EventThreadEnabled) {
        SDL_PumpEvents();
    }

    if (action == SDL_ADDEVENT) {
        int i;
        for (i = 0; i < numevents; i++) {
            if (SDL_PushEvent(&events12[i]) < 0) {
                break;  /* out of space for more events. */
            }
        }
        return i;
    }

    if (!events12) {
        action = SDL_PEEKEVENT;
        numevents = 1;
        events12 = &dummy_event;
    }

    if ((action == SDL_PEEKEVENT) || (action == SDL_GETEVENT)) {
        const SDL_bool is_get = (action == SDL_GETEVENT)? SDL_TRUE : SDL_FALSE;
        EventQueueType *prev = NULL;
        EventQueueType *item = EventQueueHead;
        EventQueueType *next = NULL;
        int chosen = 0;
        while (chosen < numevents) {
            EventQueueType *nextPrev = item;
            if (!item) {
                break;  /* no more events at the moment. */
            }

            next = item->next;  /* copy, since we might overwrite item->next */

            if (mask & (1<<item->event12.type)) {
                SDL20_memcpy(&events12[chosen++], &item->event12, sizeof (SDL12_Event));
                if (is_get) { /* remove from list? */
                    if (prev != NULL) {
                        prev->next = next;
                    }
                    if (item == EventQueueHead) {
                        EventQueueHead = next;
                    }
                    if (item == EventQueueTail) {
                        EventQueueTail = prev;
                    }

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

DECLSPEC12 int SDLCALL
SDL_PeepEvents(SDL12_Event *events12, int numevents, SDL_eventaction action, Uint32 mask)
{
    int retval;

    if (!EventQueueMutex) {
        return SDL20_SetError("SDL not initialized");
    }

    SDL20_LockMutex(EventQueueMutex);
    retval = SDL_PeepEvents_locked(events12, numevents, action, mask);
    SDL20_UnlockMutex(EventQueueMutex);

    return retval;
}

DECLSPEC12 int SDLCALL
SDL_WaitEvent(SDL12_Event *event12)
{
    if (!EventQueueMutex) {
        return SDL20_SetError("SDL not initialized");
    }

    /* the 1.2 entry point for PollEvent will grab/release the EventQueueMutex */
    while (!SDL_PollEvent(event12)) {
        SDL20_Delay(10);
    }

    return 1;
}

static SDL_bool
PushEventIfNotFiltered(SDL12_Event *event12)
{
    SDL_bool retval = SDL_FALSE;
    if (event12->type != SDL12_NOEVENT) {
        SDL_assert(EventQueueMutex != NULL);
        SDL20_LockMutex(EventQueueMutex);
        if (EventStates[event12->type] != SDL_IGNORE) {
            if ((!EventFilter12) || (EventFilter12(event12))) {
                retval = (SDL_PushEvent(event12) == 0)? SDL_TRUE : SDL_FALSE;
            }
        }
        SDL20_UnlockMutex(EventQueueMutex);
    }
    return retval;
}

DECLSPEC12 Uint8 SDLCALL
SDL_EventState(Uint8 type, int state)
{
    Uint8 retval = 0;
    /* the values of "state" match between 1.2 and 2.0 */
    if (EventQueueMutex) {
        SDL12_Event e;

        SDL20_LockMutex(EventQueueMutex);

        retval = EventStates[type];
        if (state != SDL_QUERY) {
            EventStates[type] = state;
            if ((type == SDL12_SYSWMEVENT) && SupportSysWM) {  /* we only enable syswm in SDL2 if it makes sense. */
                SDL20_EventState(SDL_SYSWMEVENT, state);
            }
        }
        if (state == SDL_IGNORE) {  /* drop existing events of this type. */
            while (SDL_PeepEvents(&e, 1, SDL_GETEVENT, (1<<type))) {
              /* nothing */ ;
            }
        }

        SDL20_UnlockMutex(EventQueueMutex);
    }

    return retval;
}

/* Calculates the Logical Scaling viewport based on a given window size.
   We pass the DPI-unscaled pixel size in when using this for rendering, and
   the DPI-scaled window size when using it to transform mouse coordinates. */
static SDL_Rect
GetOpenGLLogicalScalingViewport(int physical_width, int physical_height)
{
    float want_aspect, real_aspect;
    SDL_Rect dstrect;

    want_aspect = ((float) OpenGLLogicalScalingWidth) / ((float) OpenGLLogicalScalingHeight);
    real_aspect = ((float) physical_width) / ((float) physical_height);

    if (SDL20_fabs(want_aspect-real_aspect) < 0.0001) {
        /* The aspect ratios are the same, just scale appropriately */
        dstrect.x = 0;
        dstrect.y = 0;
        dstrect.w = physical_width;
        dstrect.h = physical_height;
    } else if (want_aspect > real_aspect) {
        /* We want a wider aspect ratio than is available - letterbox it */
        const float scale = ((float) physical_width) / OpenGLLogicalScalingWidth;
        dstrect.x = 0;
        dstrect.w = physical_width;
        dstrect.h = (int)SDL20_floor(OpenGLLogicalScalingHeight * scale);
        dstrect.y = (physical_height - dstrect.h) / 2;
    } else {
        /* We want a narrower aspect ratio than is available - use side-bars */
        const float scale = ((float)physical_height) / OpenGLLogicalScalingHeight;
        dstrect.y = 0;
        dstrect.h = physical_height;
        dstrect.w = (int)SDL20_floor(OpenGLLogicalScalingWidth * scale);
        dstrect.x = (physical_width - dstrect.w) / 2;
    }

    return dstrect;
}

/* Scale a point (e.g. absolute mouse position) to the logical scaling size */
static void
AdjustOpenGLLogicalScalingPoint(int *x, int *y)
{
    SDL_Rect viewport;
    int physical_w, physical_h;
    float scale_x, scale_y;
    int adjusted_x, adjusted_y;

    /* Don't adjust anything if we're not using Logical Scaling */
    if (!OpenGLLogicalScalingFBO || !VideoWindow20) {
        return;
    }

    /* we want to scale based on the window size, which is dpi-scaled */
    SDL20_GetWindowSize(VideoWindow20, &physical_w, &physical_h);
    viewport = GetOpenGLLogicalScalingViewport(physical_w, physical_h);

    scale_x = (float)OpenGLLogicalScalingWidth / viewport.w;
    scale_y = (float)OpenGLLogicalScalingHeight / viewport.h;

    adjusted_x = (int) ((*x - viewport.x) * scale_x);
    adjusted_y = (int) ((*y - viewport.y) * scale_y);

    /* Clamp the result to the visible window */
    *x = SDL_max(SDL_min(adjusted_x, OpenGLLogicalScalingWidth), 0);
    *y = SDL_max(SDL_min(adjusted_y, OpenGLLogicalScalingHeight), 0);
}

/* Scale a vector (e.g. relative mouse movement) to the logical scaling size */
static void
AdjustOpenGLLogicalScalingVector(int *x, int *y, float *rx, float *ry)
{
    SDL_Rect viewport;
    int physical_w, physical_h;
    float scale_x, scale_y;
    float float_x, float_y;
    float trunc_x, trunc_y;

    /* Don't adjust anything if we're not using Logical Scaling */
    if (!OpenGLLogicalScalingFBO || !VideoWindow20) {
        return;
    }

    /* we want to scale based on the window size, which is dpi-scaled */
    SDL20_GetWindowSize(VideoWindow20, &physical_w, &physical_h);
    viewport = GetOpenGLLogicalScalingViewport(physical_w, physical_h);

    scale_x = (float)OpenGLLogicalScalingWidth / viewport.w;
    scale_y = (float)OpenGLLogicalScalingHeight / viewport.h;

    float_x = *x * scale_x + (rx ? *rx : 0.f);
    float_y = *y * scale_y + (ry ? *ry : 0.f);

    trunc_x = SDL20_truncf(float_x);
    trunc_y = SDL20_truncf(float_y);

    *x = (int)trunc_x;
    *y = (int)trunc_y;

    if (rx) {
        *rx = float_x - trunc_x;
    }
    if (ry) {
        *ry = float_y - trunc_y;
    }
}

static Uint8 MouseButtonState20to12(const Uint32 state20)
{
    Uint8 retval = (state20 & 0x7);  /* left, right, and middle will match. */

    /* the X[12] buttons are different in 1.2; mousewheel was in the way. */
    if (state20 & SDL_BUTTON(SDL_BUTTON_X1)) {
        retval |= (1<<5);
    }
    if (state20 & SDL_BUTTON(SDL_BUTTON_X2)) {
        retval |= (1<<6);
    }

    return retval;
}

DECLSPEC12 Uint8 SDLCALL
SDL_GetMouseState(int *x, int *y)
{
    const Uint8 buttons = MouseButtonState20to12(SDL20_GetMouseState(x, y));
    if (x) { *x = MousePosition.x; }
    if (y) { *y = MousePosition.y; }
    return buttons;
}

DECLSPEC12 Uint8 SDLCALL
SDL_GetRelativeMouseState(int *x, int *y)
{
    return MouseButtonState20to12(SDL20_GetRelativeMouseState(x, y));
}

DECLSPEC12 char * SDLCALL
SDL_GetKeyName(SDL12Key key)
{
    switch (key) {
    #define CASESDLK12TONAME(k, n) case k: return (char *) n
    CASESDLK12TONAME(SDLK12_BACKSPACE, "backspace");
    CASESDLK12TONAME(SDLK12_TAB, "tab");
    CASESDLK12TONAME(SDLK12_CLEAR, "clear");
    CASESDLK12TONAME(SDLK12_RETURN, "return");
    CASESDLK12TONAME(SDLK12_PAUSE, "pause");
    CASESDLK12TONAME(SDLK12_ESCAPE, "escape");
    CASESDLK12TONAME(SDLK12_SPACE, "space");
    CASESDLK12TONAME(SDLK12_EXCLAIM, "!");
    CASESDLK12TONAME(SDLK12_QUOTEDBL, "\"");
    CASESDLK12TONAME(SDLK12_HASH, "#");
    CASESDLK12TONAME(SDLK12_DOLLAR, "$");
    CASESDLK12TONAME(SDLK12_AMPERSAND, "&");
    CASESDLK12TONAME(SDLK12_QUOTE, "'");
    CASESDLK12TONAME(SDLK12_LEFTPAREN, "(");
    CASESDLK12TONAME(SDLK12_RIGHTPAREN, ")");
    CASESDLK12TONAME(SDLK12_ASTERISK, "*");
    CASESDLK12TONAME(SDLK12_PLUS, "+");
    CASESDLK12TONAME(SDLK12_COMMA, ",");
    CASESDLK12TONAME(SDLK12_MINUS, "-");
    CASESDLK12TONAME(SDLK12_PERIOD, ".");
    CASESDLK12TONAME(SDLK12_SLASH, "/");
    CASESDLK12TONAME(SDLK12_0, "0");
    CASESDLK12TONAME(SDLK12_1, "1");
    CASESDLK12TONAME(SDLK12_2, "2");
    CASESDLK12TONAME(SDLK12_3, "3");
    CASESDLK12TONAME(SDLK12_4, "4");
    CASESDLK12TONAME(SDLK12_5, "5");
    CASESDLK12TONAME(SDLK12_6, "6");
    CASESDLK12TONAME(SDLK12_7, "7");
    CASESDLK12TONAME(SDLK12_8, "8");
    CASESDLK12TONAME(SDLK12_9, "9");
    CASESDLK12TONAME(SDLK12_COLON, ":");
    CASESDLK12TONAME(SDLK12_SEMICOLON, ");");
    CASESDLK12TONAME(SDLK12_LESS, "<");
    CASESDLK12TONAME(SDLK12_EQUALS, "=");
    CASESDLK12TONAME(SDLK12_GREATER, ">");
    CASESDLK12TONAME(SDLK12_QUESTION, "?");
    CASESDLK12TONAME(SDLK12_AT, "@");
    CASESDLK12TONAME(SDLK12_LEFTBRACKET, "[");
    CASESDLK12TONAME(SDLK12_BACKSLASH, "\\");
    CASESDLK12TONAME(SDLK12_RIGHTBRACKET, "]");
    CASESDLK12TONAME(SDLK12_CARET, "^");
    CASESDLK12TONAME(SDLK12_UNDERSCORE, "_");
    CASESDLK12TONAME(SDLK12_BACKQUOTE, "`");
    CASESDLK12TONAME(SDLK12_a, "a");
    CASESDLK12TONAME(SDLK12_b, "b");
    CASESDLK12TONAME(SDLK12_c, "c");
    CASESDLK12TONAME(SDLK12_d, "d");
    CASESDLK12TONAME(SDLK12_e, "e");
    CASESDLK12TONAME(SDLK12_f, "f");
    CASESDLK12TONAME(SDLK12_g, "g");
    CASESDLK12TONAME(SDLK12_h, "h");
    CASESDLK12TONAME(SDLK12_i, "i");
    CASESDLK12TONAME(SDLK12_j, "j");
    CASESDLK12TONAME(SDLK12_k, "k");
    CASESDLK12TONAME(SDLK12_l, "l");
    CASESDLK12TONAME(SDLK12_m, "m");
    CASESDLK12TONAME(SDLK12_n, "n");
    CASESDLK12TONAME(SDLK12_o, "o");
    CASESDLK12TONAME(SDLK12_p, "p");
    CASESDLK12TONAME(SDLK12_q, "q");
    CASESDLK12TONAME(SDLK12_r, "r");
    CASESDLK12TONAME(SDLK12_s, "s");
    CASESDLK12TONAME(SDLK12_t, "t");
    CASESDLK12TONAME(SDLK12_u, "u");
    CASESDLK12TONAME(SDLK12_v, "v");
    CASESDLK12TONAME(SDLK12_w, "w");
    CASESDLK12TONAME(SDLK12_x, "x");
    CASESDLK12TONAME(SDLK12_y, "y");
    CASESDLK12TONAME(SDLK12_z, "z");
    CASESDLK12TONAME(SDLK12_DELETE, "delete");

    CASESDLK12TONAME(SDLK12_WORLD_0, "world 0");
    CASESDLK12TONAME(SDLK12_WORLD_1, "world 1");
    CASESDLK12TONAME(SDLK12_WORLD_2, "world 2");
    CASESDLK12TONAME(SDLK12_WORLD_3, "world 3");
    CASESDLK12TONAME(SDLK12_WORLD_4, "world 4");
    CASESDLK12TONAME(SDLK12_WORLD_5, "world 5");
    CASESDLK12TONAME(SDLK12_WORLD_6, "world 6");
    CASESDLK12TONAME(SDLK12_WORLD_7, "world 7");
    CASESDLK12TONAME(SDLK12_WORLD_8, "world 8");
    CASESDLK12TONAME(SDLK12_WORLD_9, "world 9");
    CASESDLK12TONAME(SDLK12_WORLD_10, "world 10");
    CASESDLK12TONAME(SDLK12_WORLD_11, "world 11");
    CASESDLK12TONAME(SDLK12_WORLD_12, "world 12");
    CASESDLK12TONAME(SDLK12_WORLD_13, "world 13");
    CASESDLK12TONAME(SDLK12_WORLD_14, "world 14");
    CASESDLK12TONAME(SDLK12_WORLD_15, "world 15");
    CASESDLK12TONAME(SDLK12_WORLD_16, "world 16");
    CASESDLK12TONAME(SDLK12_WORLD_17, "world 17");
    CASESDLK12TONAME(SDLK12_WORLD_18, "world 18");
    CASESDLK12TONAME(SDLK12_WORLD_19, "world 19");
    CASESDLK12TONAME(SDLK12_WORLD_20, "world 20");
    CASESDLK12TONAME(SDLK12_WORLD_21, "world 21");
    CASESDLK12TONAME(SDLK12_WORLD_22, "world 22");
    CASESDLK12TONAME(SDLK12_WORLD_23, "world 23");
    CASESDLK12TONAME(SDLK12_WORLD_24, "world 24");
    CASESDLK12TONAME(SDLK12_WORLD_25, "world 25");
    CASESDLK12TONAME(SDLK12_WORLD_26, "world 26");
    CASESDLK12TONAME(SDLK12_WORLD_27, "world 27");
    CASESDLK12TONAME(SDLK12_WORLD_28, "world 28");
    CASESDLK12TONAME(SDLK12_WORLD_29, "world 29");
    CASESDLK12TONAME(SDLK12_WORLD_30, "world 30");
    CASESDLK12TONAME(SDLK12_WORLD_31, "world 31");
    CASESDLK12TONAME(SDLK12_WORLD_32, "world 32");
    CASESDLK12TONAME(SDLK12_WORLD_33, "world 33");
    CASESDLK12TONAME(SDLK12_WORLD_34, "world 34");
    CASESDLK12TONAME(SDLK12_WORLD_35, "world 35");
    CASESDLK12TONAME(SDLK12_WORLD_36, "world 36");
    CASESDLK12TONAME(SDLK12_WORLD_37, "world 37");
    CASESDLK12TONAME(SDLK12_WORLD_38, "world 38");
    CASESDLK12TONAME(SDLK12_WORLD_39, "world 39");
    CASESDLK12TONAME(SDLK12_WORLD_40, "world 40");
    CASESDLK12TONAME(SDLK12_WORLD_41, "world 41");
    CASESDLK12TONAME(SDLK12_WORLD_42, "world 42");
    CASESDLK12TONAME(SDLK12_WORLD_43, "world 43");
    CASESDLK12TONAME(SDLK12_WORLD_44, "world 44");
    CASESDLK12TONAME(SDLK12_WORLD_45, "world 45");
    CASESDLK12TONAME(SDLK12_WORLD_46, "world 46");
    CASESDLK12TONAME(SDLK12_WORLD_47, "world 47");
    CASESDLK12TONAME(SDLK12_WORLD_48, "world 48");
    CASESDLK12TONAME(SDLK12_WORLD_49, "world 49");
    CASESDLK12TONAME(SDLK12_WORLD_50, "world 50");
    CASESDLK12TONAME(SDLK12_WORLD_51, "world 51");
    CASESDLK12TONAME(SDLK12_WORLD_52, "world 52");
    CASESDLK12TONAME(SDLK12_WORLD_53, "world 53");
    CASESDLK12TONAME(SDLK12_WORLD_54, "world 54");
    CASESDLK12TONAME(SDLK12_WORLD_55, "world 55");
    CASESDLK12TONAME(SDLK12_WORLD_56, "world 56");
    CASESDLK12TONAME(SDLK12_WORLD_57, "world 57");
    CASESDLK12TONAME(SDLK12_WORLD_58, "world 58");
    CASESDLK12TONAME(SDLK12_WORLD_59, "world 59");
    CASESDLK12TONAME(SDLK12_WORLD_60, "world 60");
    CASESDLK12TONAME(SDLK12_WORLD_61, "world 61");
    CASESDLK12TONAME(SDLK12_WORLD_62, "world 62");
    CASESDLK12TONAME(SDLK12_WORLD_63, "world 63");
    CASESDLK12TONAME(SDLK12_WORLD_64, "world 64");
    CASESDLK12TONAME(SDLK12_WORLD_65, "world 65");
    CASESDLK12TONAME(SDLK12_WORLD_66, "world 66");
    CASESDLK12TONAME(SDLK12_WORLD_67, "world 67");
    CASESDLK12TONAME(SDLK12_WORLD_68, "world 68");
    CASESDLK12TONAME(SDLK12_WORLD_69, "world 69");
    CASESDLK12TONAME(SDLK12_WORLD_70, "world 70");
    CASESDLK12TONAME(SDLK12_WORLD_71, "world 71");
    CASESDLK12TONAME(SDLK12_WORLD_72, "world 72");
    CASESDLK12TONAME(SDLK12_WORLD_73, "world 73");
    CASESDLK12TONAME(SDLK12_WORLD_74, "world 74");
    CASESDLK12TONAME(SDLK12_WORLD_75, "world 75");
    CASESDLK12TONAME(SDLK12_WORLD_76, "world 76");
    CASESDLK12TONAME(SDLK12_WORLD_77, "world 77");
    CASESDLK12TONAME(SDLK12_WORLD_78, "world 78");
    CASESDLK12TONAME(SDLK12_WORLD_79, "world 79");
    CASESDLK12TONAME(SDLK12_WORLD_80, "world 80");
    CASESDLK12TONAME(SDLK12_WORLD_81, "world 81");
    CASESDLK12TONAME(SDLK12_WORLD_82, "world 82");
    CASESDLK12TONAME(SDLK12_WORLD_83, "world 83");
    CASESDLK12TONAME(SDLK12_WORLD_84, "world 84");
    CASESDLK12TONAME(SDLK12_WORLD_85, "world 85");
    CASESDLK12TONAME(SDLK12_WORLD_86, "world 86");
    CASESDLK12TONAME(SDLK12_WORLD_87, "world 87");
    CASESDLK12TONAME(SDLK12_WORLD_88, "world 88");
    CASESDLK12TONAME(SDLK12_WORLD_89, "world 89");
    CASESDLK12TONAME(SDLK12_WORLD_90, "world 90");
    CASESDLK12TONAME(SDLK12_WORLD_91, "world 91");
    CASESDLK12TONAME(SDLK12_WORLD_92, "world 92");
    CASESDLK12TONAME(SDLK12_WORLD_93, "world 93");
    CASESDLK12TONAME(SDLK12_WORLD_94, "world 94");
    CASESDLK12TONAME(SDLK12_WORLD_95, "world 95");

    CASESDLK12TONAME(SDLK12_KP0, "[0]");
    CASESDLK12TONAME(SDLK12_KP1, "[1]");
    CASESDLK12TONAME(SDLK12_KP2, "[2]");
    CASESDLK12TONAME(SDLK12_KP3, "[3]");
    CASESDLK12TONAME(SDLK12_KP4, "[4]");
    CASESDLK12TONAME(SDLK12_KP5, "[5]");
    CASESDLK12TONAME(SDLK12_KP6, "[6]");
    CASESDLK12TONAME(SDLK12_KP7, "[7]");
    CASESDLK12TONAME(SDLK12_KP8, "[8]");
    CASESDLK12TONAME(SDLK12_KP9, "[9]");
    CASESDLK12TONAME(SDLK12_KP_PERIOD, "[.]");
    CASESDLK12TONAME(SDLK12_KP_DIVIDE, "[/]");
    CASESDLK12TONAME(SDLK12_KP_MULTIPLY, "[*]");
    CASESDLK12TONAME(SDLK12_KP_MINUS, "[-]");
    CASESDLK12TONAME(SDLK12_KP_PLUS, "[+]");
    CASESDLK12TONAME(SDLK12_KP_ENTER, "enter");
    CASESDLK12TONAME(SDLK12_KP_EQUALS, "equals");

    CASESDLK12TONAME(SDLK12_UP, "up");
    CASESDLK12TONAME(SDLK12_DOWN, "down");
    CASESDLK12TONAME(SDLK12_RIGHT, "right");
    CASESDLK12TONAME(SDLK12_LEFT, "left");
    CASESDLK12TONAME(SDLK12_INSERT, "insert");
    CASESDLK12TONAME(SDLK12_HOME, "home");
    CASESDLK12TONAME(SDLK12_END, "end");
    CASESDLK12TONAME(SDLK12_PAGEUP, "page up");
    CASESDLK12TONAME(SDLK12_PAGEDOWN, "page down");

    CASESDLK12TONAME(SDLK12_F1, "f1");
    CASESDLK12TONAME(SDLK12_F2, "f2");
    CASESDLK12TONAME(SDLK12_F3, "f3");
    CASESDLK12TONAME(SDLK12_F4, "f4");
    CASESDLK12TONAME(SDLK12_F5, "f5");
    CASESDLK12TONAME(SDLK12_F6, "f6");
    CASESDLK12TONAME(SDLK12_F7, "f7");
    CASESDLK12TONAME(SDLK12_F8, "f8");
    CASESDLK12TONAME(SDLK12_F9, "f9");
    CASESDLK12TONAME(SDLK12_F10, "f10");
    CASESDLK12TONAME(SDLK12_F11, "f11");
    CASESDLK12TONAME(SDLK12_F12, "f12");
    CASESDLK12TONAME(SDLK12_F13, "f13");
    CASESDLK12TONAME(SDLK12_F14, "f14");
    CASESDLK12TONAME(SDLK12_F15, "f15");

    CASESDLK12TONAME(SDLK12_NUMLOCK, "numlock");
    CASESDLK12TONAME(SDLK12_CAPSLOCK, "caps lock");
    CASESDLK12TONAME(SDLK12_SCROLLOCK, "scroll lock");
    CASESDLK12TONAME(SDLK12_RSHIFT, "right shift");
    CASESDLK12TONAME(SDLK12_LSHIFT, "left shift");
    CASESDLK12TONAME(SDLK12_RCTRL, "right ctrl");
    CASESDLK12TONAME(SDLK12_LCTRL, "left ctrl");
    CASESDLK12TONAME(SDLK12_RALT, "right alt");
    CASESDLK12TONAME(SDLK12_LALT, "left alt");
    CASESDLK12TONAME(SDLK12_RMETA, "right meta");
    CASESDLK12TONAME(SDLK12_LMETA, "left meta");
    CASESDLK12TONAME(SDLK12_LSUPER, "left super"); /* "Windows" keys */
    CASESDLK12TONAME(SDLK12_RSUPER, "right super");
    CASESDLK12TONAME(SDLK12_MODE, "alt gr");
    CASESDLK12TONAME(SDLK12_COMPOSE, "compose");

    CASESDLK12TONAME(SDLK12_HELP, "help");
    CASESDLK12TONAME(SDLK12_PRINT, "print screen");
    CASESDLK12TONAME(SDLK12_SYSREQ, "sys req");
    CASESDLK12TONAME(SDLK12_BREAK, "break");
    CASESDLK12TONAME(SDLK12_MENU, "menu");
    CASESDLK12TONAME(SDLK12_POWER, "power");
    CASESDLK12TONAME(SDLK12_EURO, "euro");
    CASESDLK12TONAME(SDLK12_UNDO, "undo");
    #undef CASESDLK12TONAME
    default: break;
    }

    return (char *) "unknown key";
}

static SDL12Key
Keysym20to12(const SDL_Keycode keysym20)
{
    if (((int) keysym20) <= 255) {
        /* (most of) low-ASCII maps directly,
         * and so does the Latin-1 range (128-255) */
        if (keysym20 == SDLK_PAUSE) {
            return SDLK12_PAUSE;
        } else if (keysym20 == SDLK_CLEAR) {
            return SDLK12_CLEAR;
        }
        return (SDL12Key) keysym20;
    }

    switch (keysym20) {
    #define CASEKEYSYM20TO12(k20, k12) case SDLK_##k20: return SDLK12_##k12
    CASEKEYSYM20TO12(KP_0, KP0);
    CASEKEYSYM20TO12(KP_1, KP1);
    CASEKEYSYM20TO12(KP_2, KP2);
    CASEKEYSYM20TO12(KP_3, KP3);
    CASEKEYSYM20TO12(KP_4, KP4);
    CASEKEYSYM20TO12(KP_5, KP5);
    CASEKEYSYM20TO12(KP_6, KP6);
    CASEKEYSYM20TO12(KP_7, KP7);
    CASEKEYSYM20TO12(KP_8, KP8);
    CASEKEYSYM20TO12(KP_9, KP9);
    CASEKEYSYM20TO12(NUMLOCKCLEAR, NUMLOCK);
    CASEKEYSYM20TO12(SCROLLLOCK, SCROLLOCK);
    CASEKEYSYM20TO12(RGUI, RMETA);
    CASEKEYSYM20TO12(LGUI, LMETA);
    CASEKEYSYM20TO12(PRINTSCREEN, PRINT);
    #undef CASEKEYSYM20TO12

    #define CASEKEYSYM20TO12(k) case SDLK_##k: return SDLK12_##k
    CASEKEYSYM20TO12(CLEAR);
    CASEKEYSYM20TO12(PAUSE);
    CASEKEYSYM20TO12(KP_PERIOD);
    CASEKEYSYM20TO12(KP_DIVIDE);
    CASEKEYSYM20TO12(KP_MULTIPLY);
    CASEKEYSYM20TO12(KP_MINUS);
    CASEKEYSYM20TO12(KP_PLUS);
    CASEKEYSYM20TO12(KP_ENTER);
    CASEKEYSYM20TO12(KP_EQUALS);
    CASEKEYSYM20TO12(UP);
    CASEKEYSYM20TO12(DOWN);
    CASEKEYSYM20TO12(RIGHT);
    CASEKEYSYM20TO12(LEFT);
    CASEKEYSYM20TO12(INSERT);
    CASEKEYSYM20TO12(HOME);
    CASEKEYSYM20TO12(END);
    CASEKEYSYM20TO12(PAGEUP);
    CASEKEYSYM20TO12(PAGEDOWN);
    CASEKEYSYM20TO12(F1);
    CASEKEYSYM20TO12(F2);
    CASEKEYSYM20TO12(F3);
    CASEKEYSYM20TO12(F4);
    CASEKEYSYM20TO12(F5);
    CASEKEYSYM20TO12(F6);
    CASEKEYSYM20TO12(F7);
    CASEKEYSYM20TO12(F8);
    CASEKEYSYM20TO12(F9);
    CASEKEYSYM20TO12(F10);
    CASEKEYSYM20TO12(F11);
    CASEKEYSYM20TO12(F12);
    CASEKEYSYM20TO12(F13);
    CASEKEYSYM20TO12(F14);
    CASEKEYSYM20TO12(F15);
    CASEKEYSYM20TO12(CAPSLOCK);
    CASEKEYSYM20TO12(RSHIFT);
    CASEKEYSYM20TO12(LSHIFT);
    CASEKEYSYM20TO12(RCTRL);
    CASEKEYSYM20TO12(LCTRL);
    CASEKEYSYM20TO12(RALT);
    CASEKEYSYM20TO12(LALT);
    CASEKEYSYM20TO12(MODE);
    CASEKEYSYM20TO12(HELP);
    CASEKEYSYM20TO12(SYSREQ);
    CASEKEYSYM20TO12(MENU);
    CASEKEYSYM20TO12(POWER);
    CASEKEYSYM20TO12(UNDO);
    #undef CASEKEYSYM20TO12

    /* Map the "World Keys" (SDLK_WORLD_0 = 0xA0 to SDLK_WORLD_95 = 0xFF).
     * In SDL2 they're the UCS-4 (32bit unicode) code for the key,
     * in SDL1.2 (on X11 at least) they were the X11_KeySym & 0xFF,
     * if (X11_KeySym >> 8)) was either 0 to 8 or 0x0A or 0x0C or 0x0E
     * So map all those used UCS-4 codes to the corresponding SDLK12_WORLD_* codes
     * Note that the Latin-1 range (keysym20 <= 255) is already handled at the top of this function
     * Luckily X11's keysymdef.h lists both the values of the constants and their UCS4-value, like
     * #define XK_Aogonek                       0x01a1  / * U+0104 LATIN CAPITAL LETTER A WITH OGONEK * /
     * So I'm using that as a reference for our mappings, which only use
     * the lowest byte of the XK_* value, because of X11_KeySym & 0xFF in SDL1.2
     *
     * case UCS4_code: return (SDL12Key)lowest_byte_of_corresponding_X11_KeySym; */

    /* Latin-2 */
    case 0x0104: return (SDL12Key)0xa1;
    case 0x02D8: return (SDL12Key)0xa2;
    case 0x0141: return (SDL12Key)0xa3;
    case 0x013D: return (SDL12Key)0xa5;
    case 0x015A: return (SDL12Key)0xa6;
    case 0x0160: return (SDL12Key)0xa9;
    case 0x015E: return (SDL12Key)0xaa;
    case 0x0164: return (SDL12Key)0xab;
    case 0x0179: return (SDL12Key)0xac;
    case 0x017D: return (SDL12Key)0xae;
    case 0x017B: return (SDL12Key)0xaf;
    case 0x0105: return (SDL12Key)0xb1;
    case 0x02DB: return (SDL12Key)0xb2;
    case 0x0142: return (SDL12Key)0xb3;
    case 0x013E: return (SDL12Key)0xb5;
    case 0x015B: return (SDL12Key)0xb6;
    case 0x02C7: return (SDL12Key)0xb7;
    case 0x0161: return (SDL12Key)0xb9;
    case 0x015F: return (SDL12Key)0xba;
    case 0x0165: return (SDL12Key)0xbb;
    case 0x017A: return (SDL12Key)0xbc;
    case 0x02DD: return (SDL12Key)0xbd;
    case 0x017E: return (SDL12Key)0xbe;
    case 0x017C: return (SDL12Key)0xbf;
    case 0x0154: return (SDL12Key)0xc0;
    case 0x0102: return (SDL12Key)0xc3;
    case 0x0139: return (SDL12Key)0xc5;
    case 0x0106: return (SDL12Key)0xc6;
    case 0x010C: return (SDL12Key)0xc8;
    case 0x0118: return (SDL12Key)0xca;
    case 0x011A: return (SDL12Key)0xcc;
    case 0x010E: return (SDL12Key)0xcf;
    case 0x0110: return (SDL12Key)0xd0;
    case 0x0143: return (SDL12Key)0xd1;
    case 0x0147: return (SDL12Key)0xd2;
    case 0x0150: return (SDL12Key)0xd5;
    case 0x0158: return (SDL12Key)0xd8;
    case 0x016E: return (SDL12Key)0xd9;
    case 0x0170: return (SDL12Key)0xdb;
    case 0x0162: return (SDL12Key)0xde;
    case 0x0155: return (SDL12Key)0xe0;
    case 0x0103: return (SDL12Key)0xe3;
    case 0x013A: return (SDL12Key)0xe5;
    case 0x0107: return (SDL12Key)0xe6;
    case 0x010D: return (SDL12Key)0xe8;
    case 0x0119: return (SDL12Key)0xea;
    case 0x011B: return (SDL12Key)0xec;
    case 0x010F: return (SDL12Key)0xef;
    case 0x0111: return (SDL12Key)0xf0;
    case 0x0144: return (SDL12Key)0xf1;
    case 0x0148: return (SDL12Key)0xf2;
    case 0x0151: return (SDL12Key)0xf5;
    case 0x0159: return (SDL12Key)0xf8;
    case 0x016F: return (SDL12Key)0xf9;
    case 0x0171: return (SDL12Key)0xfb;
    case 0x0163: return (SDL12Key)0xfe;
    case 0x02D9: return (SDL12Key)0xff;
    /* Latin-3 */
    case 0x0126: return (SDL12Key)0xa1;
    case 0x0124: return (SDL12Key)0xa6;
    case 0x0130: return (SDL12Key)0xa9;
    case 0x011E: return (SDL12Key)0xab;
    case 0x0134: return (SDL12Key)0xac;
    case 0x0127: return (SDL12Key)0xb1;
    case 0x0125: return (SDL12Key)0xb6;
    case 0x0131: return (SDL12Key)0xb9;
    case 0x011F: return (SDL12Key)0xbb;
    case 0x0135: return (SDL12Key)0xbc;
    case 0x010A: return (SDL12Key)0xc5;
    case 0x0108: return (SDL12Key)0xc6;
    case 0x0120: return (SDL12Key)0xd5;
    case 0x011C: return (SDL12Key)0xd8;
    case 0x016C: return (SDL12Key)0xdd;
    case 0x015C: return (SDL12Key)0xde;
    case 0x010B: return (SDL12Key)0xe5;
    case 0x0109: return (SDL12Key)0xe6;
    case 0x0121: return (SDL12Key)0xf5;
    case 0x011D: return (SDL12Key)0xf8;
    case 0x016D: return (SDL12Key)0xfd;
    case 0x015D: return (SDL12Key)0xfe;
    /* Latin 4 */
    case 0x0138: return (SDL12Key)0xa2;
    case 0x0156: return (SDL12Key)0xa3;
    case 0x0128: return (SDL12Key)0xa5;
    case 0x013B: return (SDL12Key)0xa6;
    case 0x0112: return (SDL12Key)0xaa;
    case 0x0122: return (SDL12Key)0xab;
    case 0x0166: return (SDL12Key)0xac;
    case 0x0157: return (SDL12Key)0xb3;
    case 0x0129: return (SDL12Key)0xb5;
    case 0x013C: return (SDL12Key)0xb6;
    case 0x0113: return (SDL12Key)0xba;
    case 0x0123: return (SDL12Key)0xbb;
    case 0x0167: return (SDL12Key)0xbc;
    case 0x014A: return (SDL12Key)0xbd;
    case 0x014B: return (SDL12Key)0xbf;
    case 0x0100: return (SDL12Key)0xc0;
    case 0x012E: return (SDL12Key)0xc7;
    case 0x0116: return (SDL12Key)0xcc;
    case 0x012A: return (SDL12Key)0xcf;
    case 0x0145: return (SDL12Key)0xd1;
    case 0x014C: return (SDL12Key)0xd2;
    case 0x0136: return (SDL12Key)0xd3;
    case 0x0172: return (SDL12Key)0xd9;
    case 0x0168: return (SDL12Key)0xdd;
    case 0x016A: return (SDL12Key)0xde;
    case 0x0101: return (SDL12Key)0xe0;
    case 0x012F: return (SDL12Key)0xe7;
    case 0x0117: return (SDL12Key)0xec;
    case 0x012B: return (SDL12Key)0xef;
    case 0x0146: return (SDL12Key)0xf1;
    case 0x014D: return (SDL12Key)0xf2;
    case 0x0137: return (SDL12Key)0xf3;
    case 0x0173: return (SDL12Key)0xf9;
    case 0x0169: return (SDL12Key)0xfd;
    case 0x016B: return (SDL12Key)0xfe;
    /* Katakana */
    case 0x203E: return (SDL12Key)0x7e;
    case 0x3002: return (SDL12Key)0xa1;
    case 0x300C: return (SDL12Key)0xa2;
    case 0x300D: return (SDL12Key)0xa3;
    case 0x3001: return (SDL12Key)0xa4;
    case 0x30FB: return (SDL12Key)0xa5;
    case 0x30F2: return (SDL12Key)0xa6;
    case 0x30A1: return (SDL12Key)0xa7;
    case 0x30A3: return (SDL12Key)0xa8;
    case 0x30A5: return (SDL12Key)0xa9;
    case 0x30A7: return (SDL12Key)0xaa;
    case 0x30A9: return (SDL12Key)0xab;
    case 0x30E3: return (SDL12Key)0xac;
    case 0x30E5: return (SDL12Key)0xad;
    case 0x30E7: return (SDL12Key)0xae;
    case 0x30C3: return (SDL12Key)0xaf;
    case 0x30FC: return (SDL12Key)0xb0;
    case 0x30A2: return (SDL12Key)0xb1;
    case 0x30A4: return (SDL12Key)0xb2;
    case 0x30A6: return (SDL12Key)0xb3;
    case 0x30A8: return (SDL12Key)0xb4;
    case 0x30AA: return (SDL12Key)0xb5;
    case 0x30AB: return (SDL12Key)0xb6;
    case 0x30AD: return (SDL12Key)0xb7;
    case 0x30AF: return (SDL12Key)0xb8;
    case 0x30B1: return (SDL12Key)0xb9;
    case 0x30B3: return (SDL12Key)0xba;
    case 0x30B5: return (SDL12Key)0xbb;
    case 0x30B7: return (SDL12Key)0xbc;
    case 0x30B9: return (SDL12Key)0xbd;
    case 0x30BB: return (SDL12Key)0xbe;
    case 0x30BD: return (SDL12Key)0xbf;
    case 0x30BF: return (SDL12Key)0xc0;
    case 0x30C1: return (SDL12Key)0xc1;
    case 0x30C4: return (SDL12Key)0xc2;
    case 0x30C6: return (SDL12Key)0xc3;
    case 0x30C8: return (SDL12Key)0xc4;
    case 0x30CA: return (SDL12Key)0xc5;
    case 0x30CB: return (SDL12Key)0xc6;
    case 0x30CC: return (SDL12Key)0xc7;
    case 0x30CD: return (SDL12Key)0xc8;
    case 0x30CE: return (SDL12Key)0xc9;
    case 0x30CF: return (SDL12Key)0xca;
    case 0x30D2: return (SDL12Key)0xcb;
    case 0x30D5: return (SDL12Key)0xcc;
    case 0x30D8: return (SDL12Key)0xcd;
    case 0x30DB: return (SDL12Key)0xce;
    case 0x30DE: return (SDL12Key)0xcf;
    case 0x30DF: return (SDL12Key)0xd0;
    case 0x30E0: return (SDL12Key)0xd1;
    case 0x30E1: return (SDL12Key)0xd2;
    case 0x30E2: return (SDL12Key)0xd3;
    case 0x30E4: return (SDL12Key)0xd4;
    case 0x30E6: return (SDL12Key)0xd5;
    case 0x30E8: return (SDL12Key)0xd6;
    case 0x30E9: return (SDL12Key)0xd7;
    case 0x30EA: return (SDL12Key)0xd8;
    case 0x30EB: return (SDL12Key)0xd9;
    case 0x30EC: return (SDL12Key)0xda;
    case 0x30ED: return (SDL12Key)0xdb;
    case 0x30EF: return (SDL12Key)0xdc;
    case 0x30F3: return (SDL12Key)0xdd;
    case 0x309B: return (SDL12Key)0xde;
    case 0x309C: return (SDL12Key)0xdf;
    /* Arabic */
    case 0x060C: return (SDL12Key)0xac;
    case 0x061B: return (SDL12Key)0xbb;
    case 0x061F: return (SDL12Key)0xbf;
    case 0x0621: return (SDL12Key)0xc1;
    case 0x0622: return (SDL12Key)0xc2;
    case 0x0623: return (SDL12Key)0xc3;
    case 0x0624: return (SDL12Key)0xc4;
    case 0x0625: return (SDL12Key)0xc5;
    case 0x0626: return (SDL12Key)0xc6;
    case 0x0627: return (SDL12Key)0xc7;
    case 0x0628: return (SDL12Key)0xc8;
    case 0x0629: return (SDL12Key)0xc9;
    case 0x062A: return (SDL12Key)0xca;
    case 0x062B: return (SDL12Key)0xcb;
    case 0x062C: return (SDL12Key)0xcc;
    case 0x062D: return (SDL12Key)0xcd;
    case 0x062E: return (SDL12Key)0xce;
    case 0x062F: return (SDL12Key)0xcf;
    case 0x0630: return (SDL12Key)0xd0;
    case 0x0631: return (SDL12Key)0xd1;
    case 0x0632: return (SDL12Key)0xd2;
    case 0x0633: return (SDL12Key)0xd3;
    case 0x0634: return (SDL12Key)0xd4;
    case 0x0635: return (SDL12Key)0xd5;
    case 0x0636: return (SDL12Key)0xd6;
    case 0x0637: return (SDL12Key)0xd7;
    case 0x0638: return (SDL12Key)0xd8;
    case 0x0639: return (SDL12Key)0xd9;
    case 0x063A: return (SDL12Key)0xda;
    case 0x0640: return (SDL12Key)0xe0;
    case 0x0641: return (SDL12Key)0xe1;
    case 0x0642: return (SDL12Key)0xe2;
    case 0x0643: return (SDL12Key)0xe3;
    case 0x0644: return (SDL12Key)0xe4;
    case 0x0645: return (SDL12Key)0xe5;
    case 0x0646: return (SDL12Key)0xe6;
    case 0x0647: return (SDL12Key)0xe7;
    case 0x0648: return (SDL12Key)0xe8;
    case 0x0649: return (SDL12Key)0xe9;
    case 0x064A: return (SDL12Key)0xea;
    case 0x064B: return (SDL12Key)0xeb;
    case 0x064C: return (SDL12Key)0xec;
    case 0x064D: return (SDL12Key)0xed;
    case 0x064E: return (SDL12Key)0xee;
    case 0x064F: return (SDL12Key)0xef;
    case 0x0650: return (SDL12Key)0xf0;
    case 0x0651: return (SDL12Key)0xf1;
    case 0x0652: return (SDL12Key)0xf2;
    /* Cyrillic */
    case 0x0452: return (SDL12Key)0xa1;
    case 0x0453: return (SDL12Key)0xa2;
    case 0x0451: return (SDL12Key)0xa3;
    case 0x0454: return (SDL12Key)0xa4;
    case 0x0455: return (SDL12Key)0xa5;
    case 0x0456: return (SDL12Key)0xa6;
    case 0x0457: return (SDL12Key)0xa7;
    case 0x0458: return (SDL12Key)0xa8;
    case 0x0459: return (SDL12Key)0xa9;
    case 0x045A: return (SDL12Key)0xaa;
    case 0x045B: return (SDL12Key)0xab;
    case 0x045C: return (SDL12Key)0xac;
    case 0x0491: return (SDL12Key)0xad;
    case 0x045E: return (SDL12Key)0xae;
    case 0x045F: return (SDL12Key)0xaf;
    case 0x2116: return (SDL12Key)0xb0;
    case 0x0402: return (SDL12Key)0xb1;
    case 0x0403: return (SDL12Key)0xb2;
    case 0x0401: return (SDL12Key)0xb3;
    case 0x0404: return (SDL12Key)0xb4;
    case 0x0405: return (SDL12Key)0xb5;
    case 0x0406: return (SDL12Key)0xb6;
    case 0x0407: return (SDL12Key)0xb7;
    case 0x0408: return (SDL12Key)0xb8;
    case 0x0409: return (SDL12Key)0xb9;
    case 0x040A: return (SDL12Key)0xba;
    case 0x040B: return (SDL12Key)0xbb;
    case 0x040C: return (SDL12Key)0xbc;
    case 0x0490: return (SDL12Key)0xbd;
    case 0x040E: return (SDL12Key)0xbe;
    case 0x040F: return (SDL12Key)0xbf;
    case 0x044E: return (SDL12Key)0xc0;
    case 0x0430: return (SDL12Key)0xc1;
    case 0x0431: return (SDL12Key)0xc2;
    case 0x0446: return (SDL12Key)0xc3;
    case 0x0434: return (SDL12Key)0xc4;
    case 0x0435: return (SDL12Key)0xc5;
    case 0x0444: return (SDL12Key)0xc6;
    case 0x0433: return (SDL12Key)0xc7;
    case 0x0445: return (SDL12Key)0xc8;
    case 0x0438: return (SDL12Key)0xc9;
    case 0x0439: return (SDL12Key)0xca;
    case 0x043A: return (SDL12Key)0xcb;
    case 0x043B: return (SDL12Key)0xcc;
    case 0x043C: return (SDL12Key)0xcd;
    case 0x043D: return (SDL12Key)0xce;
    case 0x043E: return (SDL12Key)0xcf;
    case 0x043F: return (SDL12Key)0xd0;
    case 0x044F: return (SDL12Key)0xd1;
    case 0x0440: return (SDL12Key)0xd2;
    case 0x0441: return (SDL12Key)0xd3;
    case 0x0442: return (SDL12Key)0xd4;
    case 0x0443: return (SDL12Key)0xd5;
    case 0x0436: return (SDL12Key)0xd6;
    case 0x0432: return (SDL12Key)0xd7;
    case 0x044C: return (SDL12Key)0xd8;
    case 0x044B: return (SDL12Key)0xd9;
    case 0x0437: return (SDL12Key)0xda;
    case 0x0448: return (SDL12Key)0xdb;
    case 0x044D: return (SDL12Key)0xdc;
    case 0x0449: return (SDL12Key)0xdd;
    case 0x0447: return (SDL12Key)0xde;
    case 0x044A: return (SDL12Key)0xdf;
    case 0x042E: return (SDL12Key)0xe0;
    case 0x0410: return (SDL12Key)0xe1;
    case 0x0411: return (SDL12Key)0xe2;
    case 0x0426: return (SDL12Key)0xe3;
    case 0x0414: return (SDL12Key)0xe4;
    case 0x0415: return (SDL12Key)0xe5;
    case 0x0424: return (SDL12Key)0xe6;
    case 0x0413: return (SDL12Key)0xe7;
    case 0x0425: return (SDL12Key)0xe8;
    case 0x0418: return (SDL12Key)0xe9;
    case 0x0419: return (SDL12Key)0xea;
    case 0x041A: return (SDL12Key)0xeb;
    case 0x041B: return (SDL12Key)0xec;
    case 0x041C: return (SDL12Key)0xed;
    case 0x041D: return (SDL12Key)0xee;
    case 0x041E: return (SDL12Key)0xef;
    case 0x041F: return (SDL12Key)0xf0;
    case 0x042F: return (SDL12Key)0xf1;
    case 0x0420: return (SDL12Key)0xf2;
    case 0x0421: return (SDL12Key)0xf3;
    case 0x0422: return (SDL12Key)0xf4;
    case 0x0423: return (SDL12Key)0xf5;
    case 0x0416: return (SDL12Key)0xf6;
    case 0x0412: return (SDL12Key)0xf7;
    case 0x042C: return (SDL12Key)0xf8;
    case 0x042B: return (SDL12Key)0xf9;
    case 0x0417: return (SDL12Key)0xfa;
    case 0x0428: return (SDL12Key)0xfb;
    case 0x042D: return (SDL12Key)0xfc;
    case 0x0429: return (SDL12Key)0xfd;
    case 0x0427: return (SDL12Key)0xfe;
    case 0x042A: return (SDL12Key)0xff;
    /* Greek */
    case 0x0386: return (SDL12Key)0xa1;
    case 0x0388: return (SDL12Key)0xa2;
    case 0x0389: return (SDL12Key)0xa3;
    case 0x038A: return (SDL12Key)0xa4;
    case 0x03AA: return (SDL12Key)0xa5;
    case 0x038C: return (SDL12Key)0xa7;
    case 0x038E: return (SDL12Key)0xa8;
    case 0x03AB: return (SDL12Key)0xa9;
    case 0x038F: return (SDL12Key)0xab;
    case 0x0385: return (SDL12Key)0xae;
    case 0x2015: return (SDL12Key)0xaf;
    case 0x03AC: return (SDL12Key)0xb1;
    case 0x03AD: return (SDL12Key)0xb2;
    case 0x03AE: return (SDL12Key)0xb3;
    case 0x03AF: return (SDL12Key)0xb4;
    case 0x03CA: return (SDL12Key)0xb5;
    case 0x0390: return (SDL12Key)0xb6;
    case 0x03CC: return (SDL12Key)0xb7;
    case 0x03CD: return (SDL12Key)0xb8;
    case 0x03CB: return (SDL12Key)0xb9;
    case 0x03B0: return (SDL12Key)0xba;
    case 0x03CE: return (SDL12Key)0xbb;
    case 0x0391: return (SDL12Key)0xc1;
    case 0x0392: return (SDL12Key)0xc2;
    case 0x0393: return (SDL12Key)0xc3;
    case 0x0394: return (SDL12Key)0xc4;
    case 0x0395: return (SDL12Key)0xc5;
    case 0x0396: return (SDL12Key)0xc6;
    case 0x0397: return (SDL12Key)0xc7;
    case 0x0398: return (SDL12Key)0xc8;
    case 0x0399: return (SDL12Key)0xc9;
    case 0x039A: return (SDL12Key)0xca;
    case 0x039B: return (SDL12Key)0xcb;
    case 0x039C: return (SDL12Key)0xcc;
    case 0x039D: return (SDL12Key)0xcd;
    case 0x039E: return (SDL12Key)0xce;
    case 0x039F: return (SDL12Key)0xcf;
    case 0x03A0: return (SDL12Key)0xd0;
    case 0x03A1: return (SDL12Key)0xd1;
    case 0x03A3: return (SDL12Key)0xd2;
    case 0x03A4: return (SDL12Key)0xd4;
    case 0x03A5: return (SDL12Key)0xd5;
    case 0x03A6: return (SDL12Key)0xd6;
    case 0x03A7: return (SDL12Key)0xd7;
    case 0x03A8: return (SDL12Key)0xd8;
    case 0x03A9: return (SDL12Key)0xd9;
    case 0x03B1: return (SDL12Key)0xe1;
    case 0x03B2: return (SDL12Key)0xe2;
    case 0x03B3: return (SDL12Key)0xe3;
    case 0x03B4: return (SDL12Key)0xe4;
    case 0x03B5: return (SDL12Key)0xe5;
    case 0x03B6: return (SDL12Key)0xe6;
    case 0x03B7: return (SDL12Key)0xe7;
    case 0x03B8: return (SDL12Key)0xe8;
    case 0x03B9: return (SDL12Key)0xe9;
    case 0x03BA: return (SDL12Key)0xea;
    case 0x03BB: return (SDL12Key)0xeb;
    case 0x03BC: return (SDL12Key)0xec;
    case 0x03BD: return (SDL12Key)0xed;
    case 0x03BE: return (SDL12Key)0xee;
    case 0x03BF: return (SDL12Key)0xef;
    case 0x03C0: return (SDL12Key)0xf0;
    case 0x03C1: return (SDL12Key)0xf1;
    case 0x03C3: return (SDL12Key)0xf2;
    case 0x03C2: return (SDL12Key)0xf3;
    case 0x03C4: return (SDL12Key)0xf4;
    case 0x03C5: return (SDL12Key)0xf5;
    case 0x03C6: return (SDL12Key)0xf6;
    case 0x03C7: return (SDL12Key)0xf7;
    case 0x03C8: return (SDL12Key)0xf8;
    case 0x03C9: return (SDL12Key)0xf9;
    /* Technical */
    case 0x23B7: return (SDL12Key)0xa1;
    case 0x250C: return (SDL12Key)0xa2;
    case 0x2500: return (SDL12Key)0xa3;
    case 0x2320: return (SDL12Key)0xa4;
    case 0x2321: return (SDL12Key)0xa5;
    case 0x2502: return (SDL12Key)0xa6;
    case 0x23A1: return (SDL12Key)0xa7;
    case 0x23A3: return (SDL12Key)0xa8;
    case 0x23A4: return (SDL12Key)0xa9;
    case 0x23A6: return (SDL12Key)0xaa;
    case 0x239B: return (SDL12Key)0xab;
    case 0x239D: return (SDL12Key)0xac;
    case 0x239E: return (SDL12Key)0xad;
    case 0x23A0: return (SDL12Key)0xae;
    case 0x23A8: return (SDL12Key)0xaf;
    case 0x23AC: return (SDL12Key)0xb0;
    case 0x2264: return (SDL12Key)0xbc;
    case 0x2260: return (SDL12Key)0xbd;
    case 0x2265: return (SDL12Key)0xbe;
    case 0x222B: return (SDL12Key)0xbf;
    case 0x2234: return (SDL12Key)0xc0;
    case 0x221D: return (SDL12Key)0xc1;
    case 0x221E: return (SDL12Key)0xc2;
    case 0x2207: return (SDL12Key)0xc5;
    case 0x223C: return (SDL12Key)0xc8;
    case 0x2243: return (SDL12Key)0xc9;
    case 0x21D4: return (SDL12Key)0xcd;
    case 0x21D2: return (SDL12Key)0xce;
    case 0x2261: return (SDL12Key)0xcf;
    case 0x221A: return (SDL12Key)0xd6;
    case 0x2282: return (SDL12Key)0xda;
    case 0x2283: return (SDL12Key)0xdb;
    case 0x2229: return (SDL12Key)0xdc;
    case 0x222A: return (SDL12Key)0xdd;
    case 0x2227: return (SDL12Key)0xde;
    case 0x2228: return (SDL12Key)0xdf;
    case 0x2202: return (SDL12Key)0xef;
    case 0x0192: return (SDL12Key)0xf6;
    case 0x2190: return (SDL12Key)0xfb;
    case 0x2191: return (SDL12Key)0xfc;
    case 0x2192: return (SDL12Key)0xfd;
    case 0x2193: return (SDL12Key)0xfe;
    /* Publishing */
    case 0x2003: return (SDL12Key)0xa1;
    case 0x2002: return (SDL12Key)0xa2;
    case 0x2004: return (SDL12Key)0xa3;
    case 0x2005: return (SDL12Key)0xa4;
    case 0x2007: return (SDL12Key)0xa5;
    case 0x2008: return (SDL12Key)0xa6;
    case 0x2009: return (SDL12Key)0xa7;
    case 0x200A: return (SDL12Key)0xa8;
    case 0x2014: return (SDL12Key)0xa9;
    case 0x2013: return (SDL12Key)0xaa;
    case 0x2423: return (SDL12Key)0xac;
    case 0x2026: return (SDL12Key)0xae;
    case 0x2025: return (SDL12Key)0xaf;
    case 0x2153: return (SDL12Key)0xb0;
    case 0x2154: return (SDL12Key)0xb1;
    case 0x2155: return (SDL12Key)0xb2;
    case 0x2156: return (SDL12Key)0xb3;
    case 0x2157: return (SDL12Key)0xb4;
    case 0x2158: return (SDL12Key)0xb5;
    case 0x2159: return (SDL12Key)0xb6;
    case 0x215A: return (SDL12Key)0xb7;
    case 0x2105: return (SDL12Key)0xb8;
    case 0x2012: return (SDL12Key)0xbb;
    case 0x27E8: return (SDL12Key)0xbc;
    case 0x002E: return (SDL12Key)0xbd;
    case 0x27E9: return (SDL12Key)0xbe;
    case 0x215B: return (SDL12Key)0xc3;
    case 0x215C: return (SDL12Key)0xc4;
    case 0x215D: return (SDL12Key)0xc5;
    case 0x215E: return (SDL12Key)0xc6;
    case 0x2122: return (SDL12Key)0xc9;
    case 0x2613: return (SDL12Key)0xca;
    case 0x25C1: return (SDL12Key)0xcc;
    case 0x25B7: return (SDL12Key)0xcd;
    case 0x25CB: return (SDL12Key)0xce;
    case 0x25AF: return (SDL12Key)0xcf;
    case 0x2018: return (SDL12Key)0xd0;
    case 0x2019: return (SDL12Key)0xd1;
    case 0x201C: return (SDL12Key)0xd2;
    case 0x201D: return (SDL12Key)0xd3;
    case 0x211E: return (SDL12Key)0xd4;
    case 0x2030: return (SDL12Key)0xd5;
    case 0x2032: return (SDL12Key)0xd6;
    case 0x2033: return (SDL12Key)0xd7;
    case 0x271D: return (SDL12Key)0xd9;
    case 0x25AC: return (SDL12Key)0xdb;
    case 0x25C0: return (SDL12Key)0xdc;
    case 0x25B6: return (SDL12Key)0xdd;
    case 0x25CF: return (SDL12Key)0xde;
    case 0x25AE: return (SDL12Key)0xdf;
    case 0x25E6: return (SDL12Key)0xe0;
    case 0x25AB: return (SDL12Key)0xe1;
    case 0x25AD: return (SDL12Key)0xe2;
    case 0x25B3: return (SDL12Key)0xe3;
    case 0x25BD: return (SDL12Key)0xe4;
    case 0x2606: return (SDL12Key)0xe5;
    case 0x2022: return (SDL12Key)0xe6;
    case 0x25AA: return (SDL12Key)0xe7;
    case 0x25B2: return (SDL12Key)0xe8;
    case 0x25BC: return (SDL12Key)0xe9;
    case 0x261C: return (SDL12Key)0xea;
    case 0x261E: return (SDL12Key)0xeb;
    case 0x2663: return (SDL12Key)0xec;
    case 0x2666: return (SDL12Key)0xed;
    case 0x2665: return (SDL12Key)0xee;
    case 0x2720: return (SDL12Key)0xf0;
    case 0x2020: return (SDL12Key)0xf1;
    case 0x2021: return (SDL12Key)0xf2;
    case 0x2713: return (SDL12Key)0xf3;
    case 0x2717: return (SDL12Key)0xf4;
    case 0x266F: return (SDL12Key)0xf5;
    case 0x266D: return (SDL12Key)0xf6;
    case 0x2642: return (SDL12Key)0xf7;
    case 0x2640: return (SDL12Key)0xf8;
    case 0x260E: return (SDL12Key)0xf9;
    case 0x2315: return (SDL12Key)0xfa;
    case 0x2117: return (SDL12Key)0xfb;
    case 0x2038: return (SDL12Key)0xfc;
    case 0x201A: return (SDL12Key)0xfd;
    case 0x201E: return (SDL12Key)0xfe;
    /* Hebrew */
    case 0x2017: return (SDL12Key)0xdf;
    case 0x05D0: return (SDL12Key)0xe0;
    case 0x05D1: return (SDL12Key)0xe1;
    case 0x05D2: return (SDL12Key)0xe2;
    case 0x05D3: return (SDL12Key)0xe3;
    case 0x05D4: return (SDL12Key)0xe4;
    case 0x05D5: return (SDL12Key)0xe5;
    case 0x05D6: return (SDL12Key)0xe6;
    case 0x05D7: return (SDL12Key)0xe7;
    case 0x05D8: return (SDL12Key)0xe8;
    case 0x05D9: return (SDL12Key)0xe9;
    case 0x05DA: return (SDL12Key)0xea;
    case 0x05DB: return (SDL12Key)0xeb;
    case 0x05DC: return (SDL12Key)0xec;
    case 0x05DD: return (SDL12Key)0xed;
    case 0x05DE: return (SDL12Key)0xee;
    case 0x05DF: return (SDL12Key)0xef;
    case 0x05E0: return (SDL12Key)0xf0;
    case 0x05E1: return (SDL12Key)0xf1;
    case 0x05E2: return (SDL12Key)0xf2;
    case 0x05E3: return (SDL12Key)0xf3;
    case 0x05E4: return (SDL12Key)0xf4;
    case 0x05E5: return (SDL12Key)0xf5;
    case 0x05E6: return (SDL12Key)0xf6;
    case 0x05E7: return (SDL12Key)0xf7;
    case 0x05E8: return (SDL12Key)0xf8;
    case 0x05E9: return (SDL12Key)0xf9;
    case 0x05EA: return (SDL12Key)0xfa;
    /* Thai */
    case 0x0E01: return (SDL12Key)0xa1;
    case 0x0E02: return (SDL12Key)0xa2;
    case 0x0E03: return (SDL12Key)0xa3;
    case 0x0E04: return (SDL12Key)0xa4;
    case 0x0E05: return (SDL12Key)0xa5;
    case 0x0E06: return (SDL12Key)0xa6;
    case 0x0E07: return (SDL12Key)0xa7;
    case 0x0E08: return (SDL12Key)0xa8;
    case 0x0E09: return (SDL12Key)0xa9;
    case 0x0E0A: return (SDL12Key)0xaa;
    case 0x0E0B: return (SDL12Key)0xab;
    case 0x0E0C: return (SDL12Key)0xac;
    case 0x0E0D: return (SDL12Key)0xad;
    case 0x0E0E: return (SDL12Key)0xae;
    case 0x0E0F: return (SDL12Key)0xaf;
    case 0x0E10: return (SDL12Key)0xb0;
    case 0x0E11: return (SDL12Key)0xb1;
    case 0x0E12: return (SDL12Key)0xb2;
    case 0x0E13: return (SDL12Key)0xb3;
    case 0x0E14: return (SDL12Key)0xb4;
    case 0x0E15: return (SDL12Key)0xb5;
    case 0x0E16: return (SDL12Key)0xb6;
    case 0x0E17: return (SDL12Key)0xb7;
    case 0x0E18: return (SDL12Key)0xb8;
    case 0x0E19: return (SDL12Key)0xb9;
    case 0x0E1A: return (SDL12Key)0xba;
    case 0x0E1B: return (SDL12Key)0xbb;
    case 0x0E1C: return (SDL12Key)0xbc;
    case 0x0E1D: return (SDL12Key)0xbd;
    case 0x0E1E: return (SDL12Key)0xbe;
    case 0x0E1F: return (SDL12Key)0xbf;
    case 0x0E20: return (SDL12Key)0xc0;
    case 0x0E21: return (SDL12Key)0xc1;
    case 0x0E22: return (SDL12Key)0xc2;
    case 0x0E23: return (SDL12Key)0xc3;
    case 0x0E24: return (SDL12Key)0xc4;
    case 0x0E25: return (SDL12Key)0xc5;
    case 0x0E26: return (SDL12Key)0xc6;
    case 0x0E27: return (SDL12Key)0xc7;
    case 0x0E28: return (SDL12Key)0xc8;
    case 0x0E29: return (SDL12Key)0xc9;
    case 0x0E2A: return (SDL12Key)0xca;
    case 0x0E2B: return (SDL12Key)0xcb;
    case 0x0E2C: return (SDL12Key)0xcc;
    case 0x0E2D: return (SDL12Key)0xcd;
    case 0x0E2E: return (SDL12Key)0xce;
    case 0x0E2F: return (SDL12Key)0xcf;
    case 0x0E30: return (SDL12Key)0xd0;
    case 0x0E31: return (SDL12Key)0xd1;
    case 0x0E32: return (SDL12Key)0xd2;
    case 0x0E33: return (SDL12Key)0xd3;
    case 0x0E34: return (SDL12Key)0xd4;
    case 0x0E35: return (SDL12Key)0xd5;
    case 0x0E36: return (SDL12Key)0xd6;
    case 0x0E37: return (SDL12Key)0xd7;
    case 0x0E38: return (SDL12Key)0xd8;
    case 0x0E39: return (SDL12Key)0xd9;
    case 0x0E3A: return (SDL12Key)0xda;
    case 0x0E3F: return (SDL12Key)0xdf;
    case 0x0E40: return (SDL12Key)0xe0;
    case 0x0E41: return (SDL12Key)0xe1;
    case 0x0E42: return (SDL12Key)0xe2;
    case 0x0E43: return (SDL12Key)0xe3;
    case 0x0E44: return (SDL12Key)0xe4;
    case 0x0E45: return (SDL12Key)0xe5;
    case 0x0E46: return (SDL12Key)0xe6;
    case 0x0E47: return (SDL12Key)0xe7;
    case 0x0E48: return (SDL12Key)0xe8;
    case 0x0E49: return (SDL12Key)0xe9;
    case 0x0E4A: return (SDL12Key)0xea;
    case 0x0E4B: return (SDL12Key)0xeb;
    case 0x0E4C: return (SDL12Key)0xec;
    case 0x0E4D: return (SDL12Key)0xed;
    case 0x0E50: return (SDL12Key)0xf0;
    case 0x0E51: return (SDL12Key)0xf1;
    case 0x0E52: return (SDL12Key)0xf2;
    case 0x0E53: return (SDL12Key)0xf3;
    case 0x0E54: return (SDL12Key)0xf4;
    case 0x0E55: return (SDL12Key)0xf5;
    case 0x0E56: return (SDL12Key)0xf6;
    case 0x0E57: return (SDL12Key)0xf7;
    case 0x0E58: return (SDL12Key)0xf8;
    case 0x0E59: return (SDL12Key)0xf9;
    /* end of SDLK_WORLD_ keys based on Latin-* or similar High-ASCII charsets
     * and the low byte of their corresponding X11 XK_* KeySyms */
    default: break;
    }

    return SDLK12_UNKNOWN;
}
static SDL12Key
Scancode20toKeysym12(const SDL_Scancode scancode20)
{
    switch (scancode20) {
    #define CASESCANCODE20TOKEY12(s20, k12) case SDL_SCANCODE_##s20: return SDLK12_##k12
    CASESCANCODE20TOKEY12(A,a);
    CASESCANCODE20TOKEY12(B,b);
    CASESCANCODE20TOKEY12(C,c);
    CASESCANCODE20TOKEY12(D,d);
    CASESCANCODE20TOKEY12(E,e);
    CASESCANCODE20TOKEY12(F,f);
    CASESCANCODE20TOKEY12(G,g);
    CASESCANCODE20TOKEY12(H,h);
    CASESCANCODE20TOKEY12(I,i);
    CASESCANCODE20TOKEY12(J,j);
    CASESCANCODE20TOKEY12(K,k);
    CASESCANCODE20TOKEY12(L,l);
    CASESCANCODE20TOKEY12(M,m);
    CASESCANCODE20TOKEY12(N,n);
    CASESCANCODE20TOKEY12(O,o);
    CASESCANCODE20TOKEY12(P,p);
    CASESCANCODE20TOKEY12(Q,q);
    CASESCANCODE20TOKEY12(R,r);
    CASESCANCODE20TOKEY12(S,s);
    CASESCANCODE20TOKEY12(T,t);
    CASESCANCODE20TOKEY12(U,u);
    CASESCANCODE20TOKEY12(V,v);
    CASESCANCODE20TOKEY12(W,w);
    CASESCANCODE20TOKEY12(X,x);
    CASESCANCODE20TOKEY12(Y,y);
    CASESCANCODE20TOKEY12(Z,z);
    CASESCANCODE20TOKEY12(1,1);
    CASESCANCODE20TOKEY12(2,2);
    CASESCANCODE20TOKEY12(3,3);
    CASESCANCODE20TOKEY12(4,4);
    CASESCANCODE20TOKEY12(5,5);
    CASESCANCODE20TOKEY12(6,6);
    CASESCANCODE20TOKEY12(7,7);
    CASESCANCODE20TOKEY12(8,8);
    CASESCANCODE20TOKEY12(9,9);
    CASESCANCODE20TOKEY12(0,0);
    CASESCANCODE20TOKEY12(RETURN,RETURN);
    CASESCANCODE20TOKEY12(ESCAPE,ESCAPE);
    CASESCANCODE20TOKEY12(BACKSPACE,BACKSPACE);
    CASESCANCODE20TOKEY12(TAB,TAB);
    CASESCANCODE20TOKEY12(SPACE,SPACE);
    CASESCANCODE20TOKEY12(MINUS,MINUS);
    CASESCANCODE20TOKEY12(EQUALS,EQUALS);
    CASESCANCODE20TOKEY12(LEFTBRACKET,LEFTBRACKET);
    CASESCANCODE20TOKEY12(RIGHTBRACKET,RIGHTBRACKET);
    CASESCANCODE20TOKEY12(BACKSLASH,BACKSLASH);
    CASESCANCODE20TOKEY12(NONUSHASH,HASH);
    CASESCANCODE20TOKEY12(SEMICOLON,SEMICOLON);
    CASESCANCODE20TOKEY12(APOSTROPHE,QUOTE);
    CASESCANCODE20TOKEY12(GRAVE,BACKQUOTE);
    CASESCANCODE20TOKEY12(COMMA,COMMA);
    CASESCANCODE20TOKEY12(PERIOD,PERIOD);
    CASESCANCODE20TOKEY12(SLASH,SLASH);
    CASESCANCODE20TOKEY12(CAPSLOCK,CAPSLOCK);
    CASESCANCODE20TOKEY12(F1,F1);
    CASESCANCODE20TOKEY12(F2,F2);
    CASESCANCODE20TOKEY12(F3,F3);
    CASESCANCODE20TOKEY12(F4,F4);
    CASESCANCODE20TOKEY12(F5,F5);
    CASESCANCODE20TOKEY12(F6,F6);
    CASESCANCODE20TOKEY12(F7,F7);
    CASESCANCODE20TOKEY12(F8,F8);
    CASESCANCODE20TOKEY12(F9,F9);
    CASESCANCODE20TOKEY12(F10,F10);
    CASESCANCODE20TOKEY12(F11,F11);
    CASESCANCODE20TOKEY12(F12,F12);
    CASESCANCODE20TOKEY12(PRINTSCREEN,PRINT);
    CASESCANCODE20TOKEY12(SCROLLLOCK,SCROLLOCK);
    CASESCANCODE20TOKEY12(PAUSE,PAUSE);
    CASESCANCODE20TOKEY12(INSERT,INSERT);
    CASESCANCODE20TOKEY12(HOME,HOME);
    CASESCANCODE20TOKEY12(PAGEUP,PAGEUP);
    CASESCANCODE20TOKEY12(DELETE,DELETE);
    CASESCANCODE20TOKEY12(END,END);
    CASESCANCODE20TOKEY12(PAGEDOWN,PAGEDOWN);
    CASESCANCODE20TOKEY12(RIGHT,RIGHT);
    CASESCANCODE20TOKEY12(LEFT,LEFT);
    CASESCANCODE20TOKEY12(DOWN,DOWN);
    CASESCANCODE20TOKEY12(UP,UP);
    CASESCANCODE20TOKEY12(NUMLOCKCLEAR,NUMLOCK);

    CASESCANCODE20TOKEY12(KP_DIVIDE,KP_DIVIDE);
    CASESCANCODE20TOKEY12(KP_MULTIPLY,KP_MULTIPLY);
    CASESCANCODE20TOKEY12(KP_MINUS,KP_MINUS);
    CASESCANCODE20TOKEY12(KP_PLUS,KP_PLUS);
    CASESCANCODE20TOKEY12(KP_ENTER,KP_ENTER);
    CASESCANCODE20TOKEY12(KP_1,KP1);
    CASESCANCODE20TOKEY12(KP_2,KP2);
    CASESCANCODE20TOKEY12(KP_3,KP3);
    CASESCANCODE20TOKEY12(KP_4,KP4);
    CASESCANCODE20TOKEY12(KP_5,KP5);
    CASESCANCODE20TOKEY12(KP_6,KP6);
    CASESCANCODE20TOKEY12(KP_7,KP7);
    CASESCANCODE20TOKEY12(KP_8,KP8);
    CASESCANCODE20TOKEY12(KP_9,KP9);
    CASESCANCODE20TOKEY12(KP_0,KP0);

    CASESCANCODE20TOKEY12(NONUSBACKSLASH,BACKSLASH);
    /* In theory, this could be MENU, or COMPOSE, or neither, but on my machine, it's MENU. */
    CASESCANCODE20TOKEY12(APPLICATION,MENU);
    CASESCANCODE20TOKEY12(POWER,POWER);
    CASESCANCODE20TOKEY12(F13,F13);
    CASESCANCODE20TOKEY12(F14,F14);
    CASESCANCODE20TOKEY12(F15,F15);
    CASESCANCODE20TOKEY12(KP_EQUALS,KP_EQUALS);
    /* SDL 1.2 doesn't support F16..F21 */
    /* Nor SDL_SCANCODE_EXECUTE */
    CASESCANCODE20TOKEY12(HELP,HELP);
    CASESCANCODE20TOKEY12(MENU,MENU);
    /* The next several scancodes don't have equivalents, until... */
    CASESCANCODE20TOKEY12(SYSREQ,SYSREQ);
    CASESCANCODE20TOKEY12(CLEAR,CLEAR);
    /* Skip some more... */
    CASESCANCODE20TOKEY12(LCTRL,LCTRL);
    CASESCANCODE20TOKEY12(LSHIFT,LSHIFT);
    CASESCANCODE20TOKEY12(LALT,LALT);
#ifdef __MACOSX__
    CASESCANCODE20TOKEY12(LGUI,LMETA);
#else
    CASESCANCODE20TOKEY12(LGUI,LSUPER);
#endif
    CASESCANCODE20TOKEY12(RCTRL,RCTRL);
    CASESCANCODE20TOKEY12(RSHIFT,RSHIFT);
    CASESCANCODE20TOKEY12(RALT,RALT);
#ifdef __MACOSX__
    CASESCANCODE20TOKEY12(RGUI,RMETA);
#else
    CASESCANCODE20TOKEY12(RGUI,RSUPER);
#endif

    CASESCANCODE20TOKEY12(MODE,MODE);
    #undef CASESCANCODE20TOKEY12
    default: break;
    }

    return SDLK12_UNKNOWN;
}

static Uint8
Scancode20to12(SDL_Scancode sc)
{
    /* SDL 1.2 scancodes are the actual raw scancodes (for the most part), and
       so differ wildly between different systems. Fortunately, this means
       they're rarely used, and often have fallbacks. Here, we support them
       for three systems: Win32, Mac OS X, and a synthesized pseudo-Linux that
       should work.
       Windows scancodes are bascially just Linux ones - 8. OS X has a totally
       different set of scancodes from everyone else. Linux's scancodes change
       depending on what driver you're using, but only really for a few keys.
       Since there are applications (DOSBox) which look this up and behave
       accordingly, but have fallbacks, those keys have scancodes of 0 here,
       to trigger the fallbacks. */
    switch(sc) {
#if defined(_WIN32)
#define CASESCANCODE20TO12(sc20, sc12, sc12mac) case SDL_SCANCODE_##sc20: return (sc12 ? (sc12 - 8) : 0)
#elif defined(__MACOSX__)
#define CASESCANCODE20TO12(sc20, sc12, sc12mac) case SDL_SCANCODE_##sc20: return sc12mac
#else
#define CASESCANCODE20TO12(sc20, sc12, sc12mac) case SDL_SCANCODE_##sc20: return sc12
#endif
    CASESCANCODE20TO12(0, 0x13, 0x1D);
    CASESCANCODE20TO12(1, 0x0A, 0x12);
    CASESCANCODE20TO12(2, 0x0B, 0x13);
    CASESCANCODE20TO12(3, 0x0C, 0x14);
    CASESCANCODE20TO12(4, 0x0D, 0x15);
    CASESCANCODE20TO12(5, 0x0E, 0x17);
    CASESCANCODE20TO12(6, 0x0F, 0x16);
    CASESCANCODE20TO12(7, 0x10, 0x1A);
    CASESCANCODE20TO12(8, 0x11, 0x1C);
    CASESCANCODE20TO12(9, 0x12, 0x19);
    CASESCANCODE20TO12(A, 0x26, 0x00);
    CASESCANCODE20TO12(APOSTROPHE, 0x30, 0x27);
    CASESCANCODE20TO12(B, 0x38, 0x0B);
    CASESCANCODE20TO12(BACKSLASH, 0x33, 0x2A);
    CASESCANCODE20TO12(BACKSPACE, 0x16, 0x33);
    CASESCANCODE20TO12(C, 0x36, 0x08);
    CASESCANCODE20TO12(CAPSLOCK, 0x42, 0x00);
    CASESCANCODE20TO12(COMMA, 0x3B, 0x2B);
    CASESCANCODE20TO12(D, 0x28, 0x02);
    CASESCANCODE20TO12(DELETE, 0x00, 0x75);
    CASESCANCODE20TO12(DOWN, 0x00, 0x7D);
    CASESCANCODE20TO12(E, 0x1A, 0x0E);
    CASESCANCODE20TO12(END, 0x00, 0x77);
    CASESCANCODE20TO12(EQUALS, 0x15, 0x18);
    CASESCANCODE20TO12(ESCAPE, 0x09, 0x35);
    CASESCANCODE20TO12(F, 0x29, 0x03);
    CASESCANCODE20TO12(F1, 0x43, 0x7A);
    CASESCANCODE20TO12(F10, 0x4C, 0x6E);
    CASESCANCODE20TO12(F11, 0x5F, 0x67);
    CASESCANCODE20TO12(F12, 0x60, 0x6F);
    CASESCANCODE20TO12(F2, 0x44, 0x78);
    CASESCANCODE20TO12(F3, 0x45, 0x63);
    CASESCANCODE20TO12(F4, 0x46, 0x76);
    CASESCANCODE20TO12(F5, 0x47, 0x60);
    CASESCANCODE20TO12(F6, 0x48, 0x61);
    CASESCANCODE20TO12(F7, 0x49, 0x62);
    CASESCANCODE20TO12(F8, 0x4A, 0x64);
    CASESCANCODE20TO12(F9, 0x4B, 0x65);
    CASESCANCODE20TO12(G, 0x2A, 0x05);
    CASESCANCODE20TO12(GRAVE, 0x31, 0x32); /* Note: the mac scancode might not be 100% correct, see below */
    CASESCANCODE20TO12(H, 0x2B, 0x04);
    CASESCANCODE20TO12(HOME, 0x00, 0x73);
    CASESCANCODE20TO12(I, 0x1F, 0x22);
    CASESCANCODE20TO12(INSERT, 0x00, 0x72);
    CASESCANCODE20TO12(J, 0x2C, 0x26);
    CASESCANCODE20TO12(K, 0x2D, 0x28);
    CASESCANCODE20TO12(KP_0, 0x5A, 0x52);
    CASESCANCODE20TO12(KP_1, 0x57, 0x53);
    CASESCANCODE20TO12(KP_2, 0x58, 0x54);
    CASESCANCODE20TO12(KP_3, 0x59, 0x55);
    CASESCANCODE20TO12(KP_4, 0x53, 0x56);
    CASESCANCODE20TO12(KP_5, 0x54, 0x57);
    CASESCANCODE20TO12(KP_6, 0x55, 0x58);
    CASESCANCODE20TO12(KP_7, 0x4F, 0x59);
    CASESCANCODE20TO12(KP_8, 0x50, 0x5B);
    CASESCANCODE20TO12(KP_9, 0x51, 0x5C);
    CASESCANCODE20TO12(KP_DIVIDE, 0x00, 0x4B);
    CASESCANCODE20TO12(KP_ENTER, 0x00, 0x4C);
    CASESCANCODE20TO12(KP_EQUALS, 0x00, 0x51);
    CASESCANCODE20TO12(KP_MINUS, 0x52, 0x4E);
    CASESCANCODE20TO12(KP_MULTIPLY, 0x3F, 0x43);
    CASESCANCODE20TO12(KP_PERIOD, 0x5B, 0x41);
    CASESCANCODE20TO12(KP_PLUS, 0x56, 0x45);
    CASESCANCODE20TO12(L, 0x2E, 0x25);
    CASESCANCODE20TO12(LALT, 0x40, 0x00);
    CASESCANCODE20TO12(LCTRL, 0x25, 0x00);
    CASESCANCODE20TO12(LEFT, 0x00, 0x7B);
    CASESCANCODE20TO12(LEFTBRACKET, 0x22, 0x21);
    CASESCANCODE20TO12(LGUI, 0x85, 0x00);
    CASESCANCODE20TO12(LSHIFT, 0x32, 0x00);
    CASESCANCODE20TO12(M, 0x3A, 0x2E);
    CASESCANCODE20TO12(MINUS, 0x14, 0x1B);
    CASESCANCODE20TO12(N, 0x39, 0x2D);
    /* On Macs with ANSI layout, 0x32 is SDL_SCANCODE_GRAVE and _NONUSBACKSLASH doesn't exist.
     * On Macs with ISO layout, 0x32 is _NONUSBACKSLASH and 0x0A is the key at the position of _GRAVE..
     * Probably it's best to keep _GRAVE at 0x32 and use 0x0A for _NONUSBACKSLASH instead,
     * so at least it has a unique scancode at all. */
    CASESCANCODE20TO12(NONUSBACKSLASH, 0x5E, 0x0A);
    CASESCANCODE20TO12(NUMLOCKCLEAR, 0x4D, 0x47);
    CASESCANCODE20TO12(O, 0x20, 0x1F);
    CASESCANCODE20TO12(P, 0x21, 0x23);
    CASESCANCODE20TO12(PAGEDOWN, 0x00, 0x79);
    CASESCANCODE20TO12(PAGEUP, 0x00, 0x74);
    CASESCANCODE20TO12(PERIOD, 0x3C, 0x2F);
    CASESCANCODE20TO12(PRINTSCREEN, 0x6B, 0x6B);
    CASESCANCODE20TO12(Q, 0x18, 0x0C);
    CASESCANCODE20TO12(R, 0x1B, 0x0F);
    CASESCANCODE20TO12(RETURN, 0x24, 0x24);
    CASESCANCODE20TO12(RGUI, 0x86, 0x00);
    CASESCANCODE20TO12(RIGHT, 0x00, 0x7C);
    CASESCANCODE20TO12(RIGHTBRACKET, 0x23, 0x1E);
    CASESCANCODE20TO12(RSHIFT, 0x3E, 0x00);
    CASESCANCODE20TO12(S, 0x27, 0x01);
    CASESCANCODE20TO12(SCROLLLOCK, 0x4E, 0x71);
    CASESCANCODE20TO12(SEMICOLON, 0x2F, 0x29);
    CASESCANCODE20TO12(SLASH, 0x3D, 0x2C);
    CASESCANCODE20TO12(SPACE, 0x41, 0x31);
    CASESCANCODE20TO12(T, 0x1C, 0x11);
    CASESCANCODE20TO12(TAB, 0x17, 0x30);
    CASESCANCODE20TO12(U, 0x1E, 0x20);
    CASESCANCODE20TO12(UP, 0x00, 0x7E);
    CASESCANCODE20TO12(V, 0x37, 0x09);
    CASESCANCODE20TO12(W, 0x19, 0x0D);
    CASESCANCODE20TO12(X, 0x35, 0x07);
    CASESCANCODE20TO12(Y, 0x1D, 0x10);
    CASESCANCODE20TO12(Z, 0x34, 0x06);
#undef CASESCANCODE20TO12
    default:
        /* If we don't know it, return 0, which is "unknown".
           It's also "a" on Mac OS X, but SDL 1.2 uses it as "unknown", too. */
        return 0;
    }
}

DECLSPEC12 Uint8 * SDLCALL
SDL_GetKeyState(int *numkeys)
{
    if (numkeys) {
        *numkeys = (int) SDL_arraysize(KeyState);
    }
    return KeyState;
}

static int DecodeUTF8Char(char **ptr)
{
    Uint32 first_c = **(unsigned char **)ptr;
    int num_bytes = (first_c) ? (31 - SDL20_MostSignificantBitIndex32(~(first_c << 24))) : 0;
    Uint32 value = first_c & ((1 << (8 - num_bytes)) - 1);
    int i;

    (*ptr)++;
    for (i = 1; i < num_bytes; ++i) {
        value = (value << 6) | (**(unsigned char **)ptr & 0x3f);
        (*ptr)++;
    }
    return value;
}

/* Add the pending KEYDOWN event to the EventQueue, possibly with 'unicode' set
 * Returns 1 if there was a pending event. */
static int FlushPendingKeydownEvent(Uint32 unicode)
{
    if (PendingKeydownEvent.type != SDL12_KEYDOWN) {
        return 0;
    }

    PendingKeydownEvent.key.keysym.unicode = unicode;
    PushEventIfNotFiltered(&PendingKeydownEvent);

    if (KeyRepeatDelay) {
        SDL20_memcpy(&KeyRepeatEvent, &PendingKeydownEvent, sizeof (SDL12_Event));
        /* SDL 1.2 waits for the delay, and then a full interval past that before the first repeat is reported. */
        KeyRepeatNextTicks = SDL20_GetTicks() + KeyRepeatDelay + KeyRepeatInterval;
    }

    /* Reset the event. */
    SDL20_memset(&PendingKeydownEvent, 0, sizeof (SDL12_Event));

    return 1;
}

static int SDLCALL
EventFilter20to12(void *data, SDL_Event *event20)
{
    SDL12_Event event12;
    SDL12_SysWMmsg msg;

    SDL_assert(data == NULL);  /* currently unused. */

    SDL20_zero(event12);

    switch (event20->type) {
        case SDL_QUIT:
            event12.type = SDL12_QUIT;
            break;

        case SDL_WINDOWEVENT:
            if (!VideoWindow20) {
                break;  /* no window? No event. */
            }

            switch (event20->window.event) {
                /* don't send an SDL12_QUIT event for SDL_WINDOWEVENT_CLOSE;
                   we only ever have a single window, so an SDL_QUIT will be
                   coming from SDL2 next anyhow, so just send that on. */

                case SDL_WINDOWEVENT_SHOWN:
                case SDL_WINDOWEVENT_EXPOSED:
                    /* drop these during window creation (see issue #229) */
                    if (!SetVideoModeInProgress) {
                        event12.type = SDL12_VIDEOEXPOSE;
                    }
                    break;

                case SDL_WINDOWEVENT_RESIZED: {
                    /* don't generate a VIDEORESIZE event based on SIZE_CHANGED
                       events: the recommended way to handle VIDEORESIZE is
                       with a new SDL_SetVideoMode() call, and creating a new
                       window generates a SIZE_CHANGED event, which leads to an
                       infinite loop. */

                    /* don't report VIDEORESIZE if we're fullscreen-desktop;
                       we're doing logical scaling and as far as the app is
                       concerned the window doesn't change. */
                    const Uint32 flags = SDL20_GetWindowFlags(VideoWindow20);
                    if ((flags & SDL_WINDOW_FULLSCREEN_DESKTOP) == SDL_WINDOW_FULLSCREEN_DESKTOP) {
                        break;
                    }

                    event12.type = SDL12_VIDEORESIZE;
                    event12.resize.w = event20->window.data1;
                    event12.resize.h = event20->window.data2;
                    break;
                }

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

        case SDL_SYSWMEVENT:
            if (!SupportSysWM) {
                return 1;
            }

            #if defined(SDL_VIDEO_DRIVER_WINDOWS)
                SDL_assert(event20->syswm.msg->subsystem == SDL_SYSWM_WINDOWS);
                msg.hwnd = event20->syswm.msg->msg.win.hwnd;
                msg.msg = event20->syswm.msg->msg.win.msg;
                msg.wParam = event20->syswm.msg->msg.win.wParam;
                msg.lParam = event20->syswm.msg->msg.win.lParam;
            #elif defined(SDL_VIDEO_DRIVER_X11)
                SDL_assert(event20->syswm.msg->subsystem == SDL_SYSWM_X11);
                msg.subsystem = SDL12_SYSWM_X11;
                SDL20_memcpy(&msg.event.xevent, &event20->syswm.msg->msg.x11.event, sizeof (XEvent));
            #else
                SDL_assert(!"should have been caught by !SupportsSysWM test");
            #endif

            SDL20_memcpy(&msg.version, SDL_Linked_Version(), sizeof (msg.version));
            event12.type = SDL12_SYSWMEVENT;
            event12.syswm.msg = &msg;  /* this is stack-allocated, but we copy and update the pointer later. */
            break;

        case SDL_KEYUP:
            if (event20->key.repeat) {
                return 1;  /* ignore 2.0-style key repeat events */
            }

            if (TranslateKeyboardLayout) {
                event12.key.keysym.sym = Keysym20to12(event20->key.keysym.sym);
            } else {
                event12.key.keysym.sym = Scancode20toKeysym12(event20->key.keysym.scancode);
            }

            if (KeyRepeatNextTicks) {
                SDL_assert(KeyRepeatEvent.type == SDL12_KEYDOWN);
                if (KeyRepeatEvent.key.keysym.sym == event12.key.keysym.sym) {
                    KeyRepeatNextTicks = 0;
                }
            }

            KeyState[event12.key.keysym.sym] = event20->key.state;

            event12.type = SDL12_KEYUP;
            event12.key.which = 0;
            event12.key.state = event20->key.state;
            /* turns out that some apps actually made use of the hardware scancodes (checking for platform beforehand) */
            event12.key.keysym.scancode = Scancode20to12(event20->key.keysym.scancode);
            event12.key.keysym.mod = event20->key.keysym.mod;  /* these match up between 1.2 and 2.0! */
            event12.key.keysym.unicode = 0;

            /* If there's a pending KEYDOWN event, flush it on KEYUP. */
            FlushPendingKeydownEvent(0);
            break;

        case SDL_KEYDOWN:
            FlushPendingKeydownEvent(0);
            if (event20->key.repeat) {
                return 1;  /* ignore 2.0-style key repeat events */
            }

            if (TranslateKeyboardLayout) {
                PendingKeydownEvent.key.keysym.sym = Keysym20to12(event20->key.keysym.sym);
            } else {
                PendingKeydownEvent.key.keysym.sym = Scancode20toKeysym12(event20->key.keysym.scancode);
            }

            KeyState[PendingKeydownEvent.key.keysym.sym] = event20->key.state;

            PendingKeydownEvent.type = (event20->type == SDL_KEYDOWN) ? SDL12_KEYDOWN : SDL12_KEYUP;
            PendingKeydownEvent.key.which = 0;
            PendingKeydownEvent.key.state = event20->key.state;
            /* turns out that some apps actually made use of the hardware scancodes (checking for platform beforehand) */
            PendingKeydownEvent.key.keysym.scancode = Scancode20to12(event20->key.keysym.scancode);
            PendingKeydownEvent.key.keysym.mod = event20->key.keysym.mod;  /* these match up between 1.2 and 2.0! */
            PendingKeydownEvent.key.keysym.unicode = 0;

            /* If Unicode is not enabled, flush all KEYDOWN events immediately. */
            if (!EnabledUnicode) {
                FlushPendingKeydownEvent(0);
                return 1;
            }

            /* some programs rely on unicode values for these control characters */
            switch (PendingKeydownEvent.key.keysym.sym)
            {
                case SDLK12_BACKSPACE:
                    FlushPendingKeydownEvent('\b');
                    break;
                case SDLK12_TAB:
                    FlushPendingKeydownEvent('\t');
                    break;
                case SDLK12_RETURN:
                case SDLK12_KP_ENTER:
                    /* Enter: \r, Shift+Enter: \n */
                    if (PendingKeydownEvent.key.keysym.mod & KMOD_SHIFT)
                        FlushPendingKeydownEvent('\n');
                    else
                        FlushPendingKeydownEvent('\r');
                    break;
                case SDLK12_ESCAPE:
                    FlushPendingKeydownEvent(0x1B); /* '\e' */
                    break;
                default:
                    /* not a supported control character
                       when CTRL is pressed, text events aren't sent so use fallback for unicode */
                    if (PendingKeydownEvent.key.keysym.mod & KMOD_CTRL) {
                        const char *keyName = SDL_GetKeyName(PendingKeydownEvent.key.keysym.sym);
                        /* use key name as unicode if it's a single character */
                        if (keyName[0] && !keyName[1])
                            FlushPendingKeydownEvent(keyName[0]);
                        else
                            FlushPendingKeydownEvent(0);
                    }
                    break;
            }

            return 1;

        case SDL_TEXTEDITING: return 1;
        case SDL_TEXTINPUT: {
            char *text = event20->text.text;
            const int codePoint = DecodeUTF8Char(&text);
            FlushPendingKeydownEvent(codePoint);
            return 1;
        }

        case SDL_MOUSEMOTION:
            if (!VideoSurface12) {
                return 1;  /* we don't have a screen surface yet? Don't send this on to the app. */
            }

            event12.type = SDL12_MOUSEMOTION;
            event12.motion.which = (Uint8) event20->motion.which;
            event12.motion.state = event20->motion.state;
            AdjustOpenGLLogicalScalingPoint(&event20->motion.x, &event20->motion.y);
            /* Clamp the absolute position to the window dimensions. */
            event20->motion.x = SDL_max(SDL_min(event20->motion.x, VideoSurface12->w), 0);
            event20->motion.y = SDL_max(SDL_min(event20->motion.y, VideoSurface12->h), 0);
            event12.motion.x = (Uint16) event20->motion.x;
            event12.motion.y = (Uint16) event20->motion.y;
            if (UseMouseRelativeScaling) {
                AdjustOpenGLLogicalScalingVector(&event20->motion.xrel,
                                                 &event20->motion.yrel,
                                                 &MouseRelativeRemainder.x,
                                                 &MouseRelativeRemainder.y);
            }
            event12.motion.xrel = (Sint16) event20->motion.xrel;
            event12.motion.yrel = (Sint16) event20->motion.yrel;
            if (MouseInputIsRelative) {
                /* in relative mode, clamp fake absolute position to the window dimensions. */
                #define ADJUST_RELATIVE(axis, rel, dim) { \
                    MousePosition.axis += event20->motion.rel; \
                    if (MousePosition.axis <= 0) { \
                        MousePosition.axis = 0; \
                    } else if (MousePosition.axis >= VideoSurface12->dim) { \
                        MousePosition.axis = (VideoSurface12->dim - 1); \
                    } \
                    event12.motion.axis = MousePosition.axis; \
                }
                ADJUST_RELATIVE(x, xrel, w);
                ADJUST_RELATIVE(y, yrel, h);
                #undef ADJUST_RELATIVE
            } else {
                MousePosition.x = event12.motion.x;
                MousePosition.y = event12.motion.y;
            }
            break;

        case SDL_MOUSEBUTTONDOWN:
            event12.type = SDL12_MOUSEBUTTONDOWN;
            event12.button.which = (Uint8) event20->button.which;
            event12.button.button = event20->button.button;
            if (event12.button.button > 3) {
                event12.button.button += 2; /* SDL_BUTTON_X1/2 */
            }
            event12.button.state = event20->button.state;
            if (MouseInputIsRelative) {
                /* If we're using relative mouse input, we need to use our "fake" position. */
                event12.button.x = MousePosition.x;
                event12.button.y = MousePosition.y;
            } else {
                AdjustOpenGLLogicalScalingPoint(&event20->button.x, &event20->button.y);
                event12.button.x = (Uint16) event20->button.x;
                event12.button.y = (Uint16) event20->button.y;
            }
            break;

        case SDL_MOUSEBUTTONUP:
            event12.type = SDL12_MOUSEBUTTONUP;
            event12.button.which = (Uint8) event20->button.which;
            event12.button.button = event20->button.button;
            if (event12.button.button > 3) {
                event12.button.button += 2; /* SDL_BUTTON_X1/2 */
            }
            event12.button.state = event20->button.state;
            if (MouseInputIsRelative) {
                /* If we're using relative mouse input, we need to use our "fake" position. */
                event12.button.x = MousePosition.x;
                event12.button.y = MousePosition.y;
            } else {
                AdjustOpenGLLogicalScalingPoint(&event20->button.x, &event20->button.y);
                event12.button.x = (Uint16) event20->button.x;
                event12.button.y = (Uint16) event20->button.y;
            }
            break;

        case SDL_MOUSEWHEEL:
            if (event20->wheel.y == 0) {
                break;  /* don't support horizontal wheels in 1.2. */
            }
            event12.type = SDL12_MOUSEBUTTONDOWN;
            event12.button.which = (Uint8) event20->wheel.which;
            event12.button.button = (event20->wheel.y > 0) ? 4 : 5;  /* wheelup is 4, down is 5. */
            event12.button.state = SDL_PRESSED;
            event12.button.x = MousePosition.x;
            event12.button.y = MousePosition.y;
            PushEventIfNotFiltered(&event12);

            event12.type = SDL12_MOUSEBUTTONUP;  /* immediately release mouse "button" at the end of this switch. */
            event12.button.state = SDL_RELEASED;
            break;

        case SDL_JOYAXISMOTION:
            if (!JoysticksAreGameControllers) {
                const int which = FindJoystick12IndexByInstanceId(event20->jaxis.which);
                if (which != -1) {
                    event12.type = SDL12_JOYAXISMOTION;
                    event12.jaxis.which = (Uint8) which;
                    event12.jaxis.axis = event20->jaxis.axis;
                    event12.jaxis.value = event20->jaxis.value;
                }
            }
            break;

        case SDL_JOYBALLMOTION:
            if (!JoysticksAreGameControllers) {
                const int which = FindJoystick12IndexByInstanceId(event20->jball.which);
                if (which != -1) {
                    event12.type = SDL12_JOYBALLMOTION;
                    event12.jball.which = (Uint8) which;
                    event12.jball.ball = event20->jball.ball;
                    event12.jball.xrel = event20->jball.xrel;
                    event12.jball.yrel = event20->jball.yrel;
                }
            }
            break;

        case SDL_JOYHATMOTION:
            if (!JoysticksAreGameControllers) {
                const int which = FindJoystick12IndexByInstanceId(event20->jhat.which);
                if (which != -1) {
                    event12.type = SDL12_JOYHATMOTION;
                    event12.jhat.which = (Uint8) which;
                    event12.jhat.hat = event20->jhat.hat;
                    event12.jhat.value = event20->jhat.value;
                }
            }
            break;

        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
            if (!JoysticksAreGameControllers) {
                const int which = FindJoystick12IndexByInstanceId(event20->jbutton.which);
                if (which != -1) {
                    event12.type = (event20->jbutton.state) ? SDL12_JOYBUTTONDOWN : SDL12_JOYBUTTONUP;
                    event12.jbutton.which = (Uint8) which;
                    event12.jbutton.button = event20->jbutton.button;
                    event12.jbutton.state = event20->jbutton.state;
                }
            }
            break;

        case SDL_CONTROLLERAXISMOTION:
            if (JoysticksAreGameControllers) {
                const int which = FindJoystick12IndexByInstanceId(event20->caxis.which);
                if (which != -1) {
                    event12.type = SDL12_JOYAXISMOTION;
                    event12.jaxis.which = (Uint8) which;
                    event12.jaxis.axis = event20->caxis.axis;
                    event12.jaxis.value = event20->caxis.value;
                }
            }
            break;

        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP:
            if (JoysticksAreGameControllers) {
                const int which = FindJoystick12IndexByInstanceId(event20->cbutton.which);
                if (which != -1) {
                    event12.type = (event20->cbutton.state) ? SDL12_JOYBUTTONDOWN : SDL12_JOYBUTTONUP;
                    event12.jbutton.which = (Uint8) which;
                    event12.jbutton.button = event20->cbutton.button;
                    event12.jbutton.state = event20->cbutton.state;
                }
            }
            break;


        /*
        case SDL_JOYDEVICEADDED:
        case SDL_JOYDEVICEREMOVED:
        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
        case SDL_CONTROLLERDEVICEREMAPPED:
        case SDL_FINGERDOWN:
        case SDL_FINGERUP:
        case SDL_FINGERMOTION:
        case SDL_DOLLARGESTURE:
        case SDL_DOLLARRECORD:
        case SDL_MULTIGESTURE:
        case SDL_CLIPBOARDUPDATE:
        case SDL_DROPFILE:
        */

        default:
            return 1;  /* drop everything else. */
    }

    PushEventIfNotFiltered(&event12);

    /* always pass it to the 2.0 event queue, as internal watchers (like the render API)
       might need to see these events to deal with logical scaling, etc. We've already
       copied it to the separate 1.2 event queue, and run the app's 1.2 event filter.
       Next time we call 1.2's SDL_PollEvent or SDL_PumpEvents(), we'll throw away the
       entire SDL2 event queue, as everything that cares about those will have then
       had a chance to examine it. */
    return 1;
}

DECLSPEC12 void SDLCALL
SDL_SetEventFilter(SDL12_EventFilter filter12)
{
    /* We always have a filter installed, but will call the app's too. */
    EventFilter12 = filter12;
}

DECLSPEC12 SDL12_EventFilter SDLCALL
SDL_GetEventFilter(void)
{
    return EventFilter12;
}


static SDL12_Rect *
Rect20to12(const SDL_Rect *rect20, SDL12_Rect *rect12)
{
    rect12->x = (Sint16) rect20->x;
    rect12->y = (Sint16) rect20->y;
    rect12->w = (Uint16) ((rect20->w <= 0) ? 0 : rect20->w);
    rect12->h = (Uint16) ((rect20->h <= 0) ? 0 : rect20->h);
    return rect12;
}

static SDL_Rect *
Rect12to20(const SDL12_Rect *rect12, SDL_Rect *rect20)
{
    rect20->x = (int) rect12->x;
    rect20->y = (int) rect12->y;
    rect20->w = (int) rect12->w;
    rect20->h = (int) rect12->h;
    return rect20;
}

static SDL12_Surface *
Surface20to12(SDL_Surface *surface20)
{
    SDL_BlendMode blendmode = SDL_BLENDMODE_NONE;
    SDL12_Surface *surface12 = NULL;
    SDL12_Palette *palette12 = NULL;
    SDL12_PixelFormat *format12 = NULL;
    Uint32 flags = 0;

    if (!surface20) {
        return NULL;
    } else if (surface20->pitch > 65535) {
        SDL20_SetError("Pitch is too large");  /* can't fit to 16-bits */
        return NULL;
    }

    surface12 = (SDL12_Surface *) SDL20_malloc(sizeof (SDL12_Surface));
    if (!surface12) {
        goto failed;
    }

    if (surface20->format->palette) {
        palette12 = (SDL12_Palette *) SDL20_malloc(sizeof (SDL12_Palette));
        if (!palette12) {
            goto failed;
        }
    }

    format12 = (SDL12_PixelFormat *) SDL20_malloc(sizeof (SDL12_PixelFormat));
    if (!format12) {
        goto failed;
    }

    if (palette12) {
        SDL20_zerop(palette12);
        SDL_assert(surface20->format->palette);
        palette12->ncolors = surface20->format->palette->ncolors;
        palette12->colors = surface20->format->palette->colors;
    }

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

    if (SDL20_GetColorKey(surface20, &format12->colorkey) < 0) {
        format12->colorkey = 0;
    } else {
        surface12->flags |= SDL12_SRCCOLORKEY;
    }

    if (SDL20_GetSurfaceAlphaMod(surface20, &format12->alpha) < 0) {
        format12->alpha = 255;
    }

    SDL20_zerop(surface12);
    flags = surface20->flags;
    flags &= ~SDL_SIMD_ALIGNED;  /* we don't need to map this to 1.2 */
    #define MAPSURFACEFLAGS(fl) { if (surface20->flags & SDL_##fl) { surface12->flags |= SDL12_##fl; flags &= ~SDL_##fl; } }
    MAPSURFACEFLAGS(PREALLOC);
    MAPSURFACEFLAGS(RLEACCEL);
    /*MAPSURFACEFLAGS(DONTFREE);*/
    #undef MAPSURFACEFLAGS
    SDL_assert(flags == 0);  /* non-zero if there's a flag we didn't map. */

    if ((SDL20_GetSurfaceBlendMode(surface20, &blendmode) == 0) && (blendmode == SDL_BLENDMODE_BLEND)) {
        surface12->flags |= SDL12_SRCALPHA;
    }

    surface12->format = format12;
    surface12->w = surface20->w;
    surface12->h = surface20->h;
    surface12->pitch = (Uint16) surface20->pitch;
    surface12->pixels = surface20->pixels;
    surface12->offset = 0;
    surface12->surface20 = surface20;
    Rect20to12(&surface20->clip_rect, &surface12->clip_rect);
    surface12->refcount = surface20->refcount;

    return surface12;

failed:
    SDL20_free(surface12);
    SDL20_free(palette12);
    SDL20_free(format12);
    return NULL;
}

static void
SetPalette12ForMasks(SDL12_Surface *surface12, const Uint32 Rmask, const Uint32 Gmask, const Uint32 Bmask)
{
    SDL12_PixelFormat *format12;
    SDL_Color *color;
    int i, ncolors;

    format12 = surface12->format;
    if (format12->palette && (Rmask || Bmask || Gmask)) {
        int Rw, Rm, Gw, Gm, Bw, Bm;
        #define LOSSMASKSHIFTSETUP(t) { \
            format12->t##shift = 0; \
            format12->t##loss = 8; \
            if (t##mask) { \
                Uint32 mask; \
                for (mask = t##mask; !(mask & 1); mask >>= 1) { \
                    format12->t##shift++; \
                } \
                while (mask & 1) { \
                    format12->t##loss--; \
                    mask >>= 1; \
                } \
            } \
            format12->t##mask = t##mask; \
        }
        LOSSMASKSHIFTSETUP(R);
        LOSSMASKSHIFTSETUP(G);
        LOSSMASKSHIFTSETUP(B);
        #undef LOSSMASKSHIFTSETUP
        format12->Amask = 0;
        format12->Ashift = 0;
        format12->Aloss = 8;

        #define MASKSETUP(t) { \
            t##w = 0, t##m = 0; \
            if (t##mask) { \
                t##w = 8 - format12->t##loss; \
                for (i = format12->t##loss; i > 0; i -= t##w) { \
                    t##m |= 1 << i; \
                } \
            } \
        }
        MASKSETUP(R);
        MASKSETUP(G);
        MASKSETUP(B);
        #undef MASKSETUP

        ncolors = format12->palette->ncolors;
        color = format12->palette->colors;
        for (i = 0; i < ncolors; i++, color++) {
            #define SETCOLOR(T, t) { \
                const int x = (i & T##mask) >> format12->T##shift; \
                color->t = (x << format12->T##loss) | ((x * T##m) >> T##w); \
            }
            SETCOLOR(R, r);
            SETCOLOR(G, g);
            SETCOLOR(B, b);
            #undef SETCOLOR
            color->a = 255;
        }

    }
}

DECLSPEC12 SDL12_Surface * SDLCALL
SDL_CreateRGBSurface(Uint32 flags12, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    SDL_Surface *surface20;
    SDL12_Surface *surface12;

    /* SDL 1.2 checks this. */
    if ((width >= 16384) || (height >= 65536)) {
        SDL20_SetError("Width or height is too large");
        return NULL;
    }

    /* !!! FIXME: this isn't strictly correct, but SDL2 doesn't support
       !!! FIXME: arbitrary depths smaller than 8 bits, and this lets at
       !!! FIXME:  least one game (rockdodger) function correctly. */
    if (depth < 8 && depth != 1 && depth != 4) {
        if (WantDebugLogging) {
            SDL20_Log("This app is creating an %d-bit SDL_Surface, but we are bumping it to 8-bits. If you see rendering issues, please report them!", depth);
        }
        depth = 8;
    }

    if (depth <= 8) {  /* don't pass masks to SDL2 for <= 8-bit surfaces, it'll cause problems. */
        surface20 = SDL20_CreateRGBSurface(0, width, height, depth, 0, 0, 0, 0);
    } else {
        surface20 = SDL20_CreateRGBSurface(0, width, height, depth, Rmask, Gmask, Bmask, Amask);
    }

    /* SDL 1.2 would make a surface from almost any masks, even if it doesn't
       make sense; specifically, it will make a surface if a color mask is
       bogus. Sometimes this even worked because it would eventually land in
       a generic blitter that just copied data blindly. SDL2 wants more strict
       pixel formats, so try to detect this case and try again with a standard
       format. */
    if ((surface20 == NULL) && (depth >= 16) && (SDL20_MasksToPixelFormatEnum(depth, Rmask, Gmask, Bmask, Amask) == SDL_PIXELFORMAT_UNKNOWN)) {
        /* I have no illusions this is correct, it just works for the known problem cases so far. */
        if (depth == 16) {
            Rmask = SDL_SwapLE32(0x0000F800);
            Gmask = SDL_SwapLE32(0x000007E0);
            Bmask = SDL_SwapLE32(0x0000001F);
            Amask = 0;
        } else {
            Rmask = SDL_SwapLE32(0x000000FF);
            Gmask = SDL_SwapLE32(0x0000FF00);
            Bmask = SDL_SwapLE32(0x00FF0000);
            Amask = SDL_SwapLE32(Amask ? 0xFF000000 : 0x00000000);
        }
        surface20 = SDL20_CreateRGBSurface(0, width, height, depth, Rmask, Gmask, Bmask, Amask);
    }

    surface12 = Surface20to12(surface20);
    if (!surface12) {
        SDL20_FreeSurface(surface20);
        return NULL;
    }

    SDL_assert((surface12->flags & ~(SDL12_SRCCOLORKEY|SDL12_SRCALPHA)) == 0);  /* shouldn't have prealloc, rleaccel, or dontfree. */

    SetPalette12ForMasks(surface12, Rmask, Gmask, Bmask);

    if (flags12 & SDL12_SRCALPHA) {
        surface12->flags |= SDL12_SRCALPHA;
        SDL20_SetSurfaceBlendMode(surface20, SDL_BLENDMODE_BLEND);
    }

    return surface12;
}

DECLSPEC12 SDL12_Surface * SDLCALL
SDL_CreateRGBSurfaceFrom(void *pixels, int width, int height, int depth, int pitch, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    SDL_Surface *surface20;
    SDL12_Surface *surface12;

    if ((width >= 16384) || (height >= 65536)) {
        SDL20_SetError("Width or height is too large");
        return NULL;
    }

    if (depth == 8) {  /* don't pass masks to SDL2 for 8-bit surfaces, it'll cause problems. */
        surface20 = SDL20_CreateRGBSurfaceFrom(pixels, width, height, depth, pitch, 0, 0, 0, 0);
    } else {
        surface20 = SDL20_CreateRGBSurfaceFrom(pixels, width, height, depth, pitch, Rmask, Gmask, Bmask, Amask);
    }

    surface12 = Surface20to12(surface20);
    if (!surface12) {
        SDL20_FreeSurface(surface20);
        return NULL;
    }

    SDL_assert((surface12->flags & ~(SDL12_SRCCOLORKEY|SDL12_SRCALPHA)) == SDL12_PREALLOC);  /* should _only_ have prealloc. */

    SetPalette12ForMasks(surface12, Rmask, Gmask, Bmask);

    return surface12;
}

DECLSPEC12 void SDLCALL
SDL_FreeSurface(SDL12_Surface *surface12)
{
    if (surface12 && (surface12 != VideoSurface12)) {
        surface12->refcount--;
        if (surface12->refcount)
            return;
        SDL20_FreeSurface(surface12->surface20);
        if (surface12->format) {
            SDL20_free(surface12->format->palette);
            SDL20_free(surface12->format);
        }
        SDL20_free(surface12);
    }
}

DECLSPEC12 void SDLCALL
SDL_GetClipRect(SDL12_Surface *surface12, SDL12_Rect *rect)
{
    if (surface12 && rect) {
        SDL20_memcpy(rect, &surface12->clip_rect, sizeof (SDL12_Rect));
    }
}

DECLSPEC12 SDL_bool SDLCALL
SDL_SetClipRect(SDL12_Surface *surface12, const SDL12_Rect *rect12)
{
    SDL_bool retval = SDL_FALSE;
    if (surface12) {
        SDL_Rect rect20;
        retval = SDL20_SetClipRect(surface12->surface20, rect12 ? Rect12to20(rect12, &rect20) : NULL);
        SDL20_GetClipRect(surface12->surface20, &rect20);
        Rect20to12(&rect20, &surface12->clip_rect);
    }
    return retval;
}

DECLSPEC12 int SDLCALL
SDL_FillRect(SDL12_Surface *dst, SDL12_Rect *dstrect12, Uint32 color)
{
    SDL_Rect dstrect20;
    const int retval = SDL20_FillRect(dst->surface20, dstrect12 ? Rect12to20(dstrect12, &dstrect20) : NULL, color);
    if (retval == 0) {
        if (dstrect12) {  /* 1.2 stores the clip intersection in dstrect */
            SDL_Rect intersected20;
            SDL20_IntersectRect(&dstrect20, &dst->surface20->clip_rect, &intersected20);
            Rect20to12(&intersected20, dstrect12);
        }
    }
    return retval;
}

DECLSPEC12 Uint32 SDLCALL
SDL_MapRGB(const SDL12_PixelFormat *format12, Uint8 r, Uint8 g, Uint8 b)
{
    /* This is probably way slower than apps expect. */
    SDL_PixelFormat format20;
    SDL_Palette palette20;
    return SDL20_MapRGB(PixelFormat12to20(&format20, &palette20, format12), r, g, b);
}

DECLSPEC12 Uint32 SDLCALL
SDL_MapRGBA(const SDL12_PixelFormat *format12, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    /* This is probably way slower than apps expect. */
    SDL_PixelFormat format20;
    SDL_Palette palette20;
    return SDL20_MapRGBA(PixelFormat12to20(&format20, &palette20, format12), r, g, b, a);
}

DECLSPEC12 void SDLCALL
SDL_GetRGB(Uint32 pixel, const SDL12_PixelFormat *format12, Uint8 *r, Uint8 *g, Uint8 *b)
{
    /* This is probably way slower than apps expect. */
    SDL_PixelFormat format20;
    SDL_Palette palette20;
    SDL20_GetRGB(pixel, PixelFormat12to20(&format20, &palette20, format12), r, g, b);
}

DECLSPEC12 void SDLCALL
SDL_GetRGBA(Uint32 pixel, const SDL12_PixelFormat *format12, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a)
{
    /* This is probably way slower than apps expect. */
    SDL_PixelFormat format20;
    SDL_Palette palette20;
    SDL20_GetRGBA(pixel, PixelFormat12to20(&format20, &palette20, format12), r, g, b, a);
}

DECLSPEC12 const SDL12_VideoInfo * SDLCALL
SDL_GetVideoInfo(void)
{
    return VideoInfo12.vfmt ? &VideoInfo12 : NULL;
}

DECLSPEC12 int SDLCALL
SDL_VideoModeOK(int width, int height, int bpp, Uint32 sdl12flags)
{
    int i, j, actual_bpp = 0;

    if (!SDL20_WasInit(SDL_INIT_VIDEO)) {
        return 0;
    }

    /* if the 1.2 video backend could center a surface in a larger mode, it
       would accept the size. Since we scale things, we will, too, even
       without an exact width/height match. */

    if (!(sdl12flags & SDL12_FULLSCREEN)) {
        SDL_DisplayMode mode;
        SDL20_GetDesktopDisplayMode(VideoDisplayIndex, &mode);
        if ((mode.w >= width) && (mode.h >= height)) {
            actual_bpp = SDL_BITSPERPIXEL(mode.format);
        }
    } else {
        for (i = 0; i < VideoModesCount; ++i) {
            VideoModeList *vmode = &VideoModes[i];
            for (j = 0; j < vmode->nummodes; ++j) {
                if (vmode->modeslist12[j].w >= width && vmode->modeslist12[j].h >= height) {
                    if (!vmode->format) {
                        return bpp;
                    }
                    if (SDL_BITSPERPIXEL(vmode->format) == 24 && bpp == 32) {
                        actual_bpp = 32;
                    } else if (SDL_BITSPERPIXEL(vmode->format) >= (Uint32) bpp) {
                        actual_bpp = SDL_BITSPERPIXEL(vmode->format);
                    }
                }
            }
        }
    }

    return (actual_bpp == 24) ? 32 : actual_bpp;
}

DECLSPEC12 SDL12_Rect ** SDLCALL
SDL_ListModes(const SDL12_PixelFormat *format12, Uint32 flags)
{
    VideoModeList *best_modes = NULL;
    Uint32 bpp;
    int i;

    if (!SDL20_WasInit(SDL_INIT_VIDEO)) {
        SDL20_SetError("Video subsystem not initialized");
        return NULL;
    }

    if ((!format12) && (!VideoInfo12.vfmt)) {
        SDL20_SetError("No pixel format specified");
        return NULL;
    }

    if (IsDummyVideo) {
        return (SDL12_Rect **) -1;  /* 1.2's dummy driver always returns -1, and it's useful to special-case that. */
    }

    if (!(flags & SDL12_FULLSCREEN)) {
        return (SDL12_Rect **) -1;  /* any resolution is fine. */
    }

    if (format12 && (format12 != VideoInfo12.vfmt)) {
        bpp = (Uint32) format12->BitsPerPixel;
    } else {
        bpp = SDL_BITSPERPIXEL(VideoInfoVfmt20->format);
    }

    for (i = 0; i < VideoModesCount; i++) {
        VideoModeList *modes = &VideoModes[i];
        if (SDL_BITSPERPIXEL(modes->format) == bpp) {
            return modes->modes12;
        } else if (SDL_BITSPERPIXEL(modes->format) == 24 && bpp == 32) {
            best_modes = modes;
        } else if (SDL_BITSPERPIXEL(modes->format) > bpp) {
            if (!best_modes || SDL_BITSPERPIXEL(modes->format) > SDL_BITSPERPIXEL(best_modes->format)) {
                best_modes = modes;
            }
        }
    }

    if (!best_modes) {
        SDL20_SetError("No modes support requested pixel format");
        return NULL;
    }
    return best_modes->modes12;
}

DECLSPEC12 void SDLCALL
SDL_FreeCursor(SDL12_Cursor *cursor12)
{
    if (cursor12 == CurrentCursor12) {
        CurrentCursor12 = NULL;
    }
    if (cursor12) {
        if (cursor12->wm_cursor) {
            SDL20_FreeCursor(cursor12->wm_cursor);
        }
        SDL20_free(cursor12->data);
        SDL20_free(cursor12->mask);
        SDL20_free(cursor12);
    }
}

DECLSPEC12 SDL12_Cursor * SDLCALL
SDL_CreateCursor(Uint8 *data, Uint8 *mask, int w, int h, int hot_x, int hot_y)
{
    const size_t datasize = h * (w / 8);
    SDL_Cursor *cursor20 = NULL;
    SDL12_Cursor *retval = NULL;

    retval = (SDL12_Cursor *) SDL20_malloc(sizeof (SDL12_Cursor));
    if (!retval) {
        goto outofmem;
    }

    SDL20_zerop(retval);

    retval->data = (Uint8 *) SDL20_malloc(datasize);
    if (!retval->data) {
        goto outofmem;
    }

    retval->mask = (Uint8 *) SDL20_malloc(datasize);
    if (!retval->mask) {
        goto outofmem;
    }

    cursor20 = SDL20_CreateCursor(data, mask, w, h, hot_x, hot_y);
    if (!cursor20) {
        goto failed;
    }

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

DECLSPEC12 void SDLCALL
SDL_SetCursor(SDL12_Cursor *cursor)
{
    CurrentCursor12 = cursor;
    SDL20_SetCursor(cursor ? cursor->wm_cursor : NULL);
}

DECLSPEC12 SDL12_Cursor * SDLCALL
SDL_GetCursor(void)
{
    if (!CurrentCursor12) {
        CurrentCursor12 = SDL_CreateCursor(default_cdata, default_cmask,
                            DEFAULT_CWIDTH, DEFAULT_CHEIGHT, DEFAULT_CHOTX, DEFAULT_CHOTY);
    }
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
    } else {
        *x = SDL_WINDOWPOS_UNDEFINED_DISPLAY(display);
        *y = SDL_WINDOWPOS_UNDEFINED_DISPLAY(display);
    }
}

static SDL12_Surface *
EndVidModeCreate(void)
{
    QueuedOverlayItem *overlay;

    if (OpenGLBlitTexture) {
        OpenGLFuncs.glDeleteTextures(1, &OpenGLBlitTexture);
        OpenGLBlitTexture = 0;
    }
    if (VideoTexture20) {
        SDL20_DestroyTexture(VideoTexture20);
        VideoTexture20 = NULL;
    }
    if (VideoRenderer20) {
        SDL20_DestroyRenderer(VideoRenderer20);
        VideoRenderer20 = NULL;
    }
    if (VideoRendererLock) {
        SDL20_DestroyMutex(VideoRendererLock);
        VideoRendererLock = NULL;
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
    if (VideoPhysicalPalette20) {
        SDL20_FreePalette(VideoPhysicalPalette20);
        VideoPhysicalPalette20 = NULL;
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

    SDL_zero(OpenGLFuncs);
    OpenGLBlitLockCount = 0;
    OpenGLLogicalScalingWidth = 0;
    OpenGLLogicalScalingHeight = 0;
    OpenGLLogicalScalingFBO = 0;
    OpenGLLogicalScalingColor = 0;
    OpenGLLogicalScalingDepth = 0;

    MouseInputIsRelative = SDL_FALSE;
    MousePosition.x = 0;
    MousePosition.y = 0;

    overlay = QueuedDisplayOverlays.next;
    while (overlay != NULL) {
        QueuedOverlayItem *next = overlay->next;
        SDL_free(overlay);
        overlay = next;
    }
    QueuedDisplayOverlays.next = NULL;
    QueuedDisplayOverlaysTail = &QueuedDisplayOverlays;

    VideoSurfaceUpdatedInBackgroundThread = SDL_FALSE;
    SetVideoModeThread = 0;

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

static void
LoadOpenGLFunctions(void)
{
    const char *version;
    int major = 0, minor = 0;

    /* load core functions so we can guess about a few other things. */
    SDL_zero(OpenGLFuncs);
    OpenGLFuncs.SUPPORTS_Core = SDL_TRUE;
    #define OPENGL_SYM(ext,rc,fn,params,args,ret) OpenGLFuncs.fn = \
           (OpenGLFuncs.SUPPORTS_##ext)? (openglfn_##fn##_t)SDL20_GL_GetProcAddress(#fn) : NULL;
    #include "SDL20_syms.h"

    version = (const char *) OpenGLFuncs.glGetString(GL_VERSION);
    if (!version || (SDL20_sscanf(version, "%d.%d", &major, &minor) != 2)) {
        major = minor = 0;
    }

    /* Lookup reported extensions. */
    #define OPENGL_EXT(name) OpenGLFuncs.SUPPORTS_##name = SDL20_GL_ExtensionSupported(#name);
    #include "SDL20_syms.h"

    /* GL_ARB_framebuffer_object is in core OpenGL 3.0+ with the same entry point names as the extension version. */
    if (major >= 3) {
        OpenGLFuncs.SUPPORTS_GL_ARB_framebuffer_object = SDL_TRUE;
    }

    if (major >= 2) {
        OpenGLFuncs.SUPPORTS_GL_ARB_texture_non_power_of_two = SDL_TRUE;  /* core since 2.0 */
    }

    /* load everything we can. */
    #define OPENGL_SYM(ext,rc,fn,params,args,ret) OpenGLFuncs.fn = \
           (OpenGLFuncs.SUPPORTS_##ext)? (openglfn_##fn##_t)SDL20_GL_GetProcAddress(#fn) : NULL;
    #include "SDL20_syms.h"
}

static void
ResolveFauxBackbufferMSAA()
{
    const GLboolean has_scissor = OpenGLFuncs.glIsEnabled(GL_SCISSOR_TEST);

    if (has_scissor) {
        OpenGLFuncs.glDisable(GL_SCISSOR_TEST);  /* scissor test affects framebuffer_blit */
    }

    OpenGLFuncs.glBindFramebuffer(GL_READ_FRAMEBUFFER, OpenGLLogicalScalingFBO);
    OpenGLFuncs.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGLLogicalScalingMultisampleFBO);
    OpenGLFuncs.glBlitFramebuffer(0, 0, OpenGLLogicalScalingWidth, OpenGLLogicalScalingHeight,
                                  0, 0, OpenGLLogicalScalingWidth, OpenGLLogicalScalingHeight,
                                  GL_COLOR_BUFFER_BIT, GL_NEAREST);

    if (has_scissor) {
        OpenGLFuncs.glEnable(GL_SCISSOR_TEST);
    }
    OpenGLFuncs.glBindFramebuffer(GL_READ_FRAMEBUFFER, OpenGLCurrentReadFBO);
    OpenGLFuncs.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGLCurrentDrawFBO);
}

/* if the app binds its own framebuffer objects, it'll try to bind the window framebuffer
   (always zero) to draw to the screen, but if we're using a framebuffer object to handle
   scaling, we need to catch those binds and make sure rendering intended for the window
   framebuffer is redirected to our scaling FBO, so we have SDL_GL_GetProcAddress() hand out
   a shim for glBindFramebuffer to catch this. */
static void GLAPIENTRY
glBindFramebuffer_shim_for_scaling(GLenum target, GLuint name)
{
    /* OpenGLLogicalScaling{Multisample,}FBO will be zero if we aren't scaling, making this use the default. */

    /* We always cache the current framebuffer when it's changed here, so we don't have
       to repeatedly ask OpenGL what the current framebuffer is, and so we can only force
       resolves if the currently bound read FBO is the default. */
    if ((target == GL_READ_FRAMEBUFFER) || (target == GL_FRAMEBUFFER)) {
        if (OpenGLLogicalScalingMultisampleFBO) {
            /* We need to read from a resolves FBO if multisampling is enabled. */
            OpenGLCurrentReadFBO = (name == 0) ? OpenGLLogicalScalingMultisampleFBO : name;
        } else {
            OpenGLCurrentReadFBO = (name == 0) ? OpenGLLogicalScalingFBO : name;
        }
    }

    if ((target == GL_DRAW_FRAMEBUFFER) || (target == GL_FRAMEBUFFER)) {
        OpenGLCurrentDrawFBO = (name == 0) ? OpenGLLogicalScalingFBO : name;
    }

    /* If multisampling is enabled, and we're trying using the default framebuffer, do a multisample resolve. */
    if (OpenGLLogicalScalingMultisampleFBO && (OpenGLCurrentReadFBO == OpenGLLogicalScalingMultisampleFBO)) {
        ResolveFauxBackbufferMSAA();
    } else {
        /* ResolveFauxBackbufferMSAA() will bind the framebuffers, otherwise we have to do it manually */
        OpenGLFuncs.glBindFramebuffer(GL_READ_FRAMEBUFFER, OpenGLCurrentReadFBO);
        OpenGLFuncs.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGLCurrentDrawFBO);
    }

}

static void GLAPIENTRY
glReadPixels_shim_for_scaling(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void *data)
{
    if (OpenGLLogicalScalingMultisampleFBO && (OpenGLCurrentReadFBO == OpenGLLogicalScalingMultisampleFBO))
        ResolveFauxBackbufferMSAA();
    OpenGLFuncs.glReadPixels(x, y, width, height, format, type, data);
}

static void GLAPIENTRY
glCopyPixels_shim_for_scaling(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type)
{
    if (OpenGLLogicalScalingMultisampleFBO && (OpenGLCurrentReadFBO == OpenGLLogicalScalingMultisampleFBO))
        ResolveFauxBackbufferMSAA();
    OpenGLFuncs.glCopyPixels(x, y, width, height, type);
}

static void GLAPIENTRY
glCopyTexImage1D_shim_for_scaling(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border)
{
    if (OpenGLLogicalScalingMultisampleFBO && (OpenGLCurrentReadFBO == OpenGLLogicalScalingMultisampleFBO))
        ResolveFauxBackbufferMSAA();
    OpenGLFuncs.glCopyTexImage1D(target, level, internalformat, x, y, width, border);
}

static void GLAPIENTRY
glCopyTexSubImage1D_shim_for_scaling(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width)
{
    if (OpenGLLogicalScalingMultisampleFBO && (OpenGLCurrentReadFBO == OpenGLLogicalScalingMultisampleFBO))
        ResolveFauxBackbufferMSAA();
    OpenGLFuncs.glCopyTexSubImage1D(target, level, xoffset, x, y, width);
}

static void GLAPIENTRY
glCopyTexImage2D_shim_for_scaling(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border)
{
    if (OpenGLLogicalScalingMultisampleFBO && (OpenGLCurrentReadFBO == OpenGLLogicalScalingMultisampleFBO))
        ResolveFauxBackbufferMSAA();
    OpenGLFuncs.glCopyTexImage2D(target, level, internalformat, x, y, width, height, border);
}

static void GLAPIENTRY
glCopyTexSubImage2D_shim_for_scaling(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    if (OpenGLLogicalScalingMultisampleFBO && (OpenGLCurrentReadFBO == OpenGLLogicalScalingMultisampleFBO))
        ResolveFauxBackbufferMSAA();
    OpenGLFuncs.glCopyTexSubImage2D(target, level, xoffset, yoffset, x, y, width, height);
}

static void GLAPIENTRY
glCopyTexSubImage3D_shim_for_scaling(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    if (OpenGLLogicalScalingMultisampleFBO && (OpenGLCurrentReadFBO == OpenGLLogicalScalingMultisampleFBO))
        ResolveFauxBackbufferMSAA();
    OpenGLFuncs.glCopyTexSubImage3D(target, level, xoffset, yoffset, zoffset, x, y, width, height);
}

static SDL_bool
InitializeOpenGLScaling(const int w, const int h)
{
    int alpha_size = 0;
    int depth_size = 0;
    int stencil_size = 0;

    SDL_assert(VideoWindow20 != NULL);

    /* Support the MOUSE_RELATIVE_SCALING hint from SDL 2.0 for OpenGL scaling. */
    UseMouseRelativeScaling = SDL12Compat_GetHintBoolean("SDL_MOUSE_RELATIVE_SCALING", SDL_TRUE);

    if (!OpenGLFuncs.SUPPORTS_GL_ARB_framebuffer_object) {
        return SDL_FALSE;  /* no FBOs, no scaling. */
    }

    OpenGLFuncs.glBindFramebuffer(GL_FRAMEBUFFER, 0);
    OpenGLFuncs.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SDL20_GL_SwapWindow(VideoWindow20);

    SDL20_GL_GetAttribute(SDL_GL_ALPHA_SIZE, &alpha_size);
    SDL20_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &depth_size);
    SDL20_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &stencil_size);

    if (!OpenGLLogicalScalingFBO) {
        OpenGLFuncs.glGenFramebuffers(1, &OpenGLLogicalScalingFBO);
    }

    if (!OpenGLLogicalScalingColor) {
        OpenGLFuncs.glGenRenderbuffers(1, &OpenGLLogicalScalingColor);
    }

    if (!OpenGLLogicalScalingDepth) {
        OpenGLFuncs.glGenRenderbuffers(1, &OpenGLLogicalScalingDepth);
    }

    OpenGLFuncs.glBindFramebuffer(GL_FRAMEBUFFER, OpenGLLogicalScalingFBO);
    OpenGLFuncs.glBindRenderbuffer(GL_RENDERBUFFER, OpenGLLogicalScalingColor);
    OpenGLFuncs.glRenderbufferStorageMultisample(GL_RENDERBUFFER, OpenGLLogicalScalingSamples, (alpha_size > 0) ? GL_RGBA8 : GL_RGB8, w, h);
    OpenGLFuncs.glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, OpenGLLogicalScalingColor);

    if (depth_size || stencil_size) {
        OpenGLFuncs.glBindRenderbuffer(GL_RENDERBUFFER, OpenGLLogicalScalingDepth);
        OpenGLFuncs.glRenderbufferStorageMultisample(GL_RENDERBUFFER, OpenGLLogicalScalingSamples, GL_DEPTH24_STENCIL8, w, h);
        if (depth_size) {
            OpenGLFuncs.glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, OpenGLLogicalScalingDepth);
        }
        if (stencil_size) {
            OpenGLFuncs.glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, OpenGLLogicalScalingDepth);
        }
    }

    OpenGLFuncs.glBindRenderbuffer(GL_RENDERBUFFER, 0);

    if ((OpenGLFuncs.glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) || OpenGLFuncs.glGetError()) {
        OpenGLFuncs.glBindFramebuffer(GL_FRAMEBUFFER, 0);
        OpenGLFuncs.glDeleteRenderbuffers(1, &OpenGLLogicalScalingColor);
        OpenGLFuncs.glDeleteRenderbuffers(1, &OpenGLLogicalScalingDepth);
        OpenGLFuncs.glDeleteFramebuffers(1, &OpenGLLogicalScalingFBO);
        OpenGLLogicalScalingFBO = OpenGLLogicalScalingColor = OpenGLLogicalScalingDepth = 0;
        return SDL_FALSE;
    }

    if (OpenGLLogicalScalingSamples) {
        if (!OpenGLLogicalScalingMultisampleFBO) {
            OpenGLFuncs.glGenFramebuffers(1, &OpenGLLogicalScalingMultisampleFBO);
        }
        if (!OpenGLLogicalScalingMultisampleColor) {
            OpenGLFuncs.glGenRenderbuffers(1, &OpenGLLogicalScalingMultisampleColor);
        }
        if (!OpenGLLogicalScalingMultisampleDepth) {
            OpenGLFuncs.glGenRenderbuffers(1, &OpenGLLogicalScalingMultisampleDepth);
        }

        OpenGLFuncs.glBindFramebuffer(GL_FRAMEBUFFER, OpenGLLogicalScalingMultisampleFBO);
        OpenGLFuncs.glBindRenderbuffer(GL_RENDERBUFFER, OpenGLLogicalScalingMultisampleColor);
        OpenGLFuncs.glRenderbufferStorage(GL_RENDERBUFFER, (alpha_size > 0) ? GL_RGBA8 : GL_RGB8, w, h);
        OpenGLFuncs.glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, OpenGLLogicalScalingMultisampleColor);

        if (depth_size || stencil_size) {
            OpenGLFuncs.glBindRenderbuffer(GL_RENDERBUFFER, OpenGLLogicalScalingMultisampleDepth);
            OpenGLFuncs.glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
            if (depth_size) {
                OpenGLFuncs.glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, OpenGLLogicalScalingMultisampleDepth);
            }
            if (stencil_size) {
                OpenGLFuncs.glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, OpenGLLogicalScalingMultisampleDepth);
            }
        }

        OpenGLFuncs.glBindRenderbuffer(GL_RENDERBUFFER, 0);

        if ((OpenGLFuncs.glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) || OpenGLFuncs.glGetError()) {
            OpenGLFuncs.glBindFramebuffer(GL_FRAMEBUFFER, 0);
            OpenGLFuncs.glDeleteRenderbuffers(1, &OpenGLLogicalScalingMultisampleColor);
            OpenGLFuncs.glDeleteRenderbuffers(1, &OpenGLLogicalScalingMultisampleDepth);
            OpenGLFuncs.glDeleteFramebuffers(1, &OpenGLLogicalScalingMultisampleFBO);
            OpenGLLogicalScalingMultisampleFBO = OpenGLLogicalScalingMultisampleColor = OpenGLLogicalScalingMultisampleDepth = 0;
        }
        OpenGLFuncs.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGLLogicalScalingFBO);
        OpenGLFuncs.glBindFramebuffer(GL_READ_FRAMEBUFFER, OpenGLLogicalScalingMultisampleFBO);
    }

    /* initialise the cached current FBO bindings properly */
    OpenGLCurrentReadFBO = OpenGLLogicalScalingMultisampleFBO ? OpenGLLogicalScalingMultisampleFBO : OpenGLLogicalScalingFBO;
    OpenGLCurrentDrawFBO = OpenGLLogicalScalingFBO;

    OpenGLFuncs.glViewport(0, 0, w, h);
    OpenGLFuncs.glScissor(0, 0, w, h);
    OpenGLLogicalScalingWidth = w;
    OpenGLLogicalScalingHeight = h;

    OpenGLFuncs.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    return SDL_TRUE;
}


/* The idea here is that SDL's OpenGL-based renderers always notice if
   they aren't using the correct context and attempt to set the correct
   context before doing any work, but (at least for X11, and probably
   other platforms), the current context is thread-local, and _it's an
   error to set a context current if it's already current on another thread_.
   So we try to catch every place we call into the renderer and end that
   work with a call to this function, which will reset the GL context to NULL,
   so if an app tries to render from a background thread, the GL renderer
   will be able to set the context, do it's work, and then we reset it right
   after. Without this, we either need all apps to render exclusively on
   the main thread or fail to draw at all.

   This feels risky, but it's better than the alternative!

   Also, the renderer API isn't thread safe in general, so wrapping it
   in a mutex is necessary anyhow. */

static SDL_Renderer *
LockVideoRenderer(void)
{
    SDL20_LockMutex(VideoRendererLock);
    return VideoRenderer20;
}

static void
UnlockVideoRenderer(void)
{
    if ((VideoRenderer20 != NULL) && (SDL20_GL_GetCurrentContext() != NULL)) {
        SDL20_GL_MakeCurrent(NULL, NULL);
    }
    SDL20_UnlockMutex(VideoRendererLock);
}

static void HandleInputGrab(SDL12_GrabMode mode);

static SDL12_Surface *
SetVideoModeImpl(int width, int height, int bpp, Uint32 flags12)
{
    SDL_DisplayMode dmode;
    Uint32 fullscreen_flags20 = 0;
    Uint32 appfmt;
    const char *vsync_env = SDL12Compat_GetHint("SDL12COMPAT_SYNC_TO_VBLANK");
    float window_size_scaling = SDL12Compat_GetHintFloat("SDL12COMPAT_WINDOW_SCALING", 1.0f);
    SDL_bool use_gl_scaling = SDL_FALSE;
    SDL_bool use_highdpi = SDL_TRUE;
    SDL_bool fix_bordless_fs_win = SDL_TRUE;
    int scaled_width = width;
    int scaled_height = height;
    const char *fromwin_env = NULL;

    if (flags12 & SDL12_OPENGL) {
        /* For now we default GL scaling to ENABLED. If an app breaks or is linked directly
           to glBindFramebuffer, they'll need to turn it off with this environment variable.

           The reason we have a toggle to prevent this is because an app might use
           FBOs directly, and will cause this to break if they bind Framebuffer 0 instead
           of our render target. If we can fool them into calling a fake glBindFramebuffer
           that binds our logical FBO instead of the window framebuffer, we can probably
           work with these apps, too. That's easy from SDL_GL_GetProcAddress, but we
           would need to export the symbol from here too, for those that link against
           OpenGL directly, and we don't want to risk that.

           UT2004 is known to use FBOs with SDL 1.2, and I assume idTech 4 games (Doom 3,
           Quake 4, Prey) do as well. */
        use_gl_scaling = WantOpenGLScaling;

        /* default use_highdpi to false for OpenGL windows when not using
           OpenGL scaling as legacy OpenGL applications are unlikely to support
           high-DPI setups properly. (They often use the window size to determine
           the resolution in some or all parts of their code.) Because OpenGL scaling
           is never used for windows, it is always false there. */
        use_highdpi = (flags12 & SDL12_FULLSCREEN) ? use_gl_scaling : SDL_FALSE;
    }

    use_highdpi = SDL12Compat_GetHintBoolean("SDL12COMPAT_HIGHDPI", use_highdpi);

    fix_bordless_fs_win = SDL12Compat_GetHintBoolean("SDL12COMPAT_FIX_BORDERLESS_FS_WIN", fix_bordless_fs_win);

    ForceGLSwapBufferContext = SDL12Compat_GetHintBoolean("SDL12COMPAT_FORCE_GL_SWAPBUFFER_CONTEXT", SDL_FALSE);

    flags12 &= ~SDL12_HWACCEL; /* just in case - https://github.com/libsdl-org/SDL-1.2/issues/817 */

    /* SDL_SetVideoMode() implicitly inits if necessary. */
    if (SDL20_WasInit(SDL_INIT_VIDEO) == 0) {
        if (SDL20_Init(SDL_INIT_VIDEO) < 0) {
            return NULL;
        }
        if (Init12Video() < 0) {
            return NULL;
        }
    }

    if ((width < 0) || (height < 0)) {
        SDL20_SetError("Invalid width or height");
        return NULL;
    }

    if (SDL20_GetCurrentDisplayMode(VideoDisplayIndex, &dmode) < 0) {
        return NULL;
    }

    if (width == 0) {
        width = dmode.w;
    }

    if (height == 0) {
        height = dmode.h;
    }

    if (bpp == 0) {
        flags12 |= SDL12_ANYFORMAT;
        bpp = SDL_BITSPERPIXEL(dmode.format);
        /* keep this simple: we aren't handling palettes here, so for < 16-bit
           formats, give them 16-bit and we'll convert later. Nothing in SDL 1.2 will
           handle > 32 bits, so clamp there, too. AND ALSO, most apps will handle 32-bits
           but not 24, so force around that...so basically, you can have 16 or 32 bit. */
        bpp = (bpp <= 16) ? 16 : 32;
    }

    if ((bpp != 8) && (bpp != 16) && (bpp != 24) && (bpp != 32)) {
        if (flags12 & SDL12_ANYFORMAT) {
            bpp = 32;
        } else {
            SDL20_SetError("Unsupported bits-per-pixel");
            return NULL;
        }
    }

    #if !SDL_VERSION_ATLEAST(2,0,14)
    #define SDL_PIXELFORMAT_XRGB8888 SDL_PIXELFORMAT_RGB888
    #endif
    switch (bpp) {
        case  8: appfmt = SDL_PIXELFORMAT_INDEX8; break;
        case 16: appfmt = SDL_PIXELFORMAT_RGB565; break;
        case 24: appfmt = SDL_PIXELFORMAT_BGR24; break;
        case 32: appfmt = SDL_PIXELFORMAT_XRGB8888; break;
        default: SDL20_SetError("Unsupported bits-per-pixel"); return NULL;
    }

    SDL_assert((VideoSurface12 != NULL) == (VideoWindow20 != NULL));

    if (VideoSurface12 && ((VideoSurface12->flags & SDL12_OPENGL) != (flags12 & SDL12_OPENGL))) {
        EndVidModeCreate();  /* rebuild the window if moving to/from a GL context */
    } else if (VideoSurface12 && (VideoSurface12->surface20->format->format != appfmt)) {
        EndVidModeCreate();  /* rebuild the window if changing pixel format */
    } else if (VideoGLContext20) {
        /* SDL 1.2 (infuriatingly!) destroys the GL context on each resize in some cases, on various platforms. Try to match that. */
        #ifdef __WINDOWS__
            /* The windx5 driver _always_ destroyed the GL context, unconditionally, but the default (windib) did not, so match windib.
             *  windib: keep if:
             *   - window already exists
             *   - BitsPerPixel hasn't changed
             *   - none of the window flags (except SDL_ANYFORMAT) have changed
             *   - window is already SDL_OPENGL.
             *   - window is not a fullscreen window.
             */
            const SDL_bool destroy_gl_context = (
                ((VideoSurface12->flags & ~SDL12_ANYFORMAT) != (flags12 & ~SDL12_ANYFORMAT)) ||
                (VideoSurface12->format->BitsPerPixel != bpp) ||
                ((flags12 & SDL12_OPENGL) != SDL12_OPENGL) ||
                ((flags12 & SDL12_FULLSCREEN) == SDL12_FULLSCREEN)
            ) ? SDL_TRUE : SDL_FALSE;
        #elif defined(__APPLE__)
            const SDL_bool destroy_gl_context = SDL_TRUE; /* macOS ("quartz" backend) unconditionally destroys the GL context */
        #elif defined(__HAIKU__)
            const SDL_bool destroy_gl_context = SDL_FALSE; /* BeOS and Haiku ("bwindow" backend) unconditionally keeps the GL context */
        #elif defined(__LINUX__) || defined(unix) || defined(__unix__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(sun)
            /* The x11 backend does in _some_ cases, and since Linux software depends on that, even though Wayland and
             *   such wasn't a thing at the time, we treat everything that looks a little like Unix this way.
             *  x11: keep if:
             *   - window already exists
             *   - window is already SDL_OPENGL.
             *   - new flags also want SDL_OPENGL.
             *   - BitsPerPixel hasn't changed
             *   - SDL_NOFRAME hasn't changed
             */
            const SDL_bool destroy_gl_context = (
                ((VideoSurface12->flags & SDL12_OPENGL) != (flags12 & SDL12_OPENGL)) ||
                ((flags12 & SDL12_OPENGL) != SDL12_OPENGL) ||
                (VideoSurface12->format->BitsPerPixel != bpp) ||
                ((VideoSurface12->flags & SDL12_NOFRAME) != (flags12 & SDL12_NOFRAME))
            ) ? SDL_TRUE : SDL_FALSE;
        #else
            const SDL_bool destroy_gl_context = SDL_TRUE;  /* everywhere else: nuke it from orbit. Oh well. */
        #endif

        if (destroy_gl_context) {
            SDL20_GL_MakeCurrent(NULL, NULL);
            SDL20_GL_DeleteContext(VideoGLContext20);
            VideoGLContext20 = NULL;
            SDL_zero(OpenGLFuncs);
            OpenGLBlitTexture = 0;
            OpenGLBlitLockCount = 0;
            OpenGLLogicalScalingWidth = 0;
            OpenGLLogicalScalingHeight = 0;
            OpenGLLogicalScalingFBO = 0;
            OpenGLLogicalScalingColor = 0;
            OpenGLLogicalScalingDepth = 0;
            OpenGLLogicalScalingMultisampleFBO = 0;
            OpenGLLogicalScalingMultisampleColor = 0;
            OpenGLLogicalScalingMultisampleDepth = 0;
        }
    }

    if (flags12 & SDL12_FULLSCREEN) {
        /* For software rendering, we're just going to push it off onto the
            GPU, so use FULLSCREEN_DESKTOP and logical scaling there.
            If possible, we'll do this with OpenGL, too, but we might not be
            able to. */
        if (use_gl_scaling || ((flags12 & SDL12_OPENGL) == 0) || ((dmode.w == width) && (dmode.h == height))) {
            fullscreen_flags20 |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        } else {
            fullscreen_flags20 |= SDL_WINDOW_FULLSCREEN;
        }
    } else if (fix_bordless_fs_win && (flags12 & SDL12_NOFRAME) && (width == dmode.w) && (height == dmode.h)) {
        /* app appears to be trying to do a "borderless fullscreen windowed" mode, so just bump
           it to FULLSCREEN_DESKTOP so it cooperates with various display managers
           (into a fullscreen space on macOS, hide the Dock on Gnome, etc). */
        fullscreen_flags20 |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    fromwin_env = SDL20_getenv("SDL_WINDOWID");

    if (fromwin_env) {
        window_size_scaling = 1.0f;  /* don't scale for external windows */
    } else if (window_size_scaling <= 0.0f) {
        window_size_scaling = 1.0f;  /* bogus value, reset to default */
    } else if (flags12 & SDL12_RESIZABLE) {
        window_size_scaling = 1.0f;  /* assume that resizable windows are already prepared to handle whatever without scaling. */
    } else if ((fullscreen_flags20 & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0) {
        window_size_scaling = 1.0f;  /* setting the window to fullscreen or fullscreen_desktop? Don't scale it. */
    } else if ((flags12 & SDL12_OPENGL) && !use_gl_scaling) {
        window_size_scaling = 1.0f;  /* OpenGL but not doing GL scaling? Don't allow window size scaling. */
    } else {
        scaled_width = (int) (window_size_scaling * width);
        scaled_height = (int) (window_size_scaling * height);
    }

    if (fromwin_env) {
        char *endp = NULL;
        const Uint64 windowid = SDL_strtoull(fromwin_env, &endp, 0);
        if ((*fromwin_env == '\0') || (*endp != '\0')) {
            SDL20_SetError("Invalid SDL_WINDOWID");
            return EndVidModeCreate();
        } else {
            EndVidModeCreate();
            VideoWindow20 = SDL20_CreateWindowFrom((void *) (size_t) (windowid));
            if (!VideoWindow20) {
                return EndVidModeCreate();
            }
        }
    } else if (!VideoWindow20) {  /* create it */
        int x = SDL_WINDOWPOS_UNDEFINED, y = SDL_WINDOWPOS_UNDEFINED;
        Uint32 flags20 = fullscreen_flags20;
        if (flags12 & SDL12_OPENGL) { flags20 |= SDL_WINDOW_OPENGL; }
        if (flags12 & SDL12_RESIZABLE) { flags20 |= SDL_WINDOW_RESIZABLE; }
        if (flags12 & SDL12_NOFRAME) { flags20 |= SDL_WINDOW_BORDERLESS; }
        if (use_highdpi) { flags20 |= SDL_WINDOW_ALLOW_HIGHDPI; }

        /* most platforms didn't check these environment variables, but the major
           ones did (x11, windib, quartz), so we'll just offer it everywhere. */
        GetEnvironmentWindowPosition(&x, &y);

        /* If GL scaling is disabled, and a multisampled buffer is requested, do it. */
        if (!use_gl_scaling && (flags12 & SDL12_OPENGL) && OpenGLLogicalScalingSamples) {
            SDL20_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
            SDL20_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, OpenGLLogicalScalingSamples);
        }

        VideoWindow20 = SDL20_CreateWindow(WindowTitle, x, y, scaled_width, scaled_height, flags20);
        if (!VideoWindow20) {
            return EndVidModeCreate();
        }
        if (VideoIcon20) {
            SDL20_SetWindowIcon(VideoWindow20, VideoIcon20);
        }
    } else {  /* resize it */
        SDL20_SetWindowSize(VideoWindow20, scaled_width, scaled_height);
        SDL20_SetWindowFullscreen(VideoWindow20, fullscreen_flags20);
        /* This second SetWindowSize is a workaround for an SDL2 bug, see https://github.com/libsdl-org/sdl12-compat/issues/148 */
        SDL20_SetWindowSize(VideoWindow20, scaled_width, scaled_height);
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

    if (flags12 & SDL12_FULLSCREEN) {
        VideoSurface12->flags |= SDL12_FULLSCREEN;
    } else {
        VideoSurface12->flags &= ~SDL12_FULLSCREEN;
    }

    if (flags12 & SDL12_OPENGL) {
        SDL_assert(!VideoTexture20);  /* either a new window or we destroyed all this */
        SDL_assert(!VideoRenderer20);

        if (!VideoGLContext20) {
            VideoGLContext20 = SDL20_GL_CreateContext(VideoWindow20);
            if (!VideoGLContext20) {
                return EndVidModeCreate();
            }
            LoadOpenGLFunctions();
        }

        VideoSurface12->flags |= SDL12_OPENGL;

        /* Try to set up a logical scaling */
        if (use_gl_scaling) {
            if (!InitializeOpenGLScaling(width, height)) {
                const SDL_bool was_fullscreen = ((fullscreen_flags20 & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0) ? SDL_TRUE : SDL_FALSE;
                window_size_scaling = 1.0f;
                use_gl_scaling = SDL_FALSE;
                fullscreen_flags20 &= ~SDL_WINDOW_FULLSCREEN_DESKTOP;
                SDL20_SetWindowFullscreen(VideoWindow20, fullscreen_flags20);
                SDL20_SetWindowSize(VideoWindow20, width, height);  /* not scaled_width, scaled_height */
                if (was_fullscreen) {
                    fullscreen_flags20 |= SDL_WINDOW_FULLSCREEN;
                    SDL20_SetWindowFullscreen(VideoWindow20, fullscreen_flags20);
                }
            }
        }

        if ((flags12 & SDL12_OPENGLBLIT) == SDL12_OPENGLBLIT) {
            const int pixsize = VideoSurface12->format->BytesPerPixel;
            const GLenum glfmt = (pixsize == 4) ? GL_RGBA : GL_RGB;
            const GLenum gltype = (pixsize == 4) ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT_5_6_5;

            if (!OpenGLFuncs.SUPPORTS_GL_ARB_texture_non_power_of_two) {
                SDL20_SetError("Your OpenGL drivers don't support NPOT textures for SDL_OPENGLBLIT; please upgrade.");
                return EndVidModeCreate();
            }

            if (!OpenGLBlitTexture) {
                OpenGLFuncs.glGenTextures(1, &OpenGLBlitTexture);
            }
            OpenGLFuncs.glBindTexture(GL_TEXTURE_2D, OpenGLBlitTexture);
            OpenGLFuncs.glTexImage2D(GL_TEXTURE_2D, 0, (pixsize == 4) ? GL_RGBA : GL_RGB, VideoSurface12->w, VideoSurface12->h, 0, glfmt, gltype, NULL);

            VideoSurface12->surface20->pixels = SDL20_malloc(height * VideoSurface12->pitch);
            VideoSurface12->pixels = VideoSurface12->surface20->pixels;
            if (!VideoSurface12->pixels) {
                SDL20_OutOfMemory();
                return EndVidModeCreate();
            }
            SDL20_memset(VideoSurface12->pixels, 0xFF, height * VideoSurface12->pitch);  /* SDL 1.2 default OPENGLBLIT surface to full intensity */
            VideoSurface12->flags |= SDL12_OPENGLBLIT;
        }

        if (vsync_env) {
            SDL20_GL_SetSwapInterval(SDL20_atoi(vsync_env));
        } else {
            SDL20_GL_SetSwapInterval(SwapInterval);
        }

    } else {
        /* always use a renderer for non-OpenGL windows. */
        const char *old_scale_quality = SDL20_GetHint(SDL_HINT_RENDER_SCALE_QUALITY);
        const SDL_bool want_vsync = (vsync_env && SDL20_atoi(vsync_env)) ? SDL_TRUE : SDL_FALSE;
        SDL_RendererInfo rinfo;
        SDL_assert(!VideoGLContext20);  /* either a new window or we destroyed all this */
        VideoRendererLock = SDL20_CreateMutex();
        if (!VideoRendererLock) {
            return EndVidModeCreate();
        }
        if (!VideoRenderer20 && want_vsync) {
            VideoRenderer20 = SDL20_CreateRenderer(VideoWindow20, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
        }
        if (!VideoRenderer20) {
            VideoRenderer20 = SDL20_CreateRenderer(VideoWindow20, -1, SDL_RENDERER_ACCELERATED);
        }
        if (!VideoRenderer20) {
            VideoRenderer20 = SDL20_CreateRenderer(VideoWindow20, -1, 0);
        }
        if (!VideoRenderer20) {
            return EndVidModeCreate();
        }

        SDL20_RenderSetLogicalSize(VideoRenderer20, width, height);

        /* we need to make sure we're at the back of the Event Watch queue */	
        SDL20_DelEventWatch(EventFilter20to12, NULL);
        SDL20_AddEventWatch(EventFilter20to12, NULL);

        SDL20_SetRenderDrawColor(VideoRenderer20, 0, 0, 0, 255);  /* leave this black always, we only use it to clear the framebuffer. */
        SDL20_RenderClear(VideoRenderer20);
        SDL20_RenderPresent(VideoRenderer20);

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

        SDL20_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, WantScaleMethodNearest ? "0" : "1");
        VideoTexture20 = SDL20_CreateTexture(VideoRenderer20, rinfo.texture_formats[0], SDL_TEXTUREACCESS_STREAMING, width, height);
        SDL20_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, old_scale_quality);
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

        /* fill in the same default palette that SDL 1.2 does... */
        if (VideoSurface12->format->BitsPerPixel == 8) {
            int i;
            SDL_Color *color = VideoSurface12->format->palette->colors;
            for (i = 0; i < 256; i++, color++) {
                { const int x = i & 0xe0; color->r = x | x >> 3 | x >> 6; }
                { const int x = (i << 3) & 0xe0; color->g = x | x >> 3 | x >> 6; }
                { const int x = (i & 0x3) | ((i & 0x3) << 2); color->b = x | x << 4; }
                color->a = 255;
            }
            if (!VideoPhysicalPalette20) {
                VideoPhysicalPalette20 = SDL20_AllocPalette(256);
                if (!VideoPhysicalPalette20) {
                    return EndVidModeCreate();
                }
            }
            SDL20_SetPaletteColors(VideoPhysicalPalette20, VideoSurface12->format->palette->colors, 0, 256);
        }
    }

    SetVideoModeThread = SDL20_ThreadID();
    VideoSurfacePresentTicks = 0;
    VideoSurfaceLastPresentTicks = 0;
    VideoSurfaceUpdatedInBackgroundThread = SDL_FALSE;

    SDL20_RaiseWindow(VideoWindow20);

    /* SDL 1.2 always grabbed input if the video mode was fullscreen. */
    if (VideoSurface12->flags & SDL12_FULLSCREEN) {
        HandleInputGrab(SDL12_GRAB_ON);
    }

    if ((flags12 & SDL12_OPENGL) == 0) {
        /* see notes above these functions about GL context resetting. Force a lock/unlock here to set that up. */
        LockVideoRenderer();
        UnlockVideoRenderer();
    }

    SDL_PumpEvents();  /* run this once at startup. */

    return VideoSurface12;
}

DECLSPEC12 SDL12_Surface * SDLCALL
SDL_SetVideoMode(int width, int height, int bpp, Uint32 flags12)
{
    SDL12_Surface *retval;
    SetVideoModeInProgress = SDL_TRUE;
    retval = SetVideoModeImpl(width, height, bpp, flags12);
    SetVideoModeInProgress = SDL_FALSE;
    return retval;
}

DECLSPEC12 SDL12_Surface * SDLCALL
SDL_GetVideoSurface(void)
{
    return VideoSurface12;
}

static int
SaveDestAlpha(SDL12_Surface *src12, SDL12_Surface *dst12, SDL_Rect *dstrect20, Uint8 **retval)
{
    /* The 1.2 docs say this:
     * RGBA->RGBA:
     *     SDL_SRCALPHA set:
     *  alpha-blend (using the source alpha channel) the RGB values;
     *  leave destination alpha untouched. [Note: is this correct?]
     *
     * In SDL2, we change the destination alpha. We have to save it off in this case, which sucks.
     */
    Uint8 *dstalpha = NULL;
    const SDL_bool save_dstalpha = ((src12->flags & SDL12_SRCALPHA) && dst12->format->Amask && ((src12->format->alpha != 255) || src12->format->Amask)) ? SDL_TRUE : SDL_FALSE;

    if (save_dstalpha && (dstrect20->w > 0) && (dstrect20->h > 0)) {
        const Uint32 amask = dst12->format->Amask;
        const Uint32 ashift = dst12->format->Ashift;
        const Uint16 pitch = dst12->pitch;
        Uint8 *dptr;
        int x, y, w, h;

        w = dstrect20->w;
        h = dstrect20->h;

        dstalpha = (Uint8 *) SDL20_malloc(w * h);
        if (!dstalpha) {
            *retval = NULL;
            return SDL20_OutOfMemory();
        }
        dptr = dstalpha;

        if (dst12->format->BytesPerPixel == 2) {
            const Uint16 *sptr = (const Uint16 *) dst12->pixels;
            sptr += ((dst12->pitch / 2) * dstrect20->y) + dstrect20->x;
            for (y = 0; y < h; y++) {
                for (x = 0; x < w; x++) {
                    *(dptr++) = (Uint8) ((sptr[x] & amask) >> ashift);
                }
                sptr = (Uint16 *) (((Uint8 *) sptr) + pitch);
            }
        } else if (dst12->format->BytesPerPixel == 4) {
            const Uint32 *sptr = (const Uint32 *) dst12->pixels;
            sptr += ((dst12->pitch / 4) * dstrect20->y) + dstrect20->x;
            for (y = 0; y < h; y++) {
                for (x = 0; x < w; x++) {
                    *(dptr++) = (Uint8) ((sptr[x] & amask) >> ashift);
                }
                sptr = (Uint32 *) (((Uint8 *) sptr) + pitch);
            }
        } else {
            SDL_assert(!"Unhandled dest alpha");
        }
    }

    *retval = dstalpha;
    return 0;
}

static void
RestoreDestAlpha(SDL12_Surface *dst12, Uint8 *dstalpha, const SDL_Rect *dstrect20)
{
    if (dstalpha) {
        const Uint8 *sptr = dstalpha;
        const Uint32 amask = dst12->format->Amask;
        const Uint32 ashift = dst12->format->Ashift;
        const Uint16 pitch = dst12->pitch;
        int x, y, w, h;

        w = dstrect20->w;
        h = dstrect20->h;

        if (dst12->format->BytesPerPixel == 2) {
            Uint16 *dptr = (Uint16 *) dst12->pixels;
            dptr += ((dst12->pitch / 2) * dstrect20->y) + dstrect20->x;
            for (y = 0; y < h; y++) {
                for (x = 0; x < w; x++) {
                    dptr[x] = (Uint16) ((dptr[x] & ~amask) | ((((Uint16) *(sptr++)) << ashift) & amask));
                }
                dptr = (Uint16 *) (((Uint8 *) dptr) + pitch);
            }
        } else if (dst12->format->BytesPerPixel == 4) {
            Uint32 *dptr = (Uint32 *) dst12->pixels;
            dptr += ((dst12->pitch / 4) * dstrect20->y) + dstrect20->x;
            for (y = 0; y < h; y++) {
                for (x = 0; x < w; x++) {
                    dptr[x] = (dptr[x] & ~amask) | ((((Uint32) *(sptr++)) << ashift) & amask);
                }
                dptr = (Uint32 *) (((Uint8 *) dptr) + pitch);
            }
        } else {
            SDL_assert(!"Unhandled dest alpha");
        }
        SDL20_free(dstalpha);
    }
}

static void
PrepBlitDestRect(SDL_Rect *dstrect20, SDL12_Surface *dst12, const SDL12_Rect *dstrect12)
{
    /* dstrect12 w and h is ignored, SDL 1.2 only cares about position. */
    dstrect20->w = dst12->w;
    dstrect20->h = dst12->h;

    if (dstrect12) {
        SDL_Rect fulldstrect20;
        fulldstrect20.x = fulldstrect20.y = 0;
        fulldstrect20.w = dst12->w;
        fulldstrect20.h = dst12->h;
        dstrect20->x = dstrect12->x;
        dstrect20->y = dstrect12->y;
        SDL20_IntersectRect(&fulldstrect20, dstrect20, dstrect20);
    } else {
        dstrect20->x = 0;
        dstrect20->y = 0;
    }
}

DECLSPEC12 int SDLCALL
SDL_UpperBlit(SDL12_Surface *src12, SDL12_Rect *srcrect12, SDL12_Surface *dst12, SDL12_Rect *dstrect12)
{
    Uint8 *dstalpha;
    SDL_Rect srcrect20, dstrect20;
    int retval;

    PrepBlitDestRect(&dstrect20, dst12, dstrect12);

    if ((src12 == NULL) || (dst12 == NULL)) {
        return SDL20_SetError("SDL_UpperBlit: passed a NULL surface");
    } else if ((src12->pixels == NULL) || (dst12->pixels == NULL)) {
        return SDL20_SetError("SDL_UpperBlit: passed a surface with NULL pixels");
    } else if (SaveDestAlpha(src12, dst12, &dstrect20, &dstalpha) < 0) {
        return -1;
    }

    retval = SDL20_UpperBlit(src12->surface20,
                             srcrect12 ? Rect12to20(srcrect12, &srcrect20) : NULL,
                             dst12->surface20,
                             dstrect12 ? Rect12to20(dstrect12, &dstrect20) : NULL);

    RestoreDestAlpha(dst12, dstalpha, &dstrect20);

    if (dstrect12) {
        Rect20to12(&dstrect20, dstrect12);
    }

    return retval;
}

DECLSPEC12 int SDLCALL
SDL_LowerBlit(SDL12_Surface *src12, SDL12_Rect *srcrect12, SDL12_Surface *dst12, SDL12_Rect *dstrect12)
{
    Uint8 *dstalpha;
    SDL_Rect srcrect20, dstrect20;
    int retval;

    PrepBlitDestRect(&dstrect20, dst12, dstrect12);

    if (SaveDestAlpha(src12, dst12, &dstrect20, &dstalpha) < 0) {
        return -1;
    }

    retval = SDL20_LowerBlit(src12->surface20,
                             srcrect12 ? Rect12to20(srcrect12, &srcrect20) : NULL,
                             dst12->surface20,
                             dstrect12 ? Rect12to20(dstrect12, &dstrect20) : NULL);

    RestoreDestAlpha(dst12, dstalpha, &dstrect20);

    if (srcrect12) {
        Rect20to12(&srcrect20, srcrect12);
    }

    if (dstrect12) {
        Rect20to12(&dstrect20, dstrect12);
    }

    return retval;
}

DECLSPEC12 int SDLCALL
SDL_SoftStretch(SDL12_Surface *src12, SDL12_Rect *srcrect12, SDL12_Surface *dst12, SDL12_Rect *dstrect12)
{
    SDL_Rect srcrect20, dstrect20;
    return SDL20_SoftStretch(src12->surface20,
                             srcrect12 ? Rect12to20(srcrect12, &srcrect20) : NULL,
                             dst12->surface20,
                             dstrect12 ? Rect12to20(dstrect12, &dstrect20) : NULL);
}

DECLSPEC12 int SDLCALL
SDL_SetAlpha(SDL12_Surface *surface12, Uint32 flags12, Uint8 value)
{
    /* note that SDL 1.2 does not check if surface12 is NULL before dereferencing it either */
    const SDL_bool addkey = (flags12 & SDL12_SRCALPHA) ? SDL_TRUE : SDL_FALSE;
    int retval = 0;

    if (addkey) {
        if (!surface12->format->Amask) {  /* whole-surface alpha is ignored if surface has an alpha channel. */
            retval = SDL20_SetSurfaceAlphaMod(surface12->surface20, value);
            if (SDL20_GetSurfaceAlphaMod(surface12->surface20, &surface12->format->alpha) < 0) {
                surface12->format->alpha = 255;
            }
        }
        surface12->flags |= SDL12_SRCALPHA;
        SDL20_SetSurfaceBlendMode(surface12->surface20, SDL_BLENDMODE_BLEND);
    } else {
        if (!surface12->format->Amask) {  /* whole-surface alpha is ignored if surface has an alpha channel. */
            retval = SDL20_SetSurfaceAlphaMod(surface12->surface20, 255);
            if (SDL20_GetSurfaceAlphaMod(surface12->surface20, &surface12->format->alpha) < 0) {
                surface12->format->alpha = 255;
            }
        }
        surface12->flags &= ~SDL12_SRCALPHA;
        SDL20_SetSurfaceBlendMode(surface12->surface20, SDL_BLENDMODE_NONE);
    }

    return retval;
}

DECLSPEC12 int SDLCALL
SDL_LockSurface(SDL12_Surface *surface12)
{
    const int retval = SDL20_LockSurface(surface12->surface20);
    surface12->pixels = surface12->surface20->pixels;
    surface12->pitch = surface12->surface20->pitch;
    return retval;
}

DECLSPEC12 void SDLCALL
SDL_UnlockSurface(SDL12_Surface *surface12)
{
    SDL20_UnlockSurface(surface12->surface20);
    surface12->pixels = surface12->surface20->pixels;
    surface12->pitch = surface12->surface20->pitch;
}

DECLSPEC12 int SDLCALL
SDL_SetColorKey(SDL12_Surface *surface12, Uint32 flag12, Uint32 key)
{
    const SDL_bool addkey = (flag12 & SDL12_SRCCOLORKEY) ? SDL_TRUE : SDL_FALSE;
    const int retval = SDL20_SetColorKey(surface12->surface20, addkey, key);
    if (SDL20_GetColorKey(surface12->surface20, &surface12->format->colorkey) < 0) {
        surface12->format->colorkey = 0;
    }

    if (addkey) {
        surface12->flags |= SDL12_SRCCOLORKEY;
    } else {
        surface12->flags &= ~SDL12_SRCCOLORKEY;
    }

    return retval;
}

DECLSPEC12 SDL12_Surface * SDLCALL
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
        } else {
            if (flags12 & SDL12_SRCALPHA) {
                SDL20_SetSurfaceBlendMode(surface20, SDL_BLENDMODE_BLEND);
                retval->flags |= SDL12_SRCALPHA;
            }
            if (flags12 & SDL12_SRCCOLORKEY) {
                Uint8 r, g, b, a;
                SDL20_GetRGBA(src12->format->colorkey, src12->surface20->format, &r, &g, &b, &a);
                SDL_SetColorKey(retval, SDL12_SRCCOLORKEY, SDL20_MapRGBA(retval->surface20->format, r, g, b, a));
            }
        }
    }
    return retval;
}

DECLSPEC12 SDL12_Surface * SDLCALL
SDL_DisplayFormat(SDL12_Surface *surface12)
{
    const Uint32 flags = surface12->flags & (SDL12_SRCCOLORKEY|SDL12_SRCALPHA|SDL12_RLEACCELOK);
    if (!VideoSurface12) {
        SDL20_SetError("No video mode has been set");
        return NULL;
    }

    return SDL_ConvertSurface(surface12, VideoSurface12->format, flags);
}

DECLSPEC12 SDL12_Surface * SDLCALL
SDL_DisplayFormatAlpha(SDL12_Surface *surface12)
{
    const Uint32 flags = surface12->flags & (SDL12_SRCALPHA|SDL12_RLEACCELOK);
    SDL12_Surface *retval = NULL;
    SDL_PixelFormat *fmt20 = NULL;
    SDL12_PixelFormat fmt12;

    if (!VideoSurface12) {
        SDL20_SetError("No video mode has been set");
        return NULL;
    }

    /* we only allow a few formats for the screen surface, and this is the appropriate alpha format for all of them. */
    fmt20 = SDL20_AllocFormat(SDL_PIXELFORMAT_ARGB8888);
    if (!fmt20) {
        return NULL;
    }

    retval = SDL_ConvertSurface(surface12, PixelFormat20to12(&fmt12, NULL, fmt20), flags);
    SDL20_FreeFormat(fmt20);
    return retval;
}

static void
PresentScreen(void)
{
    QueuedOverlayItem *overlay;
    SDL_Renderer *renderer = LockVideoRenderer();

    if (!renderer) {
        return;
    }

    SDL20_RenderClear(renderer);
    SDL20_RenderCopy(renderer, VideoTexture20, NULL, NULL);

    /* Render any pending YUV overlay over the surface texture. */
    overlay = QueuedDisplayOverlays.next;
    if (overlay) {
        while (overlay != NULL) {
            QueuedOverlayItem *next = overlay->next;
            if (overlay->overlay12) {
                SDL12_YUVData *hwdata = (SDL12_YUVData *) overlay->overlay12->hwdata;
                SDL_Rect dstrect20;
                SDL20_RenderCopy(renderer, hwdata->texture20, NULL, Rect12to20(&overlay->dstrect12, &dstrect20));
            }
            SDL_free(overlay);
            overlay = next;
        }
        QueuedDisplayOverlays.next = NULL;
        QueuedDisplayOverlaysTail = &QueuedDisplayOverlays;
    }

    SDL20_RenderPresent(renderer);
    VideoSurfaceUpdatedInBackgroundThread = SDL_FALSE;
    VideoSurfaceLastPresentTicks = SDL20_GetTicks();
    VideoSurfacePresentTicks = 0;

    UnlockVideoRenderer();
}

static void
UpdateRect12to20(SDL12_Surface *surface12, const SDL12_Rect *rect12, SDL_Rect *rect20, SDL_bool *whole_screen)
{
    Rect12to20(rect12, rect20);
    if (!rect20->x && !rect20->y && !rect20->w && !rect20->h) {
        *whole_screen = SDL_TRUE;
        rect20->w = surface12->w;
        rect20->h = surface12->h;
    } else {
        SDL_Rect surfacerect20;
        surfacerect20.x = surfacerect20.y = 0;
        surfacerect20.w = surface12->w;
        surfacerect20.h = surface12->h;
        Rect12to20(rect12, rect20);
        SDL20_IntersectRect(&surfacerect20, rect20, rect20);
        if ((rect20->x == 0) && (rect20->y == 0) && (rect20->w == surface12->w) && (rect20->h == surface12->h)) {
            *whole_screen = SDL_TRUE;
        }
    }
}

/* For manual throttling of screen updates. */
static int
GetDesiredMillisecondsPerFrame()
{
    SDL_DisplayMode mode;
    if (VideoSurface12->flags & SDL12_FULLSCREEN) {
       SDL_assert(VideoWindow20 != NULL);
       if (SDL20_GetWindowDisplayMode(VideoWindow20, &mode) == 0) {
           if (mode.refresh_rate) {
               return 1000 / mode.refresh_rate;
           }
       }
    } else if (SDL20_GetCurrentDisplayMode(VideoDisplayIndex, &mode) == 0) {
        /* If we're windowed, assume we're on the default screen. */
        if (mode.refresh_rate) {
            return 1000 / mode.refresh_rate;
        }
    }
    return 15;
}

/* SDL_OPENGLBLIT support APIs. https://discourse.libsdl.org/t/ogl-and-sdl/2775/3 */
DECLSPEC12 void SDLCALL
SDL_GL_Lock(void)
{
    if (!OpenGLBlitTexture) {
        return;
    }

    if (++OpenGLBlitLockCount == 1) {
        OpenGLFuncs.glPushAttrib(GL_ALL_ATTRIB_BITS);
        OpenGLFuncs.glPushClientAttrib(GL_CLIENT_PIXEL_STORE_BIT);
        OpenGLFuncs.glEnable(GL_TEXTURE_2D);
        OpenGLFuncs.glEnable(GL_BLEND);
        OpenGLFuncs.glDisable(GL_FOG);
        OpenGLFuncs.glDisable(GL_ALPHA_TEST);
        OpenGLFuncs.glDisable(GL_DEPTH_TEST);
        OpenGLFuncs.glDisable(GL_SCISSOR_TEST);
        OpenGLFuncs.glDisable(GL_STENCIL_TEST);
        OpenGLFuncs.glDisable(GL_CULL_FACE);

        OpenGLFuncs.glBindTexture(GL_TEXTURE_2D, OpenGLBlitTexture);
        OpenGLFuncs.glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        OpenGLFuncs.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        OpenGLFuncs.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        OpenGLFuncs.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        OpenGLFuncs.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        OpenGLFuncs.glPixelStorei(GL_UNPACK_ROW_LENGTH, VideoSurface12->pitch / VideoSurface12->format->BytesPerPixel);
        OpenGLFuncs.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        OpenGLFuncs.glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        OpenGLFuncs.glViewport(0, 0, VideoSurface12->w, VideoSurface12->h);
        OpenGLFuncs.glMatrixMode(GL_PROJECTION);
        OpenGLFuncs.glPushMatrix();
        OpenGLFuncs.glLoadIdentity();

        OpenGLFuncs.glOrtho(0.0, (GLdouble) VideoSurface12->w, (GLdouble) VideoSurface12->h, 0.0, 0.0, 1.0);

        OpenGLFuncs.glMatrixMode(GL_MODELVIEW);
        OpenGLFuncs.glPushMatrix();
        OpenGLFuncs.glLoadIdentity();
    }
}

DECLSPEC12 void SDLCALL
SDL_GL_UpdateRects(int numrects, SDL12_Rect *rects12)
{
    if (OpenGLBlitTexture) {
        const int srcpitch = VideoSurface12->pitch;
        const int pixsize = VideoSurface12->format->BytesPerPixel;
        const GLenum glfmt = (pixsize == 4) ? GL_RGBA : GL_RGB;
        const GLenum gltype = (pixsize == 4) ? GL_UNSIGNED_BYTE : GL_UNSIGNED_SHORT_5_6_5;
        SDL_Rect surfacerect20;
        int i;

        surfacerect20.x = surfacerect20.y = 0;
        surfacerect20.w = VideoSurface12->w;
        surfacerect20.h = VideoSurface12->h;

        for (i = 0; i < numrects; i++) {
            SDL_Rect rect20;
            SDL_Rect intersected20;
            Uint8 *src;

            SDL20_IntersectRect(Rect12to20(&rects12[i], &rect20), &surfacerect20, &intersected20);

            src = (((Uint8 *) VideoSurface12->pixels) + (intersected20.y * srcpitch)) + (intersected20.x * pixsize);
            OpenGLFuncs.glTexSubImage2D(GL_TEXTURE_2D, 0, intersected20.x, intersected20.y, intersected20.w, intersected20.h, glfmt, gltype, src);

            OpenGLFuncs.glBegin(GL_TRIANGLE_STRIP);
            {
                const GLfloat tex_x1 = ((GLfloat) intersected20.x) / ((GLfloat) VideoSurface12->w);
                const GLfloat tex_y1 = ((GLfloat) intersected20.y) / ((GLfloat) VideoSurface12->h);
                const GLfloat tex_x2 = tex_x1 + ((GLfloat) intersected20.w) / ((GLfloat) VideoSurface12->w);
                const GLfloat tex_y2 = tex_y1 + ((GLfloat) intersected20.h) / ((GLfloat) VideoSurface12->h);
                const GLint vert_x1 = (GLint) intersected20.x;
                const GLint vert_y1 = (GLint) intersected20.y;
                const GLint vert_x2 = vert_x1 + (GLint) intersected20.w;
                const GLint vert_y2 = vert_y1 + (GLint) intersected20.h;
                OpenGLFuncs.glTexCoord2f(tex_x1, tex_y1);
                OpenGLFuncs.glVertex2i(vert_x1, vert_y1);
                OpenGLFuncs.glTexCoord2f(tex_x2, tex_y1);
                OpenGLFuncs.glVertex2i(vert_x2, vert_y1);
                OpenGLFuncs.glTexCoord2f(tex_x1, tex_y2);
                OpenGLFuncs.glVertex2i(vert_x1, vert_y2);
                OpenGLFuncs.glTexCoord2f(tex_x2, tex_y2);
                OpenGLFuncs.glVertex2i(vert_x2, vert_y2);
            }
            OpenGLFuncs.glEnd();
        }
    }
}


DECLSPEC12 void SDLCALL
SDL_GL_Unlock(void)
{
    if (OpenGLBlitTexture) {
        if (OpenGLBlitLockCount > 0) {
            if (--OpenGLBlitLockCount == 0) {
                OpenGLFuncs.glPopMatrix();
                OpenGLFuncs.glMatrixMode(GL_PROJECTION);
                OpenGLFuncs.glPopMatrix();
                OpenGLFuncs.glPopClientAttrib();
                OpenGLFuncs.glPopAttrib();
            }
        }
    }
}


DECLSPEC12 void SDLCALL
SDL_UpdateRects(SDL12_Surface *surface12, int numrects, SDL12_Rect *rects12)
{
    const SDL_bool ThisIsSetVideoModeThread = (SDL20_ThreadID() == SetVideoModeThread) ? SDL_TRUE : SDL_FALSE;

    /* strangely, SDL 1.2 doesn't check if surface12 is NULL before touching it */
    /* (UpdateRect, singular, does...) */

    if ((surface12 == VideoSurface12) && ((surface12->flags & SDL12_OPENGLBLIT) == SDL12_OPENGLBLIT)) {
        SDL_GL_Lock();
        SDL_GL_UpdateRects(numrects, rects12);
        SDL_GL_Unlock();
        return;
    }

    if (surface12->flags & SDL12_OPENGL) {
        SDL20_SetError("Use SDL_GL_SwapBuffers() on OpenGL surfaces");
        return;
    }

    /* everything else is marked SDL12_DOUBLEBUF and SHOULD BE a no-op here,
     * but in practice most apps never got a double-buffered surface and
     * don't handle it correctly, so we have to work around it. */
    if (surface12 == VideoSurface12) {
        const SDL_bool upload_later = (!ThisIsSetVideoModeThread && !AllowThreadedDraws) ? SDL_TRUE : SDL_FALSE;
        SDL_Palette *logicalPal = surface12->surface20->format->palette;
        const int pixsize = surface12->format->BytesPerPixel;
        const int srcpitch = surface12->pitch;
        SDL_bool whole_screen = SDL_FALSE;
        SDL_Renderer *renderer = NULL;
        void *pixels = NULL;
        SDL_Rect rect20;
        int pitch = 0;
        int i, j;

        if (!upload_later) {
            renderer = LockVideoRenderer();  /* must own the renderer before locking the texture! */
        }

        for (i = 0; i < numrects; i++) {
            UpdateRect12to20(surface12, &rects12[i], &rect20, &whole_screen);

            if (!renderer) {
                continue;
            } else if (!rect20.w || !rect20.h) {
                continue;
            } else if (SDL20_LockTexture(VideoTexture20, &rect20, &pixels, &pitch) < 0) {
                continue;  /* oh well */
            }

            if (VideoConvertSurface20) {
                SDL_Rect dstrect20;  /* pretend that the subregion is just the top left of the convert surface. */
                dstrect20.x = dstrect20.y = 0;
                dstrect20.w = rect20.w;
                dstrect20.h = rect20.h;
                surface12->surface20->format->palette = VideoPhysicalPalette20;
                VideoConvertSurface20->pixels = pixels;
                VideoConvertSurface20->pitch = pitch;
                VideoConvertSurface20->w = rect20.w;
                VideoConvertSurface20->h = rect20.h;
                SDL20_UpperBlit(VideoSurface12->surface20, &rect20, VideoConvertSurface20, &dstrect20);
            } else {
                const int cpy = rect20.w * pixsize;
                char *dst = (char *) pixels;
                const Uint8 *src = (((Uint8 *) surface12->pixels) + (rect20.y * srcpitch)) + (rect20.x * pixsize);
                for (j = 0; j < rect20.h; j++) {
                    SDL20_memcpy(dst, src, cpy);
                    src += srcpitch;
                    dst += pitch;
                }
            }

            SDL20_UnlockTexture(VideoTexture20);
        }

        if (VideoConvertSurface20) {  /* reset some state we messed with */
            surface12->surface20->format->palette = logicalPal;
            VideoConvertSurface20->pixels = NULL;
            VideoConvertSurface20->pitch = 0;
            VideoConvertSurface20->w = VideoSurface12->w;
            VideoConvertSurface20->h = VideoSurface12->h;
        }

        if (upload_later) {
            VideoSurfaceUpdatedInBackgroundThread = SDL_TRUE;
            VideoSurfacePresentTicks = whole_screen ? 1 : VideoSurfaceLastPresentTicks + GetDesiredMillisecondsPerFrame();  /* flip it later (or as soon as the main thread can). */
        } else if (whole_screen) {
            PresentScreen();  /* flip it now. */
        } else {
            if (!VideoSurfacePresentTicks) {
                VideoSurfacePresentTicks = VideoSurfaceLastPresentTicks + GetDesiredMillisecondsPerFrame();  /* flip it later. */
            } else if (SDL_TICKS_PASSED(SDL20_GetTicks(), VideoSurfacePresentTicks)) {
                PresentScreen();
            }
        }

        if (renderer) {
            UnlockVideoRenderer();
        }
    }
}

DECLSPEC12 void SDLCALL
SDL_UpdateRect(SDL12_Surface *screen12, Sint32 x, Sint32 y, Uint32 w, Uint32 h)
{
    if (screen12) {
        SDL12_Rect rect12;
        rect12.x = (Sint16) x;
        rect12.y = (Sint16) y;
        rect12.w = (Uint16) (w ? w : screen12->w);
        rect12.h = (Uint16) (h ? h : screen12->h);
        SDL_UpdateRects(screen12, 1, &rect12);
    }
}

DECLSPEC12 int SDLCALL
SDL_Flip(SDL12_Surface *surface12)
{
    if (surface12 == VideoSurface12) {
        SDL_UpdateRect(surface12, 0, 0, 0, 0);  /* update the whole screen and present now. */
    }

    return 0;
}

static void
HandleKeyRepeat(void)
{
    /* SDL 1.2 is a little weird about key repeat; it manages it itself
       (so we can't use SDL2's key repeat), it only runs during PumpEvents
       (so no AddTimer or separate thread to manage it), and it cares about
       the last key pressed (so if a key is repeating and you press a new
       one down, it stops repeating the first key). */
    if (KeyRepeatNextTicks) {
        const Uint32 now = SDL20_GetTicks();
        SDL_assert(KeyRepeatEvent.type == SDL12_KEYDOWN);
        if (SDL_TICKS_PASSED(now, KeyRepeatNextTicks)) {
            /* these repeat from the current time in SDL 1.2, not consistently! */
            KeyRepeatNextTicks = now + KeyRepeatInterval;
            PushEventIfNotFiltered(&KeyRepeatEvent);
        }
    }
}

DECLSPEC12 void SDLCALL
SDL_PumpEvents(void)
{
    const SDL_bool ThisIsSetVideoModeThread = (SDL20_ThreadID() == SetVideoModeThread) ? SDL_TRUE : SDL_FALSE;
    SDL_Event e;

    if (!ThisIsSetVideoModeThread && !AllowThreadedPumps) {
        return;
    }

    /* If the app is doing dirty rectangles, we set a flag and present the
     * screen surface when they pump for new events if we're close to 60Hz,
     * which we consider a sign that they are done rendering for the current
     * frame and it would make sense to send it to the screen. */

    if (VideoSurfacePresentTicks && SDL_TICKS_PASSED(SDL20_GetTicks(), VideoSurfacePresentTicks)) {
        if (VideoSurfaceUpdatedInBackgroundThread) {
            SDL_Flip(VideoSurface12);  /* this will update the texture and present. */
        } else {
            PresentScreen();
        }
    }

    if (EventQueueMutex) {
        SDL20_LockMutex(EventQueueMutex);
    }

    while (SDL20_PollEvent(&e)) {  /* spin to drain the SDL2 event queue. */ }

    /* If there's a pending KEYDOWN event, and we haven't got a TEXTINPUT
     * event which matches it, then let it through now. */
    if (PendingKeydownEvent.type == SDL12_KEYDOWN) {
        FlushPendingKeydownEvent(0);
    }

    HandleKeyRepeat();  /* deal with SDL 1.2-style key repeat... */

    if (EventQueueMutex) {
        SDL20_UnlockMutex(EventQueueMutex);
    }
}

DECLSPEC12 void SDLCALL
SDL_WM_SetCaption(const char *title, const char *icon)
{
    if (WindowTitle) {
        SDL20_free(WindowTitle);
    }
    if (WindowIconTitle) {
        SDL20_free(WindowIconTitle);
    }
    WindowTitle = title ? SDL20_strdup(title) : NULL;
    WindowIconTitle = icon ? SDL20_strdup(icon) : NULL;
    if (VideoWindow20) {
        SDL20_SetWindowTitle(VideoWindow20, WindowTitle);
    }
}

DECLSPEC12 void SDLCALL
SDL_WM_GetCaption(const char **title, const char **icon)
{
    if (title) {
        *title = WindowTitle;
    }
    if (icon) {
        *icon = WindowIconTitle;
    }
}

DECLSPEC12 void SDLCALL
SDL_WM_SetIcon(SDL12_Surface *icon12, Uint8 *mask)
{
    SDL_BlendMode oldmode;
    Uint32 rmask, gmask, bmask, amask;
    SDL_Surface *icon20;
    int bpp;
    int ret;

    /* Make sure we actually have an icon to set */
    if (!icon12) {
        return;
    }

    /* take the mask and zero out those alpha values. */
    oldmode = SDL_BLENDMODE_NONE;
    if (SDL20_GetSurfaceBlendMode(icon12->surface20, &oldmode) < 0) {
        return; /* oh well. */
    }
    if (!SDL20_PixelFormatEnumToMasks(SDL_PIXELFORMAT_ARGB8888, &bpp, &rmask, &gmask, &bmask, &amask)) {
        return; /* oh well. */
    }
    icon20 = SDL20_CreateRGBSurface(0, icon12->w, icon12->h, bpp, rmask, gmask, bmask, amask);
    if (!icon20) {
        return; /* oh well. */
    }

    SDL20_SetSurfaceBlendMode(icon12->surface20, SDL_BLENDMODE_NONE);
    ret = SDL20_UpperBlit(icon12->surface20, NULL, icon20, NULL);
    SDL20_SetSurfaceBlendMode(icon12->surface20, oldmode);
    if (ret == 0) {
        if (mask) {
            const int w = icon12->w;
            const int h = icon12->h;
            const int mpitch = (w + 7) / 8;
            Uint32 *ptr = (Uint32 *) icon20->pixels;
            int x, y;

            SDL_assert(icon20->format->BytesPerPixel == 4);
            SDL_assert(icon20->pitch == icon20->w * 4);

            for (y = 0; y < h; y++) {
                for (x = 0; x < w; x++, ptr++) {
                    if (!(mask[y*mpitch + x/8] & (128 >> (x % 8)))) {
                        *ptr &= ~amask;
                    } else {
                        *ptr |= amask;
                    }
                }
            }
        }

        if (VideoWindow20) {
            SDL20_SetWindowIcon(VideoWindow20, icon20);
        }
        SDL20_FreeSurface(VideoIcon20);
        VideoIcon20 = icon20;
    }

    if (VideoWindow20) {
        SDL20_SetWindowIcon(VideoWindow20, VideoIcon20);
    }
}

DECLSPEC12 int SDLCALL
SDL_WM_IconifyWindow(void)
{
    if (VideoWindow20) {
        SDL20_MinimizeWindow(VideoWindow20);
    }
    return 0;
}

DECLSPEC12 int SDLCALL
SDL_WM_ToggleFullScreen(SDL12_Surface *surface)
{
    int retval = 0;
    if (surface == VideoSurface12) {
        Uint32 flags20;
        SDL_assert(VideoWindow20);
        flags20 = SDL20_GetWindowFlags(VideoWindow20);
        if (flags20 & SDL_WINDOW_FULLSCREEN) {
            SDL_assert(VideoSurface12->flags & SDL12_FULLSCREEN);
            retval = (SDL20_SetWindowFullscreen(VideoWindow20, 0) == 0);
            if (retval) {
                VideoSurface12->flags &= ~SDL12_FULLSCREEN;
            }
        } else {
            Uint32 newflags20;
            SDL_assert((VideoSurface12->flags & SDL12_FULLSCREEN) == 0);
            newflags20 = (((VideoSurface12->flags & SDL12_OPENGL) == 0) || (OpenGLLogicalScalingFBO != 0)) ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_FULLSCREEN;
            retval = (SDL20_SetWindowFullscreen(VideoWindow20, newflags20) == 0);
            if (retval) {
                VideoSurface12->flags |= SDL12_FULLSCREEN;
            }
        }
        if (retval) {
            SDL_Renderer *renderer = LockVideoRenderer();
            if (renderer) {
                SDL20_RenderSetLogicalSize(renderer, VideoSurface12->w, VideoSurface12->h);
                UnlockVideoRenderer();
            }
        }
    }
    return retval;
}

static void
UpdateRelativeMouseMode(void)
{
    /* in SDL 1.2, hiding+grabbing the cursor was like SDL2's relative mouse mode. */
    if (VideoWindow20) {
        const SDL_bool enable = (VideoWindowGrabbed && VideoCursorHidden) ? SDL_TRUE : SDL_FALSE;
        if (MouseInputIsRelative != enable) {
            MouseInputIsRelative = enable;
            if (MouseInputIsRelative) {
                /* reset position, we'll have to track it ourselves in SDL_MOUSEMOTION events, since 1.2
                 * would give you window coordinates, even in relative mode. */
                SDL20_GetMouseState(&MousePosition.x, &MousePosition.y);
                AdjustOpenGLLogicalScalingPoint(&MousePosition.x, &MousePosition.y);
            }
            SDL20_SetRelativeMouseMode(MouseInputIsRelative);
        }
    }
}

DECLSPEC12 int SDLCALL
SDL_ShowCursor(int toggle)
{
    const int retval = VideoCursorHidden ? 0 : 1;

    if (toggle >= 0) {
        const SDL_bool wanthide = (toggle == 0) ? SDL_TRUE : SDL_FALSE;
        if (VideoCursorHidden != wanthide) {
            SDL20_ShowCursor(wanthide ? 0 : 1);
            VideoCursorHidden = wanthide;
            UpdateRelativeMouseMode();
        }
    }
    return retval;
}

static void
HandleInputGrab(SDL12_GrabMode mode)
{
    /* SDL 1.2 always grabbed input if the video mode was fullscreen. */
    const SDL_bool isfullscreen = (VideoSurface12 && (VideoSurface12->flags & SDL12_FULLSCREEN)) ? SDL_TRUE : SDL_FALSE;
    const SDL_bool wantgrab = (isfullscreen || (mode == SDL12_GRAB_ON)) ? SDL_TRUE : SDL_FALSE;
    if (VideoWindowGrabbed != wantgrab) {
        if (VideoWindow20) {
            SDL20_SetWindowGrab(VideoWindow20, wantgrab);
        }
        VideoWindowGrabbed = wantgrab;
        UpdateRelativeMouseMode();
    }
}

DECLSPEC12 SDL12_GrabMode SDLCALL
SDL_WM_GrabInput(SDL12_GrabMode mode)
{
    if (mode != SDL12_GRAB_QUERY) {
        HandleInputGrab(mode);
    }
    return VideoWindowGrabbed ? SDL12_GRAB_ON : SDL12_GRAB_OFF;
}

DECLSPEC12 void SDLCALL
SDL_WarpMouse(Uint16 x, Uint16 y)
{
    if (MouseInputIsRelative) {  /* we have to track this ourselves, in case app calls SDL_GetMouseState(). */
        MousePosition.x = (int) x;
        MousePosition.y = (int) y;
    } else {
        if (VideoWindow20) {
            SDL20_WarpMouseInWindow(VideoWindow20, x, y);
        }
    }
}

DECLSPEC12 Uint8 SDLCALL
SDL_GetAppState(void)
{
    Uint8 state12 = 0;
    if (VideoWindow20) {
        const Uint32 flags20 = SDL20_GetWindowFlags(VideoWindow20);
        if ((flags20 & SDL_WINDOW_SHOWN) && !(flags20 & SDL_WINDOW_MINIMIZED)) {
            state12 |= SDL12_APPACTIVE;
        }
        if (flags20 & SDL_WINDOW_INPUT_FOCUS) {
            state12 |= SDL12_APPINPUTFOCUS;
        }
        if (flags20 & SDL_WINDOW_MOUSE_FOCUS) {
            state12 |= SDL12_APPMOUSEFOCUS;
        }
    }
    return state12;
}

DECLSPEC12 int SDLCALL
SDL_SetPalette(SDL12_Surface *surface12, int flags, const SDL_Color *colors,
               int firstcolor, int ncolors)
{
    SDL12_Palette *palette12;
    SDL_Palette *palette20;
    SDL_Color *opaquecolors;
    int i, retval;

    if (!surface12) {
        return 0;  /* not an error, a no-op. */
    }

    if ((flags & (SDL12_LOGPAL | SDL12_PHYSPAL)) == 0) {
        return 0;  /* nothing to do. */
    }

    palette12 = surface12->format->palette;
    if (!palette12) {
        return 0;  /* not an error, a no-op. */
    }

    palette20 = surface12->surface20->format->palette;
    SDL_assert(palette20 != NULL);

    /* we need to force the "unused" field to 255, since it's "alpha" in SDL2. */
    opaquecolors = (SDL_Color *) SDL20_malloc(sizeof (SDL_Color) * ncolors);
    if (!opaquecolors) {
        return SDL20_OutOfMemory();
    }

    /* don't SDL_memcpy in case the 'a' field is uninitialized and upsets
     * memory tools like Valgrind. */
    for (i = 0; i < ncolors; i++) {
        opaquecolors[i].r = colors[i].r;
        opaquecolors[i].g = colors[i].g;
        opaquecolors[i].b = colors[i].b;
        opaquecolors[i].a = 255;
    }

    retval = 0;

    if (flags & SDL12_LOGPAL) {
        if (SDL20_SetPaletteColors(palette20, opaquecolors, firstcolor, ncolors) < 0) {
            retval = -1;
        }
    }

    if ((flags & SDL12_PHYSPAL) && (surface12 == VideoSurface12) && VideoPhysicalPalette20) {
        if (SDL20_SetPaletteColors(VideoPhysicalPalette20, opaquecolors, firstcolor, ncolors) < 0) {
            retval = -1;
        }
    }

    SDL20_free(opaquecolors);

    /* in case this pointer changed... */
    palette12->colors = palette20->colors;

    if ((surface12 == VideoSurface12) && (flags & SDL12_PHYSPAL)) {
        SDL_UpdateRect(surface12, 0, 0, 0, 0);   /* force screen to reblit with new palette. */
    }

    return retval;
}

DECLSPEC12 int SDLCALL
SDL_SetColors(SDL12_Surface *surface12, const SDL_Color * colors, int firstcolor,
              int ncolors)
{
    return SDL_SetPalette(surface12, SDL12_LOGPAL | SDL12_PHYSPAL, colors, firstcolor, ncolors);
}


#if defined(SDL_VIDEO_DRIVER_X11)
/* In 1.2, these would lock the event thread (if you _used_ the event thread), and call XSync(SDL_Display, False) before unlocking */
static void x11_lock_display(void) {}
static void x11_unlock_display(void) {}
#endif

DECLSPEC12 int SDLCALL
SDL_GetWMInfo(SDL12_SysWMinfo *info12)
{
    SDL_SysWMinfo info20;
    SDL_bool temp_window = SDL_FALSE;
    SDL_Window *win20 = VideoWindow20;
    int rc;

    if (info12->version.major > 1) {
        SDL20_SetError("Requested version is unsupported");
        return 0;  /* some programs only test against 0, not -1 */
    } else if (!SupportSysWM) {
        SDL20_SetError("No SysWM support available");
        return 0;  /* some programs only test against 0, not -1 */
    }

    if (win20 == NULL) {
        /* It was legal to call SDL_GetWMInfo without SDL_SetVideoMode() on X11 and Windows (and others...?) in 1.2. */
        win20 = SDL20_CreateWindow("SDL_GetWMInfo support window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 128, 128, SDL_WINDOW_HIDDEN);
        if (!win20) {
            return 0;
        }
        temp_window = SDL_TRUE;
    }

    SDL_zero(info20);

    /* SDL2, before the version scheme change, would fail if the requested version wasn't
       2.0.x, so if the SDL2 is from before this was fixed, we need to lie about the
       version, and assume it will work out. */
    if (LinkedSDL2VersionInt >= SDL_VERSIONNUM(2,24,0)) {
        SDL_VERSION(&info20.version);
    } else {
        info20.version.major = 2;
        info20.version.minor = 0;
        info20.version.patch = 22;
    }

    rc = SDL20_GetWindowWMInfo(win20, &info20);

    if (temp_window) {
        SDL20_DestroyWindow(win20);
    }

    if (!rc) {
        return 0;  /* some programs only test against 0, not -1 */
    }

#if defined(_WIN32)
    if (info20.subsystem == SDL_SYSWM_WINDOWS) {
        info12->window = temp_window ? 0 : info20.info.win.window;
        if (SDL_VERSIONNUM(info12->version.major, info12->version.minor, info12->version.patch) >= SDL_VERSIONNUM(1, 2, 5)) {
            info12->hglrc = (HGLRC) VideoGLContext20;
        }
    } else {
        SDL20_SetError("No SysWM information available");
        return 0;  /* some programs only test against 0, not -1 */
    }
#elif defined(SDL_VIDEO_DRIVER_X11)
    if (info20.subsystem == SDL_SYSWM_X11) {
        info12->subsystem = SDL12_SYSWM_X11;
        info12->info.x11.display = info20.info.x11.display;
        info12->info.x11.window = temp_window ? 0 : info20.info.x11.window;
        if (SDL_VERSIONNUM(info12->version.major, info12->version.minor, info12->version.patch) >= SDL_VERSIONNUM(1, 0, 2)) {
            info12->info.x11.fswindow = 0;  /* these don't exist in SDL2. */
            info12->info.x11.wmwindow = 0;
        }
        if (SDL_VERSIONNUM(info12->version.major, info12->version.minor, info12->version.patch) >= SDL_VERSIONNUM(1, 2, 12)) {
            info12->info.x11.gfxdisplay = info20.info.x11.display;  /* shrug */
        }
        info12->info.x11.lock_func = x11_lock_display;  /* just no-ops for now */
        info12->info.x11.unlock_func = x11_unlock_display;
    } else {
        SDL20_SetError("No SysWM information available");
        return 0;  /* some programs only test against 0, not -1 */
    }
#else
    info12->data = 0;  /* shrug */
#endif

    return 1;
}

DECLSPEC12 SDL_Window * SDLCALL
SDL12COMPAT_GetWindow(void)
{
    return VideoWindow20;
}

DECLSPEC12 SDL12_Overlay * SDLCALL
SDL_CreateYUVOverlay(int w, int h, Uint32 format12, SDL12_Surface *display12)
{
    /* SDL 1.2 has you pass the screen surface in here, but it doesn't check that it's _actually_ the screen surface,
       which implies you should be able to blit YUV overlays to other surfaces too...but then SDL_DisplayYUVOverlay
       always uses the screen surface unconditionally. As such, and because overlays were sort of hostile to software
       rendering anyhow, we always make an SDL_Texture in here and draw over the screen pixels, in hopes that the GPU
       gives us a boost here. */

    const char *old_scale_quality = SDL20_GetHint(SDL_HINT_RENDER_SCALE_QUALITY);
    SDL12_Overlay *retval = NULL;
    SDL12_YUVData *hwdata = NULL;
    SDL_Renderer *renderer = NULL;
    Uint32 format20 = 0;

    if (display12 != VideoSurface12) {  /* SDL 1.2 doesn't check this, but it seems irresponsible not to. */
        SDL20_SetError("YUV overlays are only supported on the screen surface");
        return NULL;
    }

    if (display12->flags & SDL12_OPENGL) {
        SDL20_SetError("YUV overlays are not supported in OpenGL mode");
        return NULL;
    }

    switch (format12) {
        #define SUPPORTED_YUV_FORMAT(x) case SDL12_##x##_OVERLAY: format20 = SDL_PIXELFORMAT_##x; break
        SUPPORTED_YUV_FORMAT(YV12);
        SUPPORTED_YUV_FORMAT(IYUV);
        SUPPORTED_YUV_FORMAT(YUY2);
        SUPPORTED_YUV_FORMAT(UYVY);
        SUPPORTED_YUV_FORMAT(YVYU);
        #undef SUPPORTED_YUV_FORMAT
        default: SDL20_SetError("Unsupported YUV format"); return NULL;
    }

    retval = (SDL12_Overlay *) SDL20_calloc(1, sizeof (SDL12_Overlay) + sizeof (SDL12_YUVData));
    if (!retval) {
        SDL20_OutOfMemory();
        return NULL;
    }

    hwdata = (SDL12_YUVData *) (retval + 1);
    hwdata->pixelbuf = (Uint8 *) SDL20_calloc(1, (w * 2) * h);
    if (!hwdata->pixelbuf) {
        SDL20_free(retval);
        SDL20_OutOfMemory();
        return NULL;
    }

    hwdata->pixels[0] = hwdata->pixelbuf;
    if ((format12 == SDL12_YV12_OVERLAY) || (format12 == SDL12_IYUV_OVERLAY)) {
        retval->planes = 3;
        hwdata->pitches[0] = w;
        hwdata->pitches[1] = hwdata->pitches[2] = w / 2;
        hwdata->pixels[1] = hwdata->pixels[0] + (w * h);
        hwdata->pixels[2] = hwdata->pixels[1] + ((w / 2) * h);
    } else {
        retval->planes = 1;
        hwdata->pitches[0] = w * 2;
    }

    renderer = LockVideoRenderer();

    SDL20_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    hwdata->texture20 = SDL20_CreateTexture(renderer, format20, SDL_TEXTUREACCESS_STREAMING, w, h);
    SDL20_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, old_scale_quality);

    UnlockVideoRenderer();

    if (!hwdata->texture20) {
        SDL20_free(hwdata->pixelbuf);
        SDL20_free(retval);
        return NULL;
    }

    retval->format = format12;
    retval->w = w;
    retval->h = h;
    retval->hwfuncs = (void *) 0x1;  /* in case it's important for this to be non-NULL. */
    retval->hwdata = hwdata;
    retval->hw_overlay = 1;
    retval->pitches = hwdata->pitches;

    /* Some programs (e.g. mplayer) access pixels without locking. */
    retval->pixels = hwdata->pixels;
    hwdata->dirty = SDL_TRUE;

    return retval;
}

DECLSPEC12 int SDLCALL
SDL_LockYUVOverlay(SDL12_Overlay *overlay12)
{
    SDL12_YUVData *hwdata;

    if (!overlay12) {
        return SDL20_InvalidParamError("overlay");
    }

    /* SMPEG locks the YUV overlay, calls SDL_DisplayYUVOverlay() on it,
       copies data to it then unlocks it, in that order, which _seems_ like an
       app bug, but it works in 1.2. Most of SDL 1.2's yuv lock/unlock
       implementations are just no-ops anyhow, so we upload during display,
       if the overlay has been locked since last time (and even
       if it is still locked), to accomodate this usage pattern. */

    hwdata = (SDL12_YUVData *) overlay12->hwdata;
    hwdata->dirty = SDL_TRUE;  /* assume the contents will change and we must upload */
    overlay12->pixels = hwdata->pixels;

    return 0;  /* success */
}

DECLSPEC12 int SDLCALL
SDL_DisplayYUVOverlay(SDL12_Overlay *overlay12, SDL12_Rect *dstrect12)
{
    QueuedOverlayItem *overlay;
    SDL12_YUVData *hwdata;
    SDL_Renderer *renderer = NULL;

    if (!overlay12) {
        return SDL20_InvalidParamError("overlay");
    } else if (!dstrect12) {
        return SDL20_InvalidParamError("dstrect");
    } else if (!(renderer = LockVideoRenderer())) {
        return SDL20_SetError("No software screen surface available");
    }

    for (overlay = QueuedDisplayOverlays.next; overlay != NULL; overlay = overlay->next) {
        if (overlay->overlay12 == overlay12) {   /* trying to draw the same overlay twice in one frame? Dump the current surface and overlays to the screen now. */
            SDL_Flip(VideoSurface12);
            break;
        }
    }

    if ((overlay = (QueuedOverlayItem *) SDL_malloc(sizeof (QueuedOverlayItem))) == NULL) {
        UnlockVideoRenderer();
        return SDL20_OutOfMemory();
    }

    hwdata = (SDL12_YUVData *) overlay12->hwdata;

    /* Upload contents if we've been locked, even if we're _still_ locked, to
       work around an SMPEG quirk. */
    if (hwdata->dirty) {
        SDL_Rect rect20;
        rect20.x = rect20.y = 0;
        rect20.w = overlay12->w;
        rect20.h = overlay12->h;
        if (overlay12->format == SDL12_IYUV_OVERLAY) {
            SDL20_UpdateYUVTexture(hwdata->texture20, &rect20,
                                 hwdata->pixels[0], hwdata->pitches[0],
                                 hwdata->pixels[1], hwdata->pitches[1],
                                 hwdata->pixels[2], hwdata->pitches[2]);
        } else if (overlay12->format == SDL12_YV12_OVERLAY) {
            SDL20_UpdateYUVTexture(hwdata->texture20, &rect20,
                                 hwdata->pixels[0], hwdata->pitches[0],
                                 hwdata->pixels[2], hwdata->pitches[2],
                                 hwdata->pixels[1], hwdata->pitches[1]);
        } else {
            SDL20_UpdateTexture(hwdata->texture20, &rect20, hwdata->pixels[0], hwdata->pitches[0]);
        }

        if (overlay12->pixels == NULL) {  /* must leave it marked as dirty if still locked! */
            hwdata->dirty = SDL_FALSE;
        }
    }

    /* The app may or may not SDL_Flip() after this...queue this to render on the next present,
       and start a timer going to force a present, in case they don't. */

    overlay->overlay12 = overlay12;
    SDL20_memcpy(&overlay->dstrect12, dstrect12, sizeof (SDL12_Rect));
    overlay->next = NULL;

    SDL_assert(QueuedDisplayOverlaysTail != NULL);
    SDL_assert(QueuedDisplayOverlaysTail->next == NULL);
    QueuedDisplayOverlaysTail->next = overlay;
    QueuedDisplayOverlaysTail = overlay;

    if (!VideoSurfacePresentTicks) {
        VideoSurfacePresentTicks = VideoSurfaceLastPresentTicks + GetDesiredMillisecondsPerFrame();  /* flip it later. */
    }

    UnlockVideoRenderer();

    return 0;
}

DECLSPEC12 void SDLCALL
SDL_UnlockYUVOverlay(SDL12_Overlay *overlay12)
{
    if (overlay12) {
        overlay12->pixels = NULL;
    }
}

DECLSPEC12 void SDLCALL
SDL_FreeYUVOverlay(SDL12_Overlay *overlay12)
{
    if (overlay12) {
        SDL_Renderer *renderer = LockVideoRenderer();
        SDL12_YUVData *hwdata = (SDL12_YUVData *) overlay12->hwdata;
        QueuedOverlayItem *overlay = QueuedDisplayOverlays.next;
        while (overlay != NULL) {
            if (overlay->overlay12 == overlay12) {
                overlay->overlay12 = NULL;  /* don't try to draw this later. */
            }
            overlay = overlay->next;
        }

        if (renderer) {
            SDL20_DestroyTexture(hwdata->texture20);
            UnlockVideoRenderer();
        }

        SDL20_free(hwdata->pixelbuf);
        SDL20_free(overlay12);
    }
}

DECLSPEC12 void * SDLCALL
SDL_GL_GetProcAddress(const char *sym)
{
    /* see comments on glBindFramebuffer_shim_for_scaling for explanation */
    if ((SDL20_strcmp(sym, "glBindFramebuffer") == 0) || (SDL20_strcmp(sym, "glBindFramebufferEXT") == 0)) {
        return (void *) glBindFramebuffer_shim_for_scaling;
    }

    /* these functions all need to have an MSAA resolve inserted before use */
    if ((SDL20_strcmp(sym, "glReadPixels") == 0)) {
        return (void *) glReadPixels_shim_for_scaling;
    }
    if ((SDL20_strcmp(sym, "glCopyPixels") == 0)) {
        return (void *) glCopyPixels_shim_for_scaling;
    }
    if ((SDL20_strcmp(sym, "glCopyTexImage1D") == 0)) {
        return (void *) glCopyTexImage1D_shim_for_scaling;
    }
    if ((SDL20_strcmp(sym, "glCopyTexSubImage1D") == 0)) {
        return (void *) glCopyTexSubImage1D_shim_for_scaling;
    }
    if ((SDL20_strcmp(sym, "glCopyTexImage2D") == 0)) {
        return (void *) glCopyTexImage2D_shim_for_scaling;
    }
    if ((SDL20_strcmp(sym, "glCopyTexSubImage2D") == 0)) {
        return (void *) glCopyTexSubImage2D_shim_for_scaling;
    }
    if ((SDL20_strcmp(sym, "glCopyTexSubImage3D") == 0)) {
        return (void *) glCopyTexSubImage3D_shim_for_scaling;
    }

    /* this function is specific to the shim library */
    if ((SDL20_strcmp(sym, "SDL12COMPAT_GetWindow") == 0)) {
        return (void *) SDL12COMPAT_GetWindow;
    }
    return SDL20_GL_GetProcAddress(sym);
}

DECLSPEC12 int SDLCALL
SDL_GL_LoadLibrary(const char *libname)
{
    /* SDL 1.2 would unload the previous library if one was loaded. SDL2
       reports an error if one is already loaded, and sometimes loads it
       internally for some video targets, so unloading it probably isn't safe.
       There really isn't a good reason to be using anything other than the
       system OpenGL in 2019, so we ignore the error in this case to match 1.2
       behavior, even if you were going to load a different library this time.
       Oh well. */
    const int rc = SDL20_GL_LoadLibrary(libname);
    if (rc < 0) {
        const char *err = SDL20_GetError();
        char *dup;

        if (SDL20_strcmp(err, "OpenGL library already loaded") == 0) {
            return 0;
        }
        /* reset the actual error. */
        dup = SDL20_strdup(err);
        if (!dup) {
            SDL20_OutOfMemory();
        } else {
            SDL20_SetError(dup);
            SDL20_free(dup);
        }
    }
    return rc;
}


DECLSPEC12 int SDLCALL
SDL_GL_SetAttribute(SDL12_GLattr attr, int value)
{
    if (attr >= SDL12_GL_MAX_ATTRIBUTE) {
        return SDL20_SetError("Unknown GL attribute");
    }

    /* swap control was moved out of this API, everything else lines up. */
    if (attr == SDL12_GL_SWAP_CONTROL) {
        SwapInterval = value;
        return 0;
    }
    else if (attr == SDL12_GL_MULTISAMPLESAMPLES) {
        OpenGLLogicalScalingSamples = value;
        return 0;
    }
    else if (attr == SDL12_GL_MULTISAMPLEBUFFERS) {
        return 0;
    }
    return SDL20_GL_SetAttribute((SDL_GLattr) attr, value);
}

DECLSPEC12 int SDLCALL
SDL_GL_GetAttribute(SDL12_GLattr attr, int* value)
{
    int retval;

    if (attr >= SDL12_GL_MAX_ATTRIBUTE) {
        return SDL20_SetError("Unknown GL attribute");
    }
    /* swap control was moved out of this API, everything else lines up. */
    if (attr == SDL12_GL_SWAP_CONTROL) {
        *value = SDL20_GL_GetSwapInterval();
        return 0;
    }
    else if (attr == SDL12_GL_MULTISAMPLESAMPLES) {
        *value = OpenGLLogicalScalingSamples;
        return 0;
    }
    else if (attr == SDL12_GL_MULTISAMPLEBUFFERS) {
        *value = (OpenGLLogicalScalingSamples) ? 1 : 0;
        return 0;
    }

    /* SDL2 has a bug where FBO 0 must be bound or SDL20_GL_GetAttribute gives incorrect information.
       See https://github.com/libsdl-org/sdl12-compat/issues/150 */
    if (OpenGLCurrentDrawFBO == 0) {
        retval = SDL20_GL_GetAttribute((SDL_GLattr) attr, value);
    } else {
        SDL_assert(OpenGLFuncs.glBindFramebuffer != NULL);
        OpenGLFuncs.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        retval = SDL20_GL_GetAttribute((SDL_GLattr) attr, value);
        OpenGLFuncs.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGLCurrentDrawFBO);
    }

    return retval;
}


DECLSPEC12 void SDLCALL
SDL_GL_SwapBuffers(void)
{
    if (VideoWindow20) {
        /* Some applications, e.g. Awesomenauts, play with glXMakeCurrent() behind our backs and break SwapBuffers() */
        if (ForceGLSwapBufferContext) {
            SDL20_GL_MakeCurrent(VideoWindow20, VideoGLContext20);
        }

        if (OpenGLLogicalScalingFBO != 0) {
            const GLboolean has_scissor = OpenGLFuncs.glIsEnabled(GL_SCISSOR_TEST);
            int physical_w, physical_h;
            GLfloat clearcolor[4];
            SDL_Rect dstrect;

            /* use the drawable size, which is != window size for HIGHDPI systems */
            SDL20_GL_GetDrawableSize(VideoWindow20, &physical_w, &physical_h);
            dstrect = GetOpenGLLogicalScalingViewport(physical_w, physical_h);

            OpenGLFuncs.glGetFloatv(GL_COLOR_CLEAR_VALUE, clearcolor);

            if (has_scissor) {
                OpenGLFuncs.glDisable(GL_SCISSOR_TEST);  /* scissor test affects framebuffer_blit */
            }

            OpenGLFuncs.glBindFramebuffer(GL_READ_FRAMEBUFFER, OpenGLLogicalScalingFBO);

            /* Resolve the multisample framebuffer if required. */
            if (OpenGLLogicalScalingMultisampleFBO) {
                OpenGLFuncs.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGLLogicalScalingMultisampleFBO);
                OpenGLFuncs.glBlitFramebuffer(0, 0, OpenGLLogicalScalingWidth, OpenGLLogicalScalingHeight,
                                              0, 0, OpenGLLogicalScalingWidth, OpenGLLogicalScalingHeight,
                                              GL_COLOR_BUFFER_BIT, GL_NEAREST);
                OpenGLFuncs.glBindFramebuffer(GL_READ_FRAMEBUFFER, OpenGLLogicalScalingMultisampleFBO);
            }

            OpenGLFuncs.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            OpenGLFuncs.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            OpenGLFuncs.glClear(GL_COLOR_BUFFER_BIT);
            OpenGLFuncs.glBlitFramebuffer(0, 0, OpenGLLogicalScalingWidth, OpenGLLogicalScalingHeight,
                                          dstrect.x, dstrect.y, dstrect.x + dstrect.w, dstrect.y + dstrect.h,
                                          GL_COLOR_BUFFER_BIT, WantScaleMethodNearest ? GL_NEAREST : GL_LINEAR);
            OpenGLFuncs.glBindFramebuffer(GL_FRAMEBUFFER, 0);
            SDL20_GL_SwapWindow(VideoWindow20);
            OpenGLFuncs.glClearColor(clearcolor[0], clearcolor[1], clearcolor[2], clearcolor[3]);
            if (has_scissor) {
                OpenGLFuncs.glEnable(GL_SCISSOR_TEST);
            }
            OpenGLFuncs.glBindFramebuffer(GL_READ_FRAMEBUFFER, OpenGLCurrentReadFBO);
            OpenGLFuncs.glBindFramebuffer(GL_DRAW_FRAMEBUFFER, OpenGLCurrentDrawFBO);
        } else {
            SDL20_GL_SwapWindow(VideoWindow20);
        }
    }
}

DECLSPEC12 int SDLCALL
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

DECLSPEC12 int SDLCALL
SDL_SetGammaRamp(const Uint16 *red, const Uint16 *green, const Uint16 *blue)
{
    return SDL20_SetWindowGammaRamp(VideoWindow20, red, green, blue);
}

DECLSPEC12 int SDLCALL
SDL_GetGammaRamp(Uint16 *red, Uint16 *green, Uint16 *blue)
{
    return SDL20_GetWindowGammaRamp(VideoWindow20, red, green, blue);
}

DECLSPEC12 int SDLCALL
SDL_EnableKeyRepeat(int delay, int interval)
{
    if ((delay < 0) || (interval < 0)) {
        return SDL20_SetError("Invalid key repeat values");
    }

    KeyRepeatEvent.type = SDL12_NOEVENT;
    KeyRepeatNextTicks = 0;
    KeyRepeatDelay = (Uint32) delay;
    KeyRepeatInterval = (Uint32) interval;
    return 0;
}

DECLSPEC12 void SDLCALL
SDL_GetKeyRepeat(int *delay, int *interval)
{
    if (delay) {
        *delay = (int) KeyRepeatDelay;
    }
    if (interval) {
        *interval = (int) KeyRepeatInterval;
    }
}

DECLSPEC12 int SDLCALL
SDL_EnableUNICODE(int enable)
{
    const int old = EnabledUnicode;
    if (enable >= 0) {
        EnabledUnicode = enable;
        if (enable) {
            SDL20_StartTextInput();
        } else {
            SDL20_StopTextInput();
        }
    }
    return old;
}

/* SDL 1.2 limited timers to a 10ms resolution. SDL 2.0 doesn't, so we might
   have to round. This is the equation 1.2 uses: */
static Uint32 RoundTimerTo12Resolution(const Uint32 ms)
{
    return (((ms + 10) - 1) / 10) * 10;
}


static Uint32 SDLCALL
SetTimerCallback12(Uint32 interval, void* param)
{
    return RoundTimerTo12Resolution(((SDL12_TimerCallback)param)(interval));
}

DECLSPEC12 int SDLCALL
SDL_SetTimer(Uint32 interval, SDL12_TimerCallback callback)
{
    static SDL_TimerID compat_timer;

    if (compat_timer) {
        SDL20_RemoveTimer(compat_timer);
        compat_timer = 0;
    }

    if (interval && callback) {
        interval = RoundTimerTo12Resolution(interval);
        compat_timer = SDL20_AddTimer(interval, SetTimerCallback12, (void*)callback);
        if (!compat_timer) {
            return -1;
        }
    }
    return 0;
}

DECLSPEC12 void SDLCALL
SDL_Delay(Uint32 ticks)
{
    /* In case there's a loading screen from a background thread and the main thread is waiting... */
    const SDL_bool ThisIsSetVideoModeThread = (SDL20_ThreadID() == SetVideoModeThread) ? SDL_TRUE : SDL_FALSE;
    if (ThisIsSetVideoModeThread) {
        if (VideoSurfaceUpdatedInBackgroundThread) {
            SDL_Flip(VideoSurface12);  /* this will update the texture and present. */
        } else if (VideoSurfacePresentTicks) {
            PresentScreen();
        }
    }

    SDL20_Delay(ticks);
}


DECLSPEC12 int SDLCALL
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


#if (defined(_WIN32) || defined(__OS2__)) && !defined(SDL_PASSED_BEGINTHREAD_ENDTHREAD)
#error SDL_PASSED_BEGINTHREAD_ENDTHREAD not defined
#endif
#ifdef SDL_PASSED_BEGINTHREAD_ENDTHREAD
#ifdef _WIN32
/* Official Windows versions of SDL-1.2 >= 1.2.10 were always built
 * with HAVE_LIBC, i.e.: *without* SDL_PASSED_BEGINTHREAD_ENDTHREAD
 * defined, in order to keep binary compatibility with SDL <= 1.2.9.
 *
 * On the other hand SDL2 >= 2.0.12, as we dictate for Windows does
 * always define SDL_PASSED_BEGINTHREAD_ENDTHREAD, in order to keep
 * SDL2 versions built with and without libc binary compatible with
 * each other.
 *
 * Therefore, we have to do the following trick below. */
DECLSPEC12 SDL_Thread * SDLCALL
SDL_CreateThread(int (SDLCALL *fn)(void *), void *data)
{
    return SDL20_CreateThread(fn, NULL, data, NULL, NULL);
}
#else
DECLSPEC12 SDL_Thread * SDLCALL
SDL_CreateThread(int (SDLCALL *fn)(void *), void *data, pfnSDL_CurrentBeginThread pfnBeginThread, pfnSDL_CurrentEndThread pfnEndThread)
{
    return SDL20_CreateThread(fn, NULL, data, pfnBeginThread, pfnEndThread);
}
#endif
#else
DECLSPEC12 SDL_Thread * SDLCALL
SDL_CreateThread(int (SDLCALL *fn)(void *), void *data)
{
    return SDL20_CreateThread(fn, NULL, data);
}
#endif

/* These two will truncate the returned value on LP64 systems,
 * a shortcoming of SDL-1.2. */
DECLSPEC12 Uint32 SDLCALL SDL_ThreadID(void)
{
    return SDL20_ThreadID();
}
DECLSPEC12 Uint32 SDLCALL SDL_GetThreadID(SDL_Thread *thread)
{
    return SDL20_GetThreadID(thread);
}

DECLSPEC12 int SDLCALL
SDL_mutexP(SDL_mutex *mutex)
{
    return SDL20_LockMutex(mutex);
}

DECLSPEC12 int SDLCALL
SDL_mutexV(SDL_mutex *mutex)
{
    return SDL20_UnlockMutex(mutex);
}

DECLSPEC12 void SDLCALL
SDL_KillThread(SDL_Thread *thread)
{
    (void)thread;
    SDL20_Log("WARNING: this app used SDL_KillThread(), an unforgivable curse.\n"
              "This program should be fixed. No thread was actually harmed.\n");
}

static Uint32 SDLCALL
AddTimerCallback12(Uint32 interval, void *param)
{
    SDL12_TimerID data = (SDL12_TimerID) param;
    return RoundTimerTo12Resolution(data->callback(interval, data->param));
}

DECLSPEC12 SDL12_TimerID SDLCALL
SDL_AddTimer(Uint32 interval, SDL12_NewTimerCallback callback, void *param)
{
    SDL12_TimerID data = (SDL12_TimerID) SDL20_malloc(sizeof (SDL12_TimerID_Data));
    if (!data) {
        SDL20_OutOfMemory();
        return NULL;
    }

    interval = RoundTimerTo12Resolution(interval);
    data->callback = callback;
    data->param = param;
    data->timer_id = SDL20_AddTimer(interval, AddTimerCallback12, data);

    if (!data->timer_id) {
        SDL20_free(data);
        return NULL;
    }

    if (EventQueueMutex) {
        SDL20_LockMutex(EventQueueMutex);
    }

    data->prev = NULL;
    data->next = AddedTimers;
    if (AddedTimers) {
        AddedTimers->prev = data;
    }
    AddedTimers = data;

    if (EventQueueMutex) {
        SDL20_UnlockMutex(EventQueueMutex);
    }

    return data;
}

DECLSPEC12 SDL_bool SDLCALL
SDL_RemoveTimer(SDL12_TimerID data)
{
    SDL_bool retval = SDL_FALSE;
    if (data) {
        /* SDL 1.2 would make sure the pointer was valid and return false instead of crashing, so we check that too. */
        SDL12_TimerID i;

        if (EventQueueMutex) {
            SDL20_LockMutex(EventQueueMutex);
        }

        for (i = AddedTimers; i != NULL; i = i->next) {
            if (i == data) {
                break;
            }
        }

        if (i != NULL) {  /* this is valid. */
            if (data->prev) {
                data->prev->next = data->next;
            }
            if (data->next) {
                data->next->prev = data->prev;
            }
            if (data == AddedTimers) {
                AddedTimers = data->next;
            }
            retval = SDL_TRUE;
            SDL20_RemoveTimer(data->timer_id);
        }

        if (EventQueueMutex) {
            SDL20_UnlockMutex(EventQueueMutex);
        }

        if (retval) {
            SDL20_free(data);
        }
    }

    return retval;
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


DECLSPEC12 SDL12_RWops * SDLCALL
SDL_AllocRW(void)
{
    SDL12_RWops *rwops = (SDL12_RWops *) SDL20_malloc(sizeof (SDL12_RWops));
    if (!rwops) {
        SDL20_OutOfMemory();
    }
    return rwops;
}

DECLSPEC12 void SDLCALL
SDL_FreeRW(SDL12_RWops *rwops12)
{
    SDL20_free(rwops12);
}

static int SDLCALL
RWops20to12_seek(struct SDL12_RWops *rwops12, int offset, int whence)
{
    return (int)rwops12->rwops20->seek(rwops12->rwops20, offset, whence);
}

static int SDLCALL
RWops20to12_read(struct SDL12_RWops *rwops12, void *ptr, int size, int maxnum)
{
    return (int)rwops12->rwops20->read(rwops12->rwops20, ptr, size, maxnum);
}

static int SDLCALL
RWops20to12_write(struct SDL12_RWops *rwops12, const void *ptr, int size, int num)
{
    return (int)rwops12->rwops20->write(rwops12->rwops20, ptr, size, num);
}

static int SDLCALL
RWops20to12_close(struct SDL12_RWops *rwops12)
{
    int rc = 0;
    if (rwops12) {
        rc = rwops12->rwops20->close(rwops12->rwops20);
        if (rc == 0) {
            SDL_FreeRW(rwops12);
        }
    }
    return rc;
}

static SDL12_RWops *
RWops20to12(SDL_RWops *rwops20)
{
    SDL12_RWops *rwops12;

    if (!rwops20) {
        return NULL;
    }

    rwops12 = SDL_AllocRW();
    if (!rwops12) {
        return NULL;
    }

    SDL20_zerop(rwops12);
    rwops12->type = rwops20->type;
    rwops12->rwops20 = rwops20;
    rwops12->seek = RWops20to12_seek;
    rwops12->read = RWops20to12_read;
    rwops12->write = RWops20to12_write;
    rwops12->close = RWops20to12_close;

    return rwops12;
}

DECLSPEC12 SDL12_RWops * SDLCALL
SDL_RWFromFile(const char *file, const char *mode)
{
    if (!file || !*file || !mode || !*mode) {
        SDL20_SetError("SDL_RWFromFile(): No file or no mode specified");
        return NULL;
    }
    return RWops20to12(SDL20_RWFromFile(file, mode));
}

DECLSPEC12 SDL12_RWops * SDLCALL
SDL_RWFromFP(void *io, int autoclose)
{
    return RWops20to12(SDL20_RWFromFP(io, autoclose));
}

DECLSPEC12 SDL12_RWops * SDLCALL
SDL_RWFromMem(void *mem, int size)
{
    return RWops20to12(SDL20_RWFromMem(mem, size));
}

DECLSPEC12 SDL12_RWops * SDLCALL
SDL_RWFromConstMem(const void *mem, int size)
{
    return RWops20to12(SDL20_RWFromConstMem(mem, size));
}

#define READ_AND_BYTESWAP(endian, bits) \
    DECLSPEC12 Uint##bits SDLCALL SDL_Read##endian##bits(SDL12_RWops *rwops12) { \
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
    DECLSPEC12 int SDLCALL SDL_Write##endian##bits(SDL12_RWops *rwops12, Uint##bits val) { \
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
    int size = (int) ((intptr_t) rwops20->hidden.unknown.data2);
    int pos;

    if (size != -1) {
        return size;
    }

    pos = rwops12->seek(rwops12, 0, RW_SEEK_CUR);
    if (pos == -1) {
        return SDL20_Error(SDL_EFSEEK);
    }
    size = rwops12->seek(rwops12, 0, RW_SEEK_END);
    rwops12->seek(rwops12, pos, RW_SEEK_SET);
    rwops20->hidden.unknown.data2 = (void *) ((intptr_t) size);
    return size;
}

static Sint64 SDLCALL
RWops12to20_seek(struct SDL_RWops *rwops20, Sint64 offset, int whence)
{
    SDL12_RWops *rwops12 = (SDL12_RWops *) rwops20->hidden.unknown.data1;
    if (offset < INT_MIN || offset > INT_MAX) {
        return SDL20_InvalidParamError("offset");
    }
    return (Sint64) rwops12->seek(rwops12, (int) offset, whence);
}

static size_t SDLCALL
RWops12to20_read(struct SDL_RWops *rwops20, void *ptr, size_t size, size_t maxnum)
{
    SDL12_RWops *rwops12 = (SDL12_RWops *) rwops20->hidden.unknown.data1;
    if (size > INT_MAX || maxnum > INT_MAX) {
        SDL20_InvalidParamError("size' or 'num");
        return 0;
    }
    return (size_t) rwops12->read(rwops12, ptr, (int) size, (int) maxnum);
}

static size_t SDLCALL
RWops12to20_write(struct SDL_RWops *rwops20, const void *ptr, size_t size, size_t num)
{
    SDL12_RWops *rwops12 = (SDL12_RWops *) rwops20->hidden.unknown.data1;
    if (size > INT_MAX || num > INT_MAX) {
        SDL20_InvalidParamError("size' or 'num");
        return 0;
    }
    return (size_t) rwops12->write(rwops12, ptr, (int) size, (int) num);
}

static int SDLCALL
RWops12to20_close(struct SDL_RWops *rwops20)
{
    int rc = 0;
    if (rwops20) {
        SDL12_RWops *rwops12 = (SDL12_RWops *) rwops20->hidden.unknown.data1;
        rc = rwops12->close(rwops12);
        SDL20_FreeRW(rwops20);
    }
    return rc;
}

static SDL_RWops *
RWops12to20(SDL12_RWops *rwops12)
{
    SDL_RWops *rwops20;

    if (!rwops12) {
        return NULL;
    }

    rwops20 = SDL20_AllocRW();
    if (!rwops20) {
        return NULL;
    }

    SDL20_zerop(rwops20);
    rwops20->type = rwops12->type;
    rwops20->hidden.unknown.data1 = rwops12;
    rwops20->hidden.unknown.data2 = (void *) ((intptr_t) -1);   /* cached size of stream */
    rwops20->size = RWops12to20_size;
    rwops20->seek = RWops12to20_seek;
    rwops20->read = RWops12to20_read;
    rwops20->write = RWops12to20_write;
    rwops20->close = RWops12to20_close;
    return rwops20;
}

DECLSPEC12 SDL12_Surface * SDLCALL
SDL_LoadBMP_RW(SDL12_RWops *rwops12, int freerwops12)
{
    SDL_RWops *rwops20 = RWops12to20(rwops12);
    SDL_Surface *surface20 = SDL20_LoadBMP_RW(rwops20, freerwops12);
    SDL12_Surface *surface12 = Surface20to12(surface20);
    if (!freerwops12) { /* free our wrapper if SDL2 didn't close it. */
        SDL20_FreeRW(rwops20);
    }
    if ((!surface12) && (surface20)) {
        SDL20_FreeSurface(surface20);
    }
    return surface12;
}

DECLSPEC12 int SDLCALL
SDL_SaveBMP_RW(SDL12_Surface *surface12, SDL12_RWops *rwops12, int freerwops12)
{
    SDL_RWops *rwops20 = RWops12to20(rwops12);
    const int retval = SDL20_SaveBMP_RW(surface12->surface20, rwops20, freerwops12);
    if (!freerwops12) {  /* free our wrapper if SDL2 didn't close it. */
        SDL20_FreeRW(rwops20);
    }
    return retval;
}

DECLSPEC12 SDL_AudioSpec * SDLCALL
SDL_LoadWAV_RW(SDL12_RWops *rwops12, int freerwops12,
               SDL_AudioSpec *spec, Uint8 **buf, Uint32 *len)
{
    SDL_RWops *rwops20 = RWops12to20(rwops12);
    SDL_AudioSpec *retval = SDL20_LoadWAV_RW(rwops20, freerwops12, spec, buf, len);
    if (retval && retval->format & 0x20) {
        SDL20_SetError("Unsupported 32-bit PCM data format");
        SDL20_FreeWAV(*buf);
        *buf = NULL;
        retval = NULL;
    }
    if (!freerwops12) {  /* free our wrapper if SDL2 didn't close it. */
        SDL20_FreeRW(rwops20);
    }
    return retval;
}


/* CD-ROM API!
   We don't support physical CD drives in sdl12-compat. In modern times, it's
   hard to find discs at all, let alone discs with audio tracks. Drives are
   also getting scarce, and ones that are plugged into the sound output
   hardware moreso. With this in mind, sdl12-compat can be instructed to
   point to a filesystem directory full .mp3 files, and will pretend this is
   an audio CD-ROM, and will decode these files and mix them into an audio
   stream as if they were playing from a disc. */

#if defined(_MSC_VER) && defined(_M_IX86)
#include "x86_msvc.h"
#endif

#define CDAUDIO_FPS 75  /* CD audio frames per second. */

/* public domain, single-header MP3 decoder for fake CD-ROM audio support! */
#define DR_MP3_IMPLEMENTATION
#if defined(__GNUC__) && (__GNUC__ >= 4) && \
  !(defined(_WIN32) || defined(__EMX__))
#define DRMP3_API __attribute__((visibility("hidden")))
#elif defined(__APPLE__)
#define DRMP3_API __private_extern__
#else
#define DRMP3_API /* just in case */
#endif
#define DR_MP3_NO_STDIO 1
#define DR_MP3_NO_S16 1
#define DR_MP3_FLOAT_OUTPUT 1
#define DR_MP3_NO_FULL_READ 1
#define DRMP3_ASSERT(x) SDL_assert((x))
#define DRMP3_MALLOC(sz) SDL20_malloc((sz))
#define DRMP3_REALLOC(p, sz) SDL20_realloc((p), (sz))
#define DRMP3_FREE(p) SDL20_free((p))
#define DRMP3_COPY_MEMORY(dst, src, sz) SDL20_memcpy((dst), (src), (sz))
#define DRMP3_MOVE_MEMORY(dst, src, sz) SDL20_memmove((dst), (src), (sz))
#define DRMP3_ZERO_MEMORY(p, sz) SDL20_memset((p), 0, (sz))

#include "dr_mp3.h"

static SDL_INLINE Sint64 SDLCALL SDL20_RWseek(SDL_RWops *ctx, Sint64 ofs, int whence) {
    return ctx->seek(ctx, ofs, whence);
}
static SDL_INLINE size_t SDLCALL SDL20_RWread(SDL_RWops *ctx, void *ptr, size_t size, size_t n) {
    return ctx->read(ctx, ptr, size, n);
}
static SDL_INLINE int SDLCALL SDL20_RWclose(SDL_RWops *ctx) {
    return ctx->close(ctx);
}

static size_t
mp3_sdlrwops_read(void *data, void *buf, size_t bytesToRead)
{
    return SDL20_RWread((SDL_RWops *) data, buf, 1, bytesToRead);
}

static drmp3_bool32
mp3_sdlrwops_seek(void *data, int offset, drmp3_seek_origin origin)
{
    const int whence = (origin == drmp3_seek_origin_start) ? RW_SEEK_SET : RW_SEEK_CUR;
    SDL_assert((origin == drmp3_seek_origin_start) || (origin == drmp3_seek_origin_current));
    return (SDL20_RWseek((SDL_RWops *) data, offset, whence) == -1) ? DRMP3_FALSE : DRMP3_TRUE;
}


static SDL_bool OpenSDL2AudioDevice(SDL_AudioSpec *want);
static int CloseSDL2AudioDevice(void);
static SDL_bool ResetAudioStream(SDL_AudioStream **_stream, SDL_AudioSpec *spec, const SDL_AudioSpec *to, const SDL_AudioFormat fromfmt, const Uint8 fromchannels, const int fromfreq);

typedef struct
{
    SDL_AudioSpec device_format;

    SDL_bool app_callback_opened;
    SDL_AudioSpec app_callback_format;
    SDL_AudioStream *app_callback_stream;

    SDL_bool cdrom_opened;
    SDL_AudioSpec cdrom_format;
    SDL_AudioStream *cdrom_stream;

    SDL12_CDstatus cdrom_status;
    int cdrom_pcm_frames_written;
    int cdrom_cur_track;
    int cdrom_cur_frame;
    int cdrom_stop_ntracks;
    int cdrom_stop_nframes;
    drmp3 cdrom_mp3;

    Uint8 *mix_buffer;
    size_t mixbuflen;
} AudioCallbackWrapperData;

static AudioCallbackWrapperData *audio_cbdata = NULL;
static SDL_atomic_t audio_callback_paused;


static void
FreeMp3(drmp3 *mp3)
{
    SDL_RWops *rw = (SDL_RWops *) mp3->pUserData;
    if (rw) {
        drmp3_uninit(mp3);
        mp3->pUserData = NULL;
        SDL20_RWclose(rw);
    }
}


static SDL_bool
CDSubsystemIsInitialized(void)
{
    if (!CDRomInit) {
        SDL20_SetError("CD-ROM subsystem not initialized");
        return SDL_FALSE;
    }
    return SDL_TRUE;
}

/* This never reports failure; if there's a problem, we report zero drives found. */
static void
InitializeCDSubsystem(void)
{
    const char *cdpath;

    if (CDRomInit) {
        return;
    }

    cdpath = SDL12Compat_GetHint("SDL12COMPAT_FAKE_CDROM_PATH");
    if (cdpath) {
        CDRomPath = SDL_strdup(cdpath);
    }

    CDRomInit = SDL_TRUE;
}

static void
QuitCDSubsystem(void)
{
    if (!CDRomInit) {
        return;
    }
    SDL_free(CDRomPath);
    CDRomPath = NULL;
    CDRomInit = SDL_FALSE;
}

DECLSPEC12 int SDLCALL
SDL_CDNumDrives(void)
{
    if (!CDSubsystemIsInitialized()) {
        return -1;
    }

    if (!CDRomPath) {
        static SDL_bool warned_once = SDL_FALSE;
        if (!warned_once) {
            warned_once = SDL_TRUE;
            SDL20_Log("This app is looking for CD-ROM drives, but no path was specified");
            SDL20_Log("Set the SDL12COMPAT_FAKE_CDROM_PATH environment variable to a directory");
            SDL20_Log("of MP3 files named trackXX.mp3 where XX is a track number in two digits");
            SDL20_Log("from 01 to 99");
        }
    }

    return CDRomPath ? 1 : 0;
}

static SDL_bool
ValidCDDriveIndex(const int drive)
{
    if (!CDSubsystemIsInitialized()) {
        return SDL_FALSE;
    }

    if (!CDRomPath || (drive != 0)) {
        SDL20_SetError("Invalid CD-ROM drive index");
        return SDL_FALSE;
    }

    return SDL_TRUE;
}

DECLSPEC12 const char * SDLCALL
SDL_CDName(int drive)
{
    return ValidCDDriveIndex(drive) ? CDRomPath : NULL;
}

DECLSPEC12 SDL12_CD * SDLCALL
SDL_CDOpen(int drive)
{
    SDL12_CD *retval;
    size_t alloclen;
    char *fullpath;
    drmp3 *mp3 = NULL;
    Uint32 total_track_offset = 0;
    SDL_bool has_audio = SDL_FALSE;

    if (!ValidCDDriveIndex(drive)) {
        return NULL;
    }

    retval = (SDL12_CD *) SDL20_calloc(1, sizeof(SDL12_CD));
    if (!retval) {
        SDL20_OutOfMemory();
        return NULL;
    }

    alloclen = SDL20_strlen(CDRomPath) + 32;
    fullpath = (char *) SDL20_malloc(alloclen);
    if (fullpath == NULL) {
        SDL20_free(retval);
        SDL20_OutOfMemory();
        return NULL;
    }

    mp3 = (drmp3 *) SDL20_malloc(sizeof (drmp3));
    if (!mp3) {
        SDL20_free(fullpath);
        SDL20_free(retval);
        SDL20_OutOfMemory();
        return NULL;
    }

    /* We would do a proper enumeration of this directory, but that
       would need platform-specific code that SDL2 doesn't offer.
       readdir() is surprisingly hard to do without a bunch of different
       platform backends! We just open files until we fail to do so,
       and then stop. */
    for (;;) {
        SDL_RWops *rw;
        drmp3_uint64 pcmframes;
        drmp3_uint32 samplerate;
        SDL12_CDtrack *track;
        SDL_bool fake_data_track = SDL_FALSE;
        int c;
        char c0, c1;

        c = retval->numtracks + 1;
        c0 = c / 10 + '0';
        c1 = c % 10 + '0';

        /* note that fake data track files just need to exist, they can be empty files. */
        SDL20_snprintf(fullpath, alloclen, "%s%strack%c%c.dat", CDRomPath, DIRSEP, c0, c1);
        rw = SDL20_RWFromFile(fullpath, "rb");
        if (rw) {  /* fake data track. */
            fake_data_track = SDL_TRUE;
            SDL20_RWclose(rw);
            rw = NULL;
        } else {
            SDL20_snprintf(fullpath, alloclen, "%s%strack%c%c.mp3", CDRomPath, DIRSEP, c0, c1);
            rw = SDL20_RWFromFile(fullpath, "rb");
            /* if there isn't a track 1 specified, pretend it's a data track, which matches most games' needs. */
            if (!rw && (c == 0)) {
                fake_data_track = SDL_TRUE;
            }
        }

        if (!rw && !fake_data_track) {
            break;  /* ok, we're done looking for more. */
        }
        track = &retval->track[retval->numtracks];
        if (!fake_data_track) {
            SDL_assert(rw != NULL);
            if (!drmp3_init(mp3, mp3_sdlrwops_read, mp3_sdlrwops_seek, rw, NULL)) {
                SDL20_RWclose(rw);
                rw = NULL;
                fake_data_track = SDL_TRUE; /* congratulations, bogus or unsupported MP3, you just became data! */
            } else {
                pcmframes = drmp3_get_pcm_frame_count(mp3);
                samplerate = mp3->sampleRate;
                FreeMp3(mp3);
                rw = NULL;

                track->id = retval->numtracks;
                track->type = 0;  /* audio track. Data tracks are 4. */
                track->length = (Uint32) ((((double) pcmframes) / ((double) samplerate)) * CDAUDIO_FPS);
                track->offset = total_track_offset;
                total_track_offset += track->length;

                has_audio = SDL_TRUE;
            }
        }

        SDL_assert(rw == NULL);  /* we should have dealt with this in all paths. */

        if (fake_data_track) {
            track->type = 4; /* data track. E.g.: quake's audio starts at track 2. */
        }

        retval->numtracks++;

        if (retval->numtracks == 99) {
            break;  /* max tracks you can have on an audio CD. */
        }
    }

    if (!has_audio) {
        retval->numtracks = 0; /* data-only */
    }
    SDL20_free(mp3);
    SDL20_free(fullpath);

    retval->id = 1;  /* just to be non-zero, I guess. */
    retval->status = (retval->numtracks > 0) ? SDL12_CD_STOPPED : SDL12_CD_TRAYEMPTY;

    if (retval->numtracks > 0) {
        SDL_AudioSpec want;
        SDL_zero(want);
        want.freq = 44100;
        want.format = AUDIO_F32SYS;
        want.channels = 2;
        want.samples = 4096;

        if (!OpenSDL2AudioDevice(&want)) {
            retval->numtracks = 0;
            retval->status = SDL12_CD_TRAYEMPTY;
        } else {
            /* Device is locked now, even if was opened and playing before. Set up some things. */
            SDL20_memcpy(&audio_cbdata->cdrom_format, &want, sizeof (SDL_AudioSpec));
            audio_cbdata->cdrom_opened = SDL_TRUE;
            audio_cbdata->cdrom_status = SDL12_CD_STOPPED;
            audio_cbdata->cdrom_pcm_frames_written = 0;
            audio_cbdata->cdrom_cur_track = 0;
            audio_cbdata->cdrom_cur_frame = 0;
            SDL20_UnlockAudio();
        }
    }

    CDRomDevice = retval;  /* NULL API args use the last opened device. */

    return retval;
}

static SDL12_CD *
ValidCDDevice(SDL12_CD *cdrom)
{
    if (!CDSubsystemIsInitialized()) {
        return NULL;
    } else if (!cdrom) {
        if (!CDRomDevice) {
            SDL20_SetError("CD-ROM not opened");
        } else {
            cdrom = CDRomDevice;
        }
    }
    return cdrom;
}


DECLSPEC12 SDL12_CDstatus SDLCALL
SDL_CDStatus(SDL12_CD *cdrom)
{
    SDL12_CDstatus retval;

    if ((cdrom = ValidCDDevice(cdrom)) == NULL) {
        return SDL12_CD_ERROR;
    }

    SDL20_LockAudio();  /* we update this during the audio callback. */
    if (audio_cbdata) {
        cdrom->status = audio_cbdata->cdrom_status;
        cdrom->cur_track = audio_cbdata->cdrom_cur_track;
        cdrom->cur_frame = audio_cbdata->cdrom_cur_frame;
    }
    retval = cdrom->status;
    SDL20_UnlockAudio();

    return retval;
}

static SDL_bool
LoadCDTrack(const int tracknum, drmp3 *mp3)
{
    const SDL_AudioSpec *have = &audio_cbdata->device_format;
    SDL_RWops *rw = NULL;
    const size_t alloclen = SDL20_strlen(CDRomPath) + 32;
    char *fullpath = (char *) SDL_malloc(alloclen);
    const int c = tracknum + 1;
    char c0, c1;

    if (!fullpath) {
        return SDL_FALSE;
    }

    c0 = c / 10 + '0';
    c1 = c % 10 + '0';
    SDL20_snprintf(fullpath, alloclen, "%s%strack%c%c.mp3", CDRomPath, DIRSEP, c0, c1);
    rw = SDL20_RWFromFile(fullpath, "rb");
    SDL20_free(fullpath);

    if (!rw) {
        return SDL_FALSE;
    }

    if (!drmp3_init(mp3, mp3_sdlrwops_read, mp3_sdlrwops_seek, rw, NULL)) {
        SDL20_RWclose(rw);
        return SDL_FALSE;
    }

    if (!ResetAudioStream(&audio_cbdata->cdrom_stream, &audio_cbdata->cdrom_format, have, AUDIO_F32SYS, mp3->channels, mp3->sampleRate)) {
        FreeMp3(mp3);
        return SDL_FALSE;
    }

    return SDL_TRUE;
}

static int
StartCDAudioPlaying(SDL12_CD *cdrom, const int start_track, const int start_frame, const int ntracks, const int nframes)
{
    drmp3 *mp3 = (drmp3 *) SDL20_malloc(sizeof (drmp3));
    const SDL_bool loaded = mp3 ? LoadCDTrack(start_track, mp3) : SDL_FALSE;
    const SDL_bool seeking = (loaded && (start_frame > 0))? SDL_TRUE : SDL_FALSE;
    const drmp3_uint64 pcm_frame = seeking ? ((drmp3_uint64) ((start_frame / 75.0) * mp3->sampleRate)) : 0;

    if (!mp3) {
        return SDL20_OutOfMemory();
    }

    if (seeking) {   /* do seeking before handing off to the audio thread. */
        drmp3_seek_to_pcm_frame(mp3, pcm_frame);
    }

    SDL20_LockAudio();
    if (audio_cbdata) {
        cdrom->status = audio_cbdata->cdrom_status = loaded ? SDL12_CD_PLAYING : SDL12_CD_TRAYEMPTY;
        audio_cbdata->cdrom_pcm_frames_written = (int) pcm_frame;
        audio_cbdata->cdrom_cur_track = start_track;
        audio_cbdata->cdrom_cur_frame = start_frame;
        audio_cbdata->cdrom_stop_ntracks = ntracks;
        audio_cbdata->cdrom_stop_nframes = nframes;
        FreeMp3(&audio_cbdata->cdrom_mp3);
        if (loaded) {
            SDL20_memcpy(&audio_cbdata->cdrom_mp3, mp3, sizeof (drmp3));
        }
    }
    SDL20_UnlockAudio();

    SDL20_free(mp3);

    return loaded ? 0 : SDL20_SetError("Failed to start CD track");
}


DECLSPEC12 int SDLCALL
SDL_CDPlayTracks(SDL12_CD *cdrom, int start_track, int start_frame, int ntracks, int nframes)
{
    if ((cdrom = ValidCDDevice(cdrom)) == NULL) {
        return -1;
    } else if (cdrom->status == SDL12_CD_TRAYEMPTY) {
        return SDL20_SetError("Tray empty");
    } else if ((start_track < 0) || (start_track >= cdrom->numtracks)) {
        return SDL20_SetError("Invalid start track");
    } else if ((start_frame < 0) || (((Uint32) start_frame) >= cdrom->track[start_track].length)) {
        return SDL20_SetError("Invalid start frame");
    } else if ((ntracks < 0) || ((start_track + ntracks) >= cdrom->numtracks)) {
        return SDL20_SetError("Invalid number of tracks");
    } else if ((nframes < 0) || (((Uint32) nframes) >= cdrom->track[start_track + ntracks].length)) {
        return SDL20_SetError("Invalid number of frames");
    }

    if (!ntracks && !nframes) {
        ntracks = cdrom->numtracks - start_track;
        nframes = cdrom->track[cdrom->numtracks - 1].length;
    }

    return StartCDAudioPlaying(cdrom, start_track, start_frame, ntracks, nframes);
}

DECLSPEC12 int SDLCALL
SDL_CDPlay(SDL12_CD *cdrom, int start, int length)
{
    const Uint32 ui32start = (Uint32) start;
    Uint32 remain = (Uint32) length;
    int start_track = -1;
    int start_frame = -1;
    int ntracks = -1;
    int nframes = -1;
    int i;

    if ((cdrom = ValidCDDevice(cdrom)) == NULL) {
        return -1;
    } else if (cdrom->status == SDL12_CD_TRAYEMPTY) {
        return SDL20_SetError("Tray empty");
    } else if (start < 0) {
        return SDL20_SetError("Invalid start");
    } else if (length < 0) {
        return SDL20_SetError("Invalid length");
    }

    for (i = 0; i < cdrom->numtracks; i++) {
        if ((ui32start >= cdrom->track[i].offset) && (ui32start < (cdrom->track[i].offset + cdrom->track[i].length))) {
            start_track = i;
            break;
        }
    }

    if (start_track == -1) {
        return SDL20_SetError("Invalid start");
    }

    start_frame = start - cdrom->track[start_track].offset;

    if (remain < (cdrom->track[start_track].length - start_frame)) {
        ntracks = 0;
        nframes = remain;
        remain = 0;
    } else {
        remain -= (cdrom->track[start_track].length - start_frame);
        for (i = start_track + 1; i < cdrom->numtracks; i++) {
            if (remain < cdrom->track[i].length) {
                ntracks = i - start_track;
                nframes = remain;
                remain = 0;
                break;
            }
            remain -= cdrom->track[i].length;
        }
    }

    if (remain) {
        ntracks = (cdrom->numtracks - start_track) - 1;
        nframes = cdrom->track[cdrom->numtracks - 1].length;
    }

    return StartCDAudioPlaying(cdrom, start_track, start_frame, ntracks, nframes);
}

DECLSPEC12 int SDLCALL
SDL_CDPause(SDL12_CD *cdrom)
{
    if ((cdrom = ValidCDDevice(cdrom)) == NULL) {
        return -1;
    } else if (cdrom->status == SDL12_CD_TRAYEMPTY) {
        return SDL20_SetError("Tray empty");
    }

    SDL20_LockAudio();
    if (audio_cbdata) {
        if (audio_cbdata->cdrom_status == SDL12_CD_PLAYING) {
            audio_cbdata->cdrom_status = SDL12_CD_PAUSED;
        }
        cdrom->status = audio_cbdata->cdrom_status;
    }
    SDL20_UnlockAudio();
    return 0;
}

DECLSPEC12 int SDLCALL
SDL_CDResume(SDL12_CD *cdrom)
{
    if ((cdrom = ValidCDDevice(cdrom)) == NULL) {
        return -1;
    } else if (cdrom->status == SDL12_CD_TRAYEMPTY) {
        return SDL20_SetError("Tray empty");
    }

    SDL20_LockAudio();
    if (audio_cbdata) {
        if (audio_cbdata->cdrom_status == SDL12_CD_PAUSED) {
            audio_cbdata->cdrom_status = SDL12_CD_PLAYING;
        }
        cdrom->status = audio_cbdata->cdrom_status;
    }
    SDL20_UnlockAudio();
    return 0;
}


DECLSPEC12 int SDLCALL
SDL_CDStop(SDL12_CD *cdrom)
{
    SDL_RWops *oldrw = NULL;

    if ((cdrom = ValidCDDevice(cdrom)) == NULL) {
        return -1;
    }

    SDL20_LockAudio();
    if (audio_cbdata) {
        if ((audio_cbdata->cdrom_status == SDL12_CD_PLAYING) || (audio_cbdata->cdrom_status == SDL12_CD_PAUSED)) {
            audio_cbdata->cdrom_status = SDL12_CD_STOPPED;
            FreeMp3(&audio_cbdata->cdrom_mp3);
        }
        cdrom->status = audio_cbdata->cdrom_status;
    }
    SDL20_UnlockAudio();

    if (oldrw) {
        SDL20_RWclose(oldrw);
    }
    return 0;
}

DECLSPEC12 int SDLCALL
SDL_CDEject(SDL12_CD *cdrom)
{
    if ((cdrom = ValidCDDevice(cdrom)) == NULL) {
        return -1;
    }

    SDL20_LockAudio();
    if (audio_cbdata) {
        audio_cbdata->cdrom_status = SDL12_CD_TRAYEMPTY;
        FreeMp3(&audio_cbdata->cdrom_mp3);
    }
    cdrom->status = SDL12_CD_TRAYEMPTY;
    SDL20_UnlockAudio();
    return 0;
}

DECLSPEC12 void SDLCALL
SDL_CDClose(SDL12_CD *cdrom)
{
    if ((cdrom = ValidCDDevice(cdrom)) == NULL) {
        return;
    }

    SDL20_LockAudio();
    if (audio_cbdata) {
        audio_cbdata->cdrom_status = SDL12_CD_STOPPED;
        audio_cbdata->cdrom_opened = SDL_FALSE;
    }
    SDL20_UnlockAudio();

    if (audio_cbdata) {
        FreeMp3(&audio_cbdata->cdrom_mp3);
        SDL20_FreeAudioStream(audio_cbdata->cdrom_stream);
        audio_cbdata->cdrom_stream = NULL;
    }

    CloseSDL2AudioDevice();

    if (cdrom == CDRomDevice) {
        CDRomDevice = NULL;
    }
    SDL20_free(cdrom);
}


static void
FakeCdRomAudioCallback(AudioCallbackWrapperData *data, Uint8 *stream, int len, const SDL_bool must_mix)
{
    Uint32 total_available, available = 0;
    Uint32 channels, want_frames;

    if (data->cdrom_status != SDL12_CD_PLAYING) {
        if (!must_mix) {
            SDL20_memset(stream, data->device_format.silence, len);
        }
        return;
    }

    SDL_assert((data->cdrom_status == SDL12_CD_PLAYING) && (data->cdrom_mp3.pUserData != NULL));

    channels = data->cdrom_format.channels;
    want_frames = data->cdrom_format.samples / channels;

    while ((!data->cdrom_mp3.atEnd) && (SDL20_AudioStreamAvailable(data->cdrom_stream) < len)) {
        const Uint32 frames_read = (Uint32) drmp3_read_pcm_frames_f32(&data->cdrom_mp3, want_frames, (float *) data->mix_buffer);
        const Uint32 bytes_read = frames_read * channels * sizeof (float);
        SDL_assert(bytes_read <= data->cdrom_format.size);
        if ((!bytes_read) || (SDL20_AudioStreamPut(data->cdrom_stream, data->mix_buffer, bytes_read) == -1)) {  /* probably out of memory if failed */
            data->cdrom_mp3.atEnd = DRMP3_TRUE;  /* force this to fail from now on */
            SDL20_AudioStreamFlush(data->cdrom_stream);  /* make sure all we've put is available to get. */
            break;
        }
    }

    total_available = SDL20_AudioStreamAvailable(data->cdrom_stream);
    available = total_available;
    if (((Uint32) len) < available) {
        available = (Uint32) len;
    }

    if (available > 0) {
        if (!must_mix) {
            SDL20_AudioStreamGet(data->cdrom_stream, stream, available);
        } else {
            SDL20_AudioStreamGet(data->cdrom_stream, data->mix_buffer, available);
            SDL20_MixAudioFormat(stream, data->mix_buffer, audio_cbdata->device_format.format, available, SDL_MIX_MAXVOLUME);
        }

        data->cdrom_pcm_frames_written += (int) ((available / ((double) SDL_AUDIO_BITSIZE(data->device_format.format) / 8.0)) / data->device_format.channels);
        data->cdrom_cur_frame = (int) ((((double)data->cdrom_pcm_frames_written) / ((double)data->device_format.freq)) * CDAUDIO_FPS);
        if (data->cdrom_stop_ntracks == 0) {
            if (data->cdrom_cur_frame >= data->cdrom_stop_nframes) {
                data->cdrom_mp3.atEnd = DRMP3_TRUE;  /* played all that was requested! */
            }
        }
    }

    if ((total_available == 0) && (data->cdrom_mp3.atEnd)) {  /* mp3 is done for whatever reason */
        SDL_bool silence = ((!must_mix) && (available < ((Uint32) len))) ? SDL_TRUE : SDL_FALSE;  /* silence any section we couldn't provide */

        FreeMp3(&data->cdrom_mp3);

        if (data->cdrom_stop_ntracks > 0) {
            data->cdrom_stop_ntracks--;
            data->cdrom_pcm_frames_written = 0;
            data->cdrom_cur_frame = 0;

            if (data->cdrom_status == SDL12_CD_PLAYING) {  /* go on to next track? */
                const SDL_bool loaded = LoadCDTrack(++data->cdrom_cur_track, &data->cdrom_mp3);
                if (!loaded) {
                    data->cdrom_status = SDL12_CD_STOPPED;
                } else {  /* let new track fill out rest of callback. */
                    if (available < ((Uint32) len)) {
                        FakeCdRomAudioCallback(data, stream + available, len - available, must_mix);
                        silence = SDL_FALSE;
                    }
                }
            }
        } else {
            data->cdrom_status = SDL12_CD_STOPPED;  /* played all that was requested! */
        }

        if (silence) {
            SDL20_memset(stream + available, data->device_format.silence, len - available);
        }
    }
}


static void SDLCALL
AudioCallbackWrapper(void *userdata, Uint8 *stream, int len)
{
    AudioCallbackWrapperData *data = (AudioCallbackWrapperData *) userdata;
    SDL_bool must_mix = SDL_FALSE;

    if (data->app_callback_opened && !SDL20_AtomicGet(&audio_callback_paused)) {
        while (SDL20_AudioStreamAvailable(data->app_callback_stream) < len) {
            SDL20_memset(data->mix_buffer, data->app_callback_format.silence, data->app_callback_format.size);  /* SDL2 doesn't clear the stream before calling in here, but 1.2 expects it. */
            data->app_callback_format.callback(data->app_callback_format.userdata, data->mix_buffer, data->app_callback_format.size);
            if (SDL20_AudioStreamPut(data->app_callback_stream, data->mix_buffer, data->app_callback_format.size) == -1) {  /* probably out of memory if failed */
                break;  /* this will make the AudioStreamGet call fail. */
            }
        }
        if (SDL20_AudioStreamGet(data->app_callback_stream, stream, len) != len) {
            SDL20_memset(stream, data->device_format.silence, len);
        } else {
            must_mix = SDL_TRUE;
        }
    }

    FakeCdRomAudioCallback(data, stream, len, must_mix);
}


static SDL_bool
ResetAudioStream(SDL_AudioStream **_stream, SDL_AudioSpec *spec, const SDL_AudioSpec *to, const SDL_AudioFormat fromfmt, const Uint8 fromchannels, const int fromfreq)
{
    if ((!*_stream) || (spec->channels != fromchannels) || (spec->format != fromfmt) || (spec->freq != fromfreq)) {
        SDL20_FreeAudioStream(*_stream);
        *_stream = SDL20_NewAudioStream(fromfmt, fromchannels, fromfreq, to->format, to->channels, to->freq);
        if (!*_stream) {
            return SDL_FALSE;
        }

        spec->channels = fromchannels;
        spec->format = fromfmt;
        spec->freq = fromfreq;
        spec->size = spec->samples * spec->channels * (SDL_AUDIO_BITSIZE(spec->format) / 8);

        if (audio_cbdata->mixbuflen < spec->size) {
            void *ptr = SDL20_realloc(audio_cbdata->mix_buffer, spec->size);
            if (!ptr) {
                SDL20_FreeAudioStream(*_stream);
                *_stream = NULL;
                SDL20_OutOfMemory();
                return SDL_FALSE;
            }
            audio_cbdata->mixbuflen = spec->size;
            audio_cbdata->mix_buffer = (Uint8 *) ptr;
        }
    }
    return SDL_TRUE;
}

static SDL_bool
OpenSDL2AudioDevice(SDL_AudioSpec *appwant)
{
    SDL_AudioSpec devwant;

    /* note that 0x80 isn't perfect silence for U16 formats, but we only have one byte that is used for memset() calls, so it has to do. SDL2 has the same bug. */
    appwant->silence = SDL_AUDIO_ISSIGNED(appwant->format) ? 0x00 : 0x80;
    appwant->size = appwant->samples * appwant->channels * (SDL_AUDIO_BITSIZE(appwant->format) / 8);

    if (audio_cbdata != NULL) {  /* device is already open. */
        SDL20_LockAudio();  /* Device is already at acceptable parameters, just pause it for further setup by caller. */
        return SDL_TRUE;
    }

    audio_cbdata = (AudioCallbackWrapperData *) SDL20_calloc(1, sizeof (AudioCallbackWrapperData));
    if (!audio_cbdata) {
        SDL20_OutOfMemory();
        return SDL_FALSE;
    }

    /* Two things use the audio device: the app, through 1.2's SDL_OpenAudio,
       and the fake CD-ROM device. Either can open the device, and both write
       to SDL_AudioStreams to buffer and convert data. We open the device
       in a format that accommodates both inputs.

       In case the app asks for something really low-quality--an old game
       playing 8-bit mono audio at 8000Hz or whatever--we force the audio
       hardware to something better, in case the app _also_ wants to play
       CD audio, so we can get good quality out of that without reopening
       the device. We have to be able to buffer and convert between the
       app's needs and SDL2's anyhow, so this isn't a big deal. If they
       ask for better than CD quality, we'll allow it and upsample any
       CD audio that is played.

       If the CD-ROM is opened first, and the app wants better-than-CD
       quality later, there's not much we can do, it'll have to
       downsample, but I suspect this is rare, and the audio will
       still be good enough. */

    SDL20_memcpy(&devwant, appwant, sizeof (SDL_AudioSpec));
    devwant.callback = AudioCallbackWrapper;
    devwant.userdata = audio_cbdata;
    devwant.freq = SDL_max(devwant.freq, 44100);
    devwant.channels = SDL_max(devwant.channels, 2);
    if (SDL_AUDIO_BITSIZE(devwant.format) < 16) {
        devwant.format = AUDIO_S16SYS;
    }

    if (SDL20_OpenAudio(&devwant, &audio_cbdata->device_format) == -1) {
        SDL_free(audio_cbdata);
        audio_cbdata = NULL;
        return SDL_FALSE;
    }

    SDL20_LockAudio();
    SDL20_PauseAudio(0);  /* always unpause, but caller will unlock after finalizing setup. */

    return SDL_TRUE;
}

static int
CloseSDL2AudioDevice(void)
{
    SDL_bool close_sdl2_device;

    SDL20_LockAudio();
    close_sdl2_device = (audio_cbdata && !audio_cbdata->app_callback_opened && !audio_cbdata->cdrom_opened) ? SDL_TRUE : SDL_FALSE;
    SDL20_UnlockAudio();

    if (close_sdl2_device) {
        SDL20_CloseAudio();
        SDL20_FreeAudioStream(audio_cbdata->app_callback_stream);
        SDL20_FreeAudioStream(audio_cbdata->cdrom_stream);
        SDL20_free(audio_cbdata->mix_buffer);
        SDL20_free(audio_cbdata);
        audio_cbdata = NULL;
    }

    return -1;
}


DECLSPEC12 int SDLCALL
SDL_OpenAudio(SDL_AudioSpec *want, SDL_AudioSpec *obtained)
{
    SDL_bool already_opened;

    /* SDL_OpenAudio() will init the subsystem for you if necessary, yuck. */
    if ((InitializedSubsystems20 & SDL_INIT_AUDIO) != SDL_INIT_AUDIO) {
        if (SDL_InitSubSystem(SDL12_INIT_AUDIO) < 0) {
            return -1;
        }
    }

    /* SDL2 uses a NULL callback to mean "we plan to use SDL_QueueAudio()" */
    if (want && (want->callback == NULL)) {
        return SDL20_SetError("Callback can't be NULL");
    }

    SDL20_LockAudio();
    already_opened = (audio_cbdata && audio_cbdata->app_callback_opened) ? SDL_TRUE : SDL_FALSE;
    SDL20_UnlockAudio();
    if (already_opened) {
        return SDL20_SetError("Audio device already opened");
    }

    if (!want->format) {
        const char *env = SDL20_getenv("SDL_AUDIO_FORMAT");  /* SDL 1.2 checks this. */
        if (env != NULL) {
            if      (SDL20_strcmp(env, "U8") == 0) { want->format = AUDIO_U8; }
            else if (SDL20_strcmp(env, "S8") == 0) { want->format = AUDIO_S8; }
            else if (SDL20_strcmp(env, "U16") == 0) { want->format = AUDIO_U16SYS; }
            else if (SDL20_strcmp(env, "S16") == 0) { want->format = AUDIO_S16SYS; }
            else if (SDL20_strcmp(env, "U16LSB") == 0) { want->format = AUDIO_U16LSB; }
            else if (SDL20_strcmp(env, "S16LSB") == 0) { want->format = AUDIO_S16LSB; }
            else if (SDL20_strcmp(env, "U16MSB") == 0) { want->format = AUDIO_U16MSB; }
            else if (SDL20_strcmp(env, "S16MSB") == 0) { want->format = AUDIO_S16MSB; }
            else if (SDL20_strcmp(env, "U16SYS") == 0) { want->format = AUDIO_U16SYS; }
            else if (SDL20_strcmp(env, "S16SYS") == 0) { want->format = AUDIO_S16SYS; }
        }
        if (!want->format) {
            want->format = AUDIO_S16SYS;
        }
    }

    if (!want->freq) {
        const char *env = SDL20_getenv("SDL_AUDIO_FREQUENCY");  /* SDL 1.2 checks this. */
        if (env != NULL) {
            want->freq = SDL20_atoi(env);
        }
        if (!want->freq) {
            want->freq = 22050;
        }
        want->samples = 0;
    }

    if (!want->channels) {
        const char *env = SDL20_getenv("SDL_AUDIO_CHANNELS");  /* SDL 1.2 checks this. */
        if (env != NULL) {
            want->channels = SDL20_atoi(env);
        }
        if (!want->channels) {
            want->channels = 2;
        }
    }

    if (!want->samples) {
        const char *env = SDL20_getenv("SDL_AUDIO_SAMPLES");  /* SDL 1.2 checks this. */
        if (env != NULL) {
            want->samples = SDL20_atoi(env);
        }
        if (!want->samples) {
            const Uint32 samp = (want->freq / 1000) * 46;  /* ~46 ms */
            Uint32 pow2 = 1;
            while (pow2 < samp) {
                pow2 <<= 1;
            }
            want->samples = pow2;
        }
    }

    /* the app always passes callback data through an SDL_AudioStream, since it
       has to share with the fake CD-ROM support. This also avoids the risk of
       getting an incompatible device configuration from SDL2. As such,
       the app always gets the format it requests. */
    if (!OpenSDL2AudioDevice(want)) {
        return -1;
    }

    /* Device is locked now, unconditionally. Set up some things. */

    if (obtained) {  /* the app always gets the format it requests */
        SDL20_memcpy(obtained, want, sizeof (SDL_AudioSpec));
    }

    SDL20_memcpy(&audio_cbdata->app_callback_format, want, sizeof (SDL_AudioSpec));
    SDL20_AtomicSet(&audio_callback_paused, SDL_TRUE);  /* app callback always starts paused after open. */

    SDL_assert(audio_cbdata->app_callback_stream == NULL);
    if (!ResetAudioStream(&audio_cbdata->app_callback_stream, &audio_cbdata->app_callback_format, &audio_cbdata->device_format, want->format, want->channels, want->freq)) {
        SDL20_UnlockAudio();  /* make sure CD audio doesn't hang if it's playing. */
        return CloseSDL2AudioDevice();  /* will stay open if CD audio is still playing, cleans up otherwise. */
    }

    audio_cbdata->app_callback_opened = SDL_TRUE;

    SDL20_UnlockAudio();  /* we're off and going. */

    return 0;
}

DECLSPEC12 void SDLCALL
SDL_PauseAudio(int pause_on)
{
    SDL20_AtomicSet(&audio_callback_paused, pause_on ? SDL_TRUE : SDL_FALSE);
}

DECLSPEC12 SDL_AudioStatus SDLCALL
SDL_GetAudioStatus(void)
{
    SDL_AudioStatus retval = SDL_AUDIO_STOPPED;
    SDL20_LockAudio();
    if (audio_cbdata && audio_cbdata->app_callback_opened) {
        retval = SDL20_AtomicGet(&audio_callback_paused) ? SDL_AUDIO_PAUSED : SDL_AUDIO_PLAYING;
    }
    SDL20_UnlockAudio();
    return retval;
}

DECLSPEC12 void SDLCALL
SDL_MixAudio(Uint8 *dst, const Uint8 *src, Uint32 len, int volume)
{
    SDL_AudioFormat fmt;

    if (volume == 0) {
        return;  /* nothing to do. */
    }

    /* in 1.2, if not the subsystem isn't initialized _at all_, it forces
       format to AUDIO_S16. If it's initialized but the device isn't opened,
       you get a format of zero and it returns an error without mixing
       anything. */
    SDL20_LockAudio();
    if ((InitializedSubsystems20 & SDL_INIT_AUDIO) != SDL_INIT_AUDIO) {
        fmt = AUDIO_S16;  /* to quote 1.2: "HACK HACK HACK" */
    } else if (!audio_cbdata || !audio_cbdata->app_callback_opened) {
        fmt = 0;  /* this will fail, but drop the lock first. */
    } else {
        fmt = audio_cbdata->app_callback_format.format;
    }
    SDL20_UnlockAudio();

    if (fmt == 0) {
        SDL_SetError("SDL_MixAudio(): unknown audio format");  /* this is the exact error 1.2 reports for this. */
    } else {
        SDL20_MixAudioFormat(dst, src, fmt, len, volume);
    }
}


DECLSPEC12 void SDLCALL
SDL_CloseAudio(void)
{
    SDL20_LockAudio();
    if (audio_cbdata) {
        audio_cbdata->app_callback_opened = SDL_FALSE;
        SDL20_FreeAudioStream(audio_cbdata->app_callback_stream);
        audio_cbdata->app_callback_stream = NULL;
    }
    SDL20_UnlockAudio();

    CloseSDL2AudioDevice();
}

static SDL_AudioCVT *
AudioCVT12to20(const SDL12_AudioCVT *cvt12, SDL_AudioCVT *cvt20)
{
    SDL_zerop(cvt20);
    cvt20->needed = cvt12->needed;
    cvt20->src_format = cvt12->src_format;
    cvt20->dst_format = cvt12->dst_format;
    cvt20->rate_incr = cvt12->rate_incr;
    cvt20->buf = cvt12->buf;
    cvt20->len = cvt12->len;
    cvt20->len_cvt = cvt12->len_cvt;
    cvt20->len_mult = cvt12->len_mult;
    cvt20->len_ratio = cvt12->len_ratio;
    SDL20_memcpy(cvt20->filters, cvt12->filters, sizeof (cvt12->filters));
    cvt20->filter_index = cvt12->filter_index;
    return cvt20;
}

static SDL12_AudioCVT *
AudioCVT20to12(const SDL_AudioCVT *cvt20, SDL12_AudioCVT *cvt12)
{
    SDL_zerop(cvt12);
    cvt12->needed = cvt20->needed;
    cvt12->src_format = cvt20->src_format;
    cvt12->dst_format = cvt20->dst_format;
    cvt12->rate_incr = cvt20->rate_incr;
    cvt12->buf = cvt20->buf;
    cvt12->len = cvt20->len;
    cvt12->len_cvt = cvt20->len_cvt;
    cvt12->len_mult = cvt20->len_mult;
    cvt12->len_ratio = cvt20->len_ratio;
    SDL20_memcpy(cvt12->filters, cvt20->filters, sizeof (cvt20->filters));
    cvt12->filter_index = cvt20->filter_index;
    return cvt12;
}

static void SDLCALL
CompatibilityCVT_RunStream(SDL12_AudioCVT *cvt12, Uint16 format)
{
    const size_t channel_mash = (size_t) cvt12->filters[SDL_arraysize(cvt12->filters) - 1];
    const Uint8 src_channels = (Uint8) (channel_mash & 0xFF);
    const Uint8 dst_channels = (Uint8) ((channel_mash >> 8) & 0xFF);

    /* use an audiostream, so we can allocate a dynamic buffer for the work, even if the app screwed up their allocation. */
    SDL_AudioStream *stream = SDL20_NewAudioStream(format, src_channels, 44100, cvt12->dst_format, dst_channels, 44100);  /* don't resample here! */
    if (stream == NULL) {
        return;  /* oh well. */
    }

    if ((SDL20_AudioStreamPut(stream, cvt12->buf, cvt12->len_cvt) == -1) || (SDL20_AudioStreamFlush(stream) == -1)) {  /* probably out of memory if failed. */
        SDL20_FreeAudioStream(stream);
        return;  /* oh well. */
    }

    cvt12->len_cvt = SDL20_AudioStreamAvailable(stream);
    SDL20_AudioStreamGet(stream, cvt12->buf, cvt12->len_cvt);
    SDL20_FreeAudioStream(stream);

    if (cvt12->filters[++cvt12->filter_index]) {
        cvt12->filters[cvt12->filter_index](cvt12, cvt12->dst_format);
    }
}

/* this is an extremely low-quality resampler: it only doubles or halves the
   sample rate and offers no interpolation...but it's also how SDL 1.2 did it.
   Notably: it can resample in-place, so this avoids bugs in apps that don't
   set up SDL_AudioCVT's buffer correctly, or expect this weird 1.2 behavior
   for whatever reason. */
static void SDLCALL
CompatibilityCVT_Resampler(SDL12_AudioCVT *cvt12, Uint16 format)
{
    const int bitsize = (int) SDL_AUDIO_BITSIZE(format);
    int i;

    SDL_assert((bitsize == 8) || (bitsize == 16));  /* there were no 32-bit audio types in 1.2. */

    if (cvt12->rate_incr < 1.0) {   /* upsampling */
        /*printf("2x Upsampling!\n");*/
        #define DO_RESAMPLE(typ) \
            const typ *src = (const typ *) (cvt12->buf + cvt12->len_cvt); \
            typ *dst = (typ *) (cvt12->buf + (cvt12->len_cvt * 2)); \
            for (i = cvt12->len_cvt / sizeof (typ); i; i--) { \
                const typ sample = *(--src); \
                dst -= 2; \
                dst[0] = dst[1] = sample; \
            }
        if (bitsize == 8) {
            DO_RESAMPLE(Uint8);
        } else if (bitsize == 16) {
            DO_RESAMPLE(Uint16);
        }
        #undef DO_RESAMPLE
        cvt12->len_cvt *= 2;
    } else {  /* downsampling. */
        /*printf("2x Downsampling!\n");*/
        #define DO_RESAMPLE(typ) \
            const typ *src = (const typ *) cvt12->buf; \
            typ *dst = (typ *) cvt12->buf; \
            for (i = cvt12->len_cvt / (sizeof (typ) * 2); i; i--, src += 2) { \
                *(dst++) = *src; \
            }
        if (bitsize == 8) {
            DO_RESAMPLE(Uint8);
        } else if (bitsize == 16) {
            DO_RESAMPLE(Uint16);
        }
        #undef DO_RESAMPLE
        cvt12->len_cvt /= 2;
    }

    if (cvt12->filters[++cvt12->filter_index]) {
        cvt12->filters[cvt12->filter_index](cvt12, format);
    }
}

DECLSPEC12 int SDLCALL
SDL_BuildAudioCVT(SDL12_AudioCVT *cvt12, Uint16 src_format, Uint8 src_channels, int src_rate, Uint16 dst_format, Uint8 dst_channels, int dst_rate)
{
    int retval = 0;

    SDL_zerop(cvt12); /* SDL 1.2 derefences cvt12 without checking for NULL */

    if (!WantCompatibilityAudioCVT) {
        SDL_AudioCVT cvt20;
        retval = SDL20_BuildAudioCVT(&cvt20, src_format, src_channels, src_rate, dst_format, dst_channels, dst_rate);
        AudioCVT20to12(&cvt20, cvt12);
    } else {
        const size_t channel_mash = ((size_t) src_channels) | (((size_t) dst_channels) << 8);

        if ((src_format == dst_format) && (src_channels == dst_channels) && (src_rate == dst_rate)) {
            return 0;  /* no conversion needed. */
        }

        cvt12->needed = 1;
        cvt12->len_mult = 1;
        cvt12->len_ratio = 1.0;
        cvt12->src_format = src_format;
        cvt12->dst_format = dst_format;

        if ((src_format != dst_format) || (src_channels != dst_channels)) {
            if (src_format != dst_format) {
                /* there are only 8 and 16 bit formats in SDL 1.2 */
                if (SDL_AUDIO_BITSIZE(src_format) < SDL_AUDIO_BITSIZE(dst_format)) {
                    cvt12->len_mult *= 2;
                    cvt12->len_ratio *= 2.0;
                } else {
                    cvt12->len_ratio /= 2.0;
                }
            }

            /* SDL 1.2 only supported 1, 2, 4, and 6 channels, and would fail to convert between 4 and 6 at all. :O */
            if (src_channels < dst_channels) {
                const int diff = (int) (dst_channels / src_channels);
                cvt12->len_mult *= diff;
                cvt12->len_ratio *= (double) diff;
            } else if (src_channels > dst_channels) {
                const int diff = (int) (src_channels / dst_channels);
                cvt12->len_ratio /= (double) diff;
            }

            cvt12->filters[cvt12->filter_index++] = CompatibilityCVT_RunStream;
            cvt12->filters[SDL_arraysize(cvt12->filters) - 1] = (SDL12_AudioCVTFilter) channel_mash;  /* cheat by hiding this info at end of array. */
        }

        if (src_rate != dst_rate) {
            Uint32 hi_rate = src_rate;
            Uint32 lo_rate = dst_rate;
            int len_mult = 1;
            double len_ratio = 0.5;

            if (src_rate < dst_rate) {  /* flip everything. */
                hi_rate = dst_rate;
                lo_rate = src_rate;
                len_mult = 2;
                len_ratio = 2.0;
            }

            while (((lo_rate * 2) / 100) <= (hi_rate / 100)) {   /* this is what SDL 1.2 does. *shrug* */
                if (cvt12->filter_index >= (SDL_arraysize(cvt12->filters) - 2)) {
                    return SDL20_SetError("Too many conversion filters needed");
                }
                cvt12->filters[cvt12->filter_index++] = CompatibilityCVT_Resampler;
                cvt12->len_mult *= len_mult;
                lo_rate *= 2;
                cvt12->len_ratio *= len_ratio;
            }

            cvt12->rate_incr = ((double) src_rate) / ((double) dst_rate);
        }

        retval = 1;  /* conversion definitely needed. */
    }

    return retval;
}

DECLSPEC12 int SDLCALL
SDL_ConvertAudio(SDL12_AudioCVT *cvt12)
{
    int retval = 0;

    if (!cvt12->buf) {  /* neither SDL 1.2 nor 2.0 makes sure cvt12 isn't NULL here.  :/  */
        retval = SDL20_SetError("No buffer allocated for conversion");
    } else if (!WantCompatibilityAudioCVT) {
        SDL_AudioCVT cvt20;
        retval = SDL20_ConvertAudio(AudioCVT12to20(cvt12, &cvt20));
        AudioCVT20to12(&cvt20, cvt12);
    } else {
        cvt12->len_cvt = cvt12->len;
        cvt12->filter_index = 0;
        if (cvt12->filters[0]) {
            cvt12->filters[0](cvt12, cvt12->src_format);
        }
    }

    return retval;
}


/* SDL_GL_DisableContext and SDL_GL_EnableContext_Thread are not real SDL 1.2
   APIs, but some idTech4 games shipped with a custom SDL 1.2 build that added
   these functions, to let them make a GL context current on a background thread,
   so we supply them as well to be binary compatible for those games. */

DECLSPEC12 void SDLCALL
SDL_GL_DisableContext(void)
{
    SDL20_GL_MakeCurrent(NULL, NULL);
}

DECLSPEC12 void SDLCALL
SDL_GL_EnableContext_Thread(void)
{
    const SDL_bool enable = (VideoGLContext20 && VideoWindow20) ? SDL_TRUE : SDL_FALSE;
    SDL20_GL_MakeCurrent(enable ? VideoWindow20 : NULL, enable ? VideoGLContext20 : NULL);
}


/* X11_KeyToUnicode is an internal function in the SDL 1.2 x11 backend that some Linux
   software (older versions of the Torque Engine, for example) would call directly, so
   we're supplying an extremely naive implementation here. Apps using this should be
   fixed if possible, and this implementation is generally incorrect but hopefully
   enough to get apps to limp along.
   As this isn't X11-specific, we supply it globally, so x11 binaries can transition
   to Wayland, and if there's some wildly-misbuilt win32 software, they can call it
   too. :) */
#if !(defined(_WIN32) || defined(__OS2__)) /* #if defined(__unix__) || defined(__APPLE__) ?? */
DECLSPEC12 Uint16 SDLCALL
X11_KeyToUnicode(SDL12Key key, SDL12Mod mod)
{
    if (((int) key) >= 127) {
        return 0;
    }
    if ((key >= SDLK12_a) && (key <= SDLK12_z)) {
        const int shifted = ((mod & (KMOD12_LSHIFT|KMOD12_RSHIFT)) != 0) ? 1 : 0;
        int capital = ((mod & KMOD12_CAPS) != 0) ? 1 : 0;
        if (shifted) {
            capital = !capital;
        }
        return (Uint16) ((capital ? 'A' : 'a') + (key - SDLK12_a));
    }

    return (Uint16) key;
}
#endif

#ifdef __cplusplus
}
#endif

/* vi: set ts=4 sw=4 expandtab: */
