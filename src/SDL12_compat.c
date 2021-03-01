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
#include <limits.h>
#ifndef _WIN32
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
            SDL20_Log("FIXME: %s (%s:%d)\n", x, __FUNCTION__, __LINE__); \
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

#define SDL12_LOGPAL 1
#define SDL12_PHYSPAL 2

// this will be in SDL 2.0.10, but here it is in case your headers are missing it.
#ifndef SDL_SIMD_ALIGNED
#define SDL_SIMD_ALIGNED    0x00000008
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
    SDL12Mod mod;
    Uint16 unicode;
} SDL12_keysym;

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
    int nummodes;
    SDL12_Rect *modeslist12;
    SDL12_Rect **modes12;  /* ptrs to each item in modeslist, for SDL_ListModes() */
} VideoModeList;

typedef struct
{
    int device_index;
    SDL_Joystick *joystick;
} JoystickOpenedItem;

// !!! FIXME: go through all of these.
static VideoModeList *VideoModes = NULL;
static int VideoModesCount = 0;  // this counts items in VideoModeList, not total video modes.
static SDL12_VideoInfo VideoInfo12;
static SDL12_Palette VideoInfoPalette12;
static SDL12_PixelFormat VideoInfoVfmt12;
static SDL_PixelFormat *VideoInfoVfmt20 = NULL;
static SDL_bool VideoWindowGrabbed = SDL_FALSE;
static SDL_bool VideoCursorHidden = SDL_FALSE;
static SDL_Window *VideoWindow20 = NULL;
static SDL_Renderer *VideoRenderer20 = NULL;
static SDL_Texture *VideoTexture20 = NULL;
static SDL12_Surface *VideoSurface12 = NULL;
static Uint32 VideoSurfacePresentTicks = 0;
static Uint32 VideoSurfaceLastPresentTicks = 0;
static SDL_Surface *VideoConvertSurface20 = NULL;
static SDL_GLContext VideoGLContext20 = NULL;
static char *WindowTitle = NULL;
static char *WindowIconTitle = NULL;
static SDL_Surface *VideoIcon20 = NULL;
static int EnabledUnicode = 0;
static int VideoDisplayIndex = 0;
static int CDRomInit = 0;
static SDL12_EventFilter EventFilter12 = NULL;
static SDL12_Cursor *CurrentCursor12 = NULL;
static Uint8 EventStates[SDL12_NUMEVENTS];
static int SwapInterval = 0;
static JoystickOpenedItem JoystickOpenList[16];
static Uint8 KeyState[SDLK12_LAST];
static SDL_bool MouseInputIsRelative = SDL_FALSE;
static SDL_Point MousePositionWhenRelative = { 0, 0 };

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
static char loaderror[256];
#if defined(_WIN32)
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN 1
    #endif
    #include <windows.h>
    #define SDL20_LIBNAME "SDL2.dll"
    /* require SDL2 >= 2.0.12 for SDL_CreateThread binary compatibility */
    #define SDL20_REQUIRED_VER SDL_VERSIONNUM(2,0,12)
    static HANDLE Loaded_SDL20 = NULL;
    #define LoadSDL20Library() ((Loaded_SDL20 = LoadLibraryA(SDL20_LIBNAME)) != NULL)
    #define LookupSDL20Sym(sym) GetProcAddress(Loaded_SDL20, sym)
    #define CloseSDL20Library() { if (Loaded_SDL20) { FreeLibrary(Loaded_SDL20); Loaded_SDL20 = NULL; } }
    #define strcpy_fn  lstrcpyA
    #define sprintf_fn wsprintfA
#elif defined(__unix__) || defined(__APPLE__)
    #include <dlfcn.h>
    #ifdef __APPLE__
    #define SDL20_LIBNAME "libSDL2-2.0.0.dylib"
    #else
    #define SDL20_LIBNAME "libSDL2-2.0.so.0"
    #endif
    #define SDL20_REQUIRED_VER SDL_VERSIONNUM(2,0,9)
    static void *Loaded_SDL20 = NULL;
    #define LoadSDL20Library() ((Loaded_SDL20 = dlopen(SDL20_LIBNAME, RTLD_LOCAL|RTLD_NOW)) != NULL)
    #define LookupSDL20Sym(sym) dlsym(Loaded_SDL20, sym)
    #define CloseSDL20Library() { if (Loaded_SDL20) { dlclose(Loaded_SDL20); Loaded_SDL20 = NULL; } }
    #define strcpy_fn  strcpy
    #define sprintf_fn sprintf
#elif defined(__OS2__)
    #include <os2.h>
    #define SDL20_LIBNAME "SDL2.dll"
    #define SDL20_REQUIRED_VER SDL_VERSIONNUM(2,0,9)
    #define strcpy_fn  strcpy
    #define sprintf_fn sprintf
    static HMODULE Loaded_SDL20 = NULLHANDLE;
    static SDL_bool LoadSDL20Library(void) {
        char err[256];
        if (DosLoadModule(err, sizeof(err), SDL20_LIBNAME, &Loaded_SDL20) != 0) {
            return SDL_FALSE;
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
#else
    #error Please define your platform.
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

static int
LoadSDL20(void)
{
    int okay = 1;
    if (!Loaded_SDL20) {
        okay = LoadSDL20Library();
        if (!okay) {
            strcpy_fn(loaderror, "Failed loading SDL2 library.");
        } else {
            #define SDL20_SYM(rc,fn,params,args,ret) SDL20_##fn = (SDL20_##fn##_t) LoadSDL20Symbol("SDL_" #fn, &okay);
            #include "SDL20_syms.h"
            if (okay) {
                SDL_version v;
                SDL20_GetVersion(&v);
                okay = (SDL_VERSIONNUM(v.major,v.minor,v.patch) >= SDL20_REQUIRED_VER);
                if (!okay) {
                    sprintf_fn(loaderror, "SDL2 %d.%d.%d library is too old.", v.major, v.minor, v.patch);
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

#elif defined(_WIN32)
#if defined(_MSC_VER) && !defined(__FLTUSED__)
#define __FLTUSED__
__declspec(selectany) int _fltused = 1;
#endif
#if defined(__MINGW32__)
#define _DllMainCRTStartup DllMainCRTStartup
#endif
BOOL WINAPI _DllMainCRTStartup(HANDLE dllhandle, DWORD reason, LPVOID reserved)
{
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
    #error Please define your platform
#endif

#ifdef _WIN32
DECLSPEC void SDLCALL
SDL_SetModuleHandle(void *handle)
{
    /* handled internally by SDL2 - nothing to do.. */
}

DECLSPEC int SDLCALL
SDL_RegisterApp(char *name, Uint32 style, void *hInst)
{
    return 0;
}

DECLSPEC void SDLCALL
SDL_UnregisterApp(void)
{
}
#endif

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

DECLSPEC void * SDLCALL
SDL_revcpy(void *dst, const void *src, size_t len)
{
    /* this doesn't reverse the data...I think this was just a memcpy that
       was meant to be CPU-cache friendly if you knew you were working with
       data going backwards in memory, instead of jumping over pages to copy
       from the start...? Whatever, just do a memcpy here. */
    return SDL_memcpy(dst, src, len);
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

DECLSPEC SDL_bool SDLCALL
SDL_HasMMXExt(void)
{
    return (get_cpu_ext_features() & 0x00400000)? SDL_TRUE : SDL_FALSE;
}

DECLSPEC SDL_bool SDLCALL
SDL_Has3DNowExt(void)
{
    return (get_cpu_ext_features() & 0x40000000)? SDL_TRUE : SDL_FALSE;
}

DECLSPEC SDL_Joystick * SDLCALL
SDL_JoystickOpen(int device_index)
{
    size_t i;
    SDL20_LockJoysticks();
    for (i = 0; i < SDL_arraysize(JoystickOpenList); i++) {
        if (JoystickOpenList[i].joystick == NULL) {
            break;
        }
    }

    if (i == SDL_arraysize(JoystickOpenList)) {
        SDL20_UnlockJoysticks();
        SDL20_SetError("Too many open joysticks");
        return NULL;
    }

    JoystickOpenList[i].joystick = SDL20_JoystickOpen(device_index);
    if (JoystickOpenList[i].joystick) {
        JoystickOpenList[i].device_index = device_index;
    }

    SDL20_UnlockJoysticks();
    return JoystickOpenList[i].joystick;
}

DECLSPEC void SDLCALL
SDL_JoystickClose(SDL_Joystick *joystick)
{
    size_t i;
    SDL20_LockJoysticks();
    for (i = 0; i < SDL_arraysize(JoystickOpenList); i++) {
        if (JoystickOpenList[i].joystick == joystick) {
            break;
        }
    }

    if (i < SDL_arraysize(JoystickOpenList)) {
        JoystickOpenList[i].joystick = NULL;
    }

    SDL20_UnlockJoysticks();

    SDL20_JoystickClose(joystick);
}

DECLSPEC const char * SDLCALL
SDL_JoystickName(int device_index)
{
    return SDL20_JoystickNameForIndex(device_index);
}

DECLSPEC int SDLCALL
SDL_JoystickIndex(SDL_Joystick *joystick)
{
    size_t i;
    SDL20_LockJoysticks();
    for (i = 0; i < SDL_arraysize(JoystickOpenList); i++) {
        if (JoystickOpenList[i].joystick == joystick) {
            break;
        }
    }

    if (i < SDL_arraysize(JoystickOpenList)) {
        SDL20_UnlockJoysticks();
        return JoystickOpenList[i].device_index;
    }

    SDL20_UnlockJoysticks();
    return SDL20_SetError("Can't find joystick");
}

DECLSPEC int SDLCALL
SDL_JoystickOpened(int device_index)
{
    int retval = 0;
    size_t i;
    SDL20_LockJoysticks();
    for (i = 0; i < SDL_arraysize(JoystickOpenList); i++) {
        if ((JoystickOpenList[i].joystick) && (JoystickOpenList[i].device_index == device_index)) {
            retval = 1;
            break;
        }
    }
    SDL20_UnlockJoysticks();
    return retval;
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
    format12->colorkey = 0;  // this is a surface, not pixelformat, properties in SDL2.
    format12->alpha = 255;  // this is a surface, not pixelformat, properties in SDL2.
    return format12;
}

static int
GetVideoDisplay(void)
{
    const char *variable;
    FIXME("cache this value during SDL_Init() so it doesn't change.");
    variable = SDL20_getenv("SDL_VIDEO_FULLSCREEN_DISPLAY");
    if (!variable) {
        variable = SDL20_getenv("SDL_VIDEO_FULLSCREEN_HEAD");
    }
    if (variable) {
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
    void *ptr = NULL;
    int i, j;

    if (VideoModesCount > 0) {
        return 0;  // already did this.
    }

    SDL_assert(VideoModes == NULL);

    for (i = 0; i < total; ++i) {
        SDL_DisplayMode mode;

        if (SDL20_GetDisplayMode(VideoDisplayIndex, i, &mode) < 0) {
            continue;
        } else if (!mode.w || !mode.h) {
            SDL_assert(0 && "Can this actually happen?");
            continue;
        }

        if (!vmode || (mode.format != vmode->format)) {  // SDL20_GetDisplayMode() sorts on bpp first. We know when to change arrays.
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

        /* make sure we don't have this one already (with a different refresh rate, etc). */
        for (j = 0; j < vmode->nummodes; j++) {
            if ((vmode->modeslist12[j].w == mode.w) && (vmode->modeslist12[j].h == mode.h)) {
                break;
            }
        }

        if (j < vmode->nummodes) {
            continue;  /* already have this one. */
        }

        FIXME("Make sure mode dimensions fit in 16-bits for SDL12_Rect");

        ptr = SDL20_realloc(vmode->modeslist12, sizeof (SDL12_Rect) * (vmode->nummodes + 1));
        if (ptr == NULL) {
            return SDL20_OutOfMemory();
        }
        vmode->modeslist12 = (SDL12_Rect *) ptr;

        vmode->modeslist12[vmode->nummodes].x = 0;
        vmode->modeslist12[vmode->nummodes].y = 0;
        vmode->modeslist12[vmode->nummodes].w = mode.w;
        vmode->modeslist12[vmode->nummodes].h = mode.h;

        vmode->nummodes++;
    }

    // link up modes12 for SDL_ListModes()'s use...
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

    return 0;
}

static int
Init12Video(void)
{
    SDL_DisplayMode mode;
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
        VideoInfo12.wm_available = 1;  FIXME("not sure if we can determine this in SDL2...?");
        VideoInfo12.video_mem = 1024 * 256;  /* good enough. */
    }

    return 0;
}

DECLSPEC int SDLCALL
SDL_InitSubSystem(Uint32 sdl12flags)
{
    Uint32 sdl20flags = 0;
    int rc;

    FIXME("there is never a parachute in SDL2, should we catch segfaults ourselves?");

    FIXME("support event thread where it makes sense to do so?");

    if ((sdl12flags & SDL12_INIT_EVENTTHREAD) == SDL12_INIT_EVENTTHREAD) {
        return SDL20_SetError("OS doesn't support threaded events");
    }

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
        if (Init12Video() < 0) {
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

static SDL12_Surface *EndVidModeCreate(void);
static void
Quit12Video(void)
{
    int i;

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

    if (sdl12flags & SDL12_INIT_VIDEO) {
        Quit12Video();
    }

    FIXME("do something about SDL12_INIT_EVENTTHREAD");
    SDL20_QuitSubSystem(sdl20flags);

    if ((SDL20_WasInit(0) == 0) && (!CDRomInit)) {
        SDL20_Quit();
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
    else {
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

    SDL_PumpEvents();  /* this will run our filter and build our 1.2 queue. */

    if (EventQueueHead == NULL) {
        return 0;  /* no events at the moment. */
    }

    SDL_memcpy(event12, &EventQueueHead->event12, sizeof (SDL12_Event));
    next = EventQueueHead->next;
    EventQueueHead->next = EventQueueAvailable;
    EventQueueAvailable = EventQueueHead;
    EventQueueHead = next;
    if (!next) {
        EventQueueTail = NULL;
    }

    return 1;
}

DECLSPEC int SDLCALL
SDL_PushEvent(SDL12_Event *event12)
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
            if (SDL_PushEvent(&events12[i]) < 0)
                break;  /* out of space for more events. */
        }
        return i;
    }
    else if ((action == SDL_PEEKEVENT) || (action == SDL_GETEVENT))
    {
        const SDL_bool isGet = (action == SDL_GETEVENT)? SDL_TRUE : SDL_FALSE;
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
                return (SDL_PushEvent(event12) == 0)? SDL_TRUE : SDL_FALSE;
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

DECLSPEC Uint8 SDLCALL
SDL_GetMouseState(int *x, int *y)
{
    const Uint8 buttons = MouseButtonState20to12(SDL20_GetMouseState(x, y));
    if (MouseInputIsRelative) {
        if (x) { *x = MousePositionWhenRelative.x; }
        if (y) { *y = MousePositionWhenRelative.y; }
    }
    return buttons;
}

DECLSPEC Uint8 SDLCALL
SDL_GetRelativeMouseState(int *x, int *y)
{
    return MouseButtonState20to12(SDL20_GetRelativeMouseState(x, y));
}

DECLSPEC char * SDLCALL
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
	    CASESDLK12TONAME(SDLK12_LSUPER, "left super");	/* "Windows" keys */
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
    if (((int) keysym20) < 127) {  /* (most of) low-ASCII maps directly */
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
        CASEKEYSYM20TO12(SYSREQ);;
        CASEKEYSYM20TO12(MENU);
        CASEKEYSYM20TO12(POWER);
        CASEKEYSYM20TO12(UNDO);
        #undef CASEKEYSYM20TO12
        default: break;
    }

    FIXME("nothing maps to SDLK12_COMPOSE, SDLK12_BREAK, or SDLK12_EURO ...?");
    FIXME("map some of the SDLK12_WORLD keys");
    return SDLK12_UNKNOWN;
}

DECLSPEC Uint8 * SDLCALL
SDL_GetKeyState(int *numkeys)
{
    if (numkeys) {
        *numkeys = (int) SDL_arraysize(KeyState);
    }
    return KeyState;
}


static int SDLCALL
EventFilter20to12(void *data, SDL_Event *event20)
{
    //const int maxUserEvents12 = SDL12_NUMEVENTS - SDL12_USEREVENT;
    SDL12_Event event12;

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
        case SDL_SYSWMEVENT: FIXME("write me"); return 1;

        case SDL_KEYUP:
        case SDL_KEYDOWN:
            if (event20->key.repeat) {
                return 1;  /* ignore 2.0-style key repeat events */
            }
            event12.key.keysym.sym = Keysym20to12(event20->key.keysym.sym);
            if (event12.key.keysym.sym == SDLK12_UNKNOWN) {
                return 1;  /* drop it if we can't map it */
            }

            KeyState[event12.key.keysym.sym] = event20->key.state;

            event12.type = (event20->type == SDL_KEYDOWN) ? SDL12_KEYDOWN : SDL12_KEYUP;
            event12.key.which = 0;
            event12.key.state = event20->key.state;
            FIXME("SDL1.2 and SDL2.0 scancodes are incompatible");
            // turns out that some apps actually made use of the hardware scancodes (checking for platform beforehand)
            event12.key.keysym.scancode = 0;
            event12.key.keysym.mod = event20->key.keysym.mod;  /* these match up between 1.2 and 2.0! */
            FIXME("unicode");
            /* without setting keysym.unicode to at least keysym.sym, text input in some games don't work at all ! */
            event12.key.keysym.unicode = (EnabledUnicode)? event12.key.keysym.sym : 0;
            break;

        case SDL_TEXTEDITING: FIXME("write me"); return 1;
        case SDL_TEXTINPUT: FIXME("write me"); return 1;

        case SDL_MOUSEMOTION:
            event12.type = SDL12_MOUSEMOTION;
            event12.motion.which = (Uint8) event20->motion.which;
            event12.motion.state = event20->motion.state;
            event12.motion.x = (Uint16) event20->motion.x;
            event12.motion.y = (Uint16) event20->motion.y;
            event12.motion.xrel = (Sint16) event20->motion.xrel;
            event12.motion.yrel = (Sint16) event20->motion.yrel;
            if (MouseInputIsRelative) {
                /* in relative mode, clamp fake absolute position to the window dimensions. */
                #define ADJUST_RELATIVE(axis, rel, dim) { \
                    MousePositionWhenRelative.axis += event20->motion.rel; \
                    if (MousePositionWhenRelative.axis <= 0) { \
                        MousePositionWhenRelative.axis = 0; \
                    } else if (MousePositionWhenRelative.axis >= VideoSurface12->dim) { \
                        MousePositionWhenRelative.axis = (VideoSurface12->dim - 1); \
                    } \
                }
                ADJUST_RELATIVE(x, xrel, w);
                ADJUST_RELATIVE(y, yrel, h);
                #undef ADJUST_RELATIVE
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
            event12.button.x = (Uint16) event20->button.x;
            event12.button.y = (Uint16) event20->button.y;
            break;

        case SDL_MOUSEBUTTONUP:
            event12.type = SDL12_MOUSEBUTTONUP;
            event12.button.which = (Uint8) event20->button.which;
            event12.button.button = event20->button.button;
            if (event12.button.button > 3) {
                event12.button.button += 2; /* SDL_BUTTON_X1/2 */
            }
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
            event12.button.state = SDL_PRESSED;
            event12.button.x = 0;
            event12.button.y = 0;
            PushEventIfNotFiltered(&event12);

            event12.type = SDL12_MOUSEBUTTONUP;  /* immediately release mouse "button" at the end of this switch. */
            event12.button.state = SDL_RELEASED;
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


static SDL12_Rect *
Rect20to12(const SDL_Rect *rect20, SDL12_Rect *rect12)
{
    rect12->x = (Sint16) rect20->x;
    rect12->y = (Sint16) rect20->y;
    rect12->w = (Uint16) rect20->w;
    rect12->h = (Uint16) rect20->h;
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
    SDL_BlendMode blendmode;
    SDL12_Surface *surface12 = NULL;
    SDL12_Palette *palette12 = NULL;
    SDL12_PixelFormat *format12 = NULL;
    Uint32 flags = 0;

    if (!surface20)
        return NULL;

    surface12 = (SDL12_Surface *) SDL20_malloc(sizeof (SDL12_Surface));
    if (!surface12)
        goto failed;

    if (surface20->format->palette) {
        palette12 = (SDL12_Palette *) SDL20_malloc(sizeof (SDL12_Palette));
        if (!palette12)
            goto failed;
    }

    format12 = (SDL12_PixelFormat *) SDL20_malloc(sizeof (SDL12_PixelFormat));
    if (!format12)
        goto failed;

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

    if (SDL20_HasColorKey(surface20)) {
        if (SDL20_GetColorKey(surface20, &format12->colorkey) < 0) {
            format12->colorkey = 0;
        } else {
            surface12->flags |= SDL12_SRCCOLORKEY;
        }
    }

    if (SDL20_GetSurfaceAlphaMod(surface20, &format12->alpha) < 0) {
        format12->alpha = 255;
    }

    blendmode = SDL_BLENDMODE_NONE;
    if ((SDL20_GetSurfaceBlendMode(surface20, &blendmode) == 0) && (blendmode == SDL_BLENDMODE_BLEND)) {
        surface12->flags |= SDL12_SRCALPHA;
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

    surface12->format = format12;
    surface12->w = surface20->w;
    surface12->h = surface20->h;
    surface12->pitch = (Uint16) surface20->pitch;  FIXME("make sure this fits in a Uint16");
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
    SDL_PixelFormat  * format20;
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
        #undef LOSSMASSSHIFTSETUP
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

        format20 = surface12->surface20->format;
        #define UPDATEFMT20(t) \
            format20->t##mask = format12->t##mask; \
            format20->t##loss = format12->t##loss; \
            format20->t##shift = format12->t##shift;
        UPDATEFMT20(R);
        UPDATEFMT20(G);
        UPDATEFMT20(B);
        UPDATEFMT20(A);
        #undef UPDATEFMT20
    }
}

DECLSPEC SDL12_Surface * SDLCALL
SDL_CreateRGBSurface(Uint32 flags12, int width, int height, int depth, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    SDL_Surface *surface20;
    SDL12_Surface *surface12;

    // SDL 1.2 checks this.
    if ((width >= 16384) || (height >= 65536)) {
        SDL20_SetError("Width or height is too large");
        return NULL;
    }

    if (depth == 8) {  // don't pass masks to SDL2 for 8-bit surfaces, it'll cause problems.
        surface20 = SDL20_CreateRGBSurface(0, width, height, depth, 0, 0, 0, 0);
    } else {
        surface20 = SDL20_CreateRGBSurface(0, width, height, depth, Rmask, Gmask, Bmask, Amask);
    }

    surface12 = Surface20to12(surface20);
    if (!surface12) {
        SDL20_FreeSurface(surface20);
        return NULL;
    }

    SDL_assert((surface12->flags & ~(SDL12_SRCCOLORKEY|SDL12_SRCALPHA)) == 0);  // shouldn't have prealloc, rleaccel, or dontfree.

    SetPalette12ForMasks(surface12, Rmask, Gmask, Bmask);

    if (flags12 & SDL12_SRCALPHA) {
        surface12->flags |= SDL12_SRCALPHA;
        SDL20_SetSurfaceBlendMode(surface20, SDL_BLENDMODE_BLEND);
    }

    return surface12;
}

DECLSPEC SDL12_Surface * SDLCALL
SDL_CreateRGBSurfaceFrom(void *pixels, int width, int height, int depth, int pitch, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
{
    SDL_Surface *surface20;
    SDL12_Surface *surface12;

    if ((width >= 16384) || (height >= 65536)) {
        SDL20_SetError("Width or height is too large");
        return NULL;
    }

    if (depth == 8) {  // don't pass masks to SDL2 for 8-bit surfaces, it'll cause problems.
        surface20 = SDL20_CreateRGBSurfaceFrom(pixels, width, height, depth, pitch, 0, 0, 0, 0);
    } else {
        surface20 = SDL20_CreateRGBSurfaceFrom(pixels, width, height, depth, pitch, Rmask, Gmask, Bmask, Amask);
    }

    surface12 = Surface20to12(surface20);
    if (!surface12) {
        SDL20_FreeSurface(surface20);
        return NULL;
    }

    SDL_assert((surface12->flags & ~(SDL12_SRCCOLORKEY|SDL12_SRCALPHA)) == SDL12_PREALLOC);  // should _only_ have prealloc.

    SetPalette12ForMasks(surface12, Rmask, Gmask, Bmask);

    return surface12;
}

DECLSPEC void SDLCALL
SDL_FreeSurface(SDL12_Surface *surface12)
{
    if (surface12 && (surface12 != VideoSurface12)) {
        SDL20_FreeSurface(surface12->surface20);
        if (surface12->format) {
            SDL20_free(surface12->format->palette);
            SDL20_free(surface12->format);
        }
        SDL20_free(surface12);
    }
}

DECLSPEC void SDLCALL
SDL_GetClipRect(SDL12_Surface *surface12, SDL12_Rect *rect)
{
    if (surface12 && rect) {
        SDL_memcpy(rect, &surface12->clip_rect, sizeof (SDL12_Rect));
    }
}

DECLSPEC SDL_bool SDLCALL
SDL_SetClipRect(SDL12_Surface *surface12, const SDL12_Rect *rect12)
{
    SDL_bool retval = SDL_FALSE;
    if (surface12)
    {
        SDL_Rect rect20;
        retval = SDL20_SetClipRect(surface12->surface20, rect12 ? Rect12to20(rect12, &rect20) : NULL);
        SDL20_GetClipRect(surface12->surface20, &rect20);
        Rect20to12(&rect20, &surface12->clip_rect);
    }
    return retval;
}

DECLSPEC int SDLCALL
SDL_FillRect(SDL12_Surface *dst, SDL12_Rect *dstrect12, Uint32 color)
{
    SDL_Rect dstrect20;
    const int retval = SDL20_FillRect(dst->surface20, dstrect12 ? Rect12to20(dstrect12, &dstrect20) : NULL, color);
    if (retval == 0)
    {
        if (dstrect12) {  /* 1.2 stores the clip intersection in dstrect */
            SDL_Rect intersected20;
            SDL20_IntersectRect(&dstrect20, &dst->surface20->clip_rect, &intersected20);
            Rect20to12(&intersected20, dstrect12);
        }
    }
    return retval;
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
    SDL20_GetRGB(pixel, PixelFormat12to20(&format20, &palette20, format12), r, g, b);
}

DECLSPEC void SDLCALL
SDL_GetRGBA(Uint32 pixel, const SDL12_PixelFormat *format12, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a)
{
    /* This is probably way slower than apps expect. */
    SDL_PixelFormat format20;
    SDL_Palette palette20;
    SDL20_GetRGBA(pixel, PixelFormat12to20(&format20, &palette20, format12), r, g, b, a);
}

DECLSPEC const SDL12_VideoInfo * SDLCALL
SDL_GetVideoInfo(void)
{
    return VideoInfo12.vfmt ? &VideoInfo12 : NULL;
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

DECLSPEC SDL12_Rect ** SDLCALL
SDL_ListModes(const SDL12_PixelFormat *format12, Uint32 flags)
{
    Uint32 fmt;
    int i;

    if (!SDL20_WasInit(SDL_INIT_VIDEO)) {
        SDL20_SetError("Video subsystem not initialized");
        return NULL;
    }

    if ((!format12) && (!VideoInfo12.vfmt)) {
        SDL20_SetError("No pixel format specified");
        return NULL;
    }

    if (!(flags & SDL12_FULLSCREEN)) {
        return (SDL12_Rect **) (-1);  /* any resolution is fine. */
    }

    if (format12 && (format12 != VideoInfo12.vfmt)) {
        fmt = SDL20_MasksToPixelFormatEnum(format12->BitsPerPixel, format12->Rmask, format12->Gmask, format12->Bmask, format12->Amask);
    } else {
        fmt = VideoInfoVfmt20->format;
    }

    for (i = 0; i < VideoModesCount; i++) {
        VideoModeList *modes = &VideoModes[i];
        if (modes->format == fmt) {
            return modes->modes12;
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

#if 0 /* unused, yet. */
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
#endif

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

    MouseInputIsRelative = SDL_FALSE;
    MousePositionWhenRelative.x = 0;
    MousePositionWhenRelative.y = 0;

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
    SDL_DisplayMode dmode;
    Uint32 fullscreen_flags20 = 0;
    Uint32 appfmt;

    FIXME("currently ignores SDL_WINDOWID, which we could use with SDL_CreateWindowFrom ...?");

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

    if (VideoSurface12 && ((VideoSurface12->flags & SDL12_OPENGL) != (flags12 & SDL12_OPENGL)) ) {
        EndVidModeCreate();  /* rebuild the window if moving to/from a GL context */
    } else if (VideoSurface12 && (VideoSurface12->surface20->format->format != appfmt)) {
        EndVidModeCreate();  /* rebuild the window if changing pixel format */
    } else if (VideoSurface12 && (VideoSurface12->w != width || VideoSurface12->h != height) && ((flags12 & SDL12_FULLSCREEN) == 0)) {
        EndVidModeCreate(); /* rebuild the window if window size changed and not in full screen */
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
        if (((flags12 & SDL12_OPENGL) == 0) || ((dmode.w == width) && (dmode.h == height))) {
            fullscreen_flags20 |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        } else {
            fullscreen_flags20 |= SDL_WINDOW_FULLSCREEN;
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
        if (VideoIcon20) {
            SDL20_SetWindowIcon(VideoWindow20, VideoIcon20);
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

    if (flags12 & SDL12_FULLSCREEN) {
        VideoSurface12->flags |= SDL12_FULLSCREEN;
    } else {
        VideoSurface12->flags &= ~SDL12_FULLSCREEN;
    }

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
        const char *env = SDL20_getenv("SDL12COMPAT_SYNC_TO_VBLANK");
        const SDL_bool want_vsync = (env && SDL20_atoi(env)) ? SDL_TRUE : SDL_FALSE;
        SDL_RendererInfo rinfo;
        SDL_assert(!VideoGLContext20);  /* either a new window or we destroyed all this */
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
        VideoSurface12->flags |= SDL12_DOUBLEBUF;
        // some apps check for the presence/lack of double buffering via MUSTLOCK
        // so set SDL12_HWSURFACE so they are less confused
        VideoSurface12->flags |= SDL12_HWSURFACE;
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
        }
    }

    SDL20_RaiseWindow(VideoWindow20);

    FIXME("setup screen saver");

    VideoSurfacePresentTicks = 0;
    VideoSurfaceLastPresentTicks = 0;

    return VideoSurface12;
}

DECLSPEC SDL12_Surface * SDLCALL
SDL_GetVideoSurface(void)
{
    return VideoSurface12;
}

static int
SaveDestAlpha(SDL12_Surface *src12, SDL12_Surface *dst12, Uint8 **retval)
{
    /* The 1.2 docs say this:
     * RGBA->RGBA:
     *     SDL_SRCALPHA set:
     * 	alpha-blend (using the source alpha channel) the RGB values;
     * 	leave destination alpha untouched. [Note: is this correct?]
     *
     * In SDL2, we change the destination alpha. We have to save it off in this case, which sucks.
     */
    Uint8 *dstalpha = NULL;
    const SDL_bool save_dstalpha = ((src12->flags & SDL12_SRCALPHA) && dst12->format->Amask && ((src12->format->alpha != 255) || src12->format->Amask)) ? SDL_TRUE : SDL_FALSE;

    FIXME("This should only save the dst rect in use");

    if (save_dstalpha) {
        Uint8 *dptr;
        int x, y;

        const int w = dst12->w;
        const int h = dst12->h;

        const Uint32 amask = dst12->format->Amask;
        const Uint32 ashift = dst12->format->Ashift;
        const Uint16 pitch = dst12->pitch;

        dstalpha = (Uint8 *) SDL_malloc(w * h);
        if (!dstalpha) {
            *retval = NULL;
            return SDL20_OutOfMemory();
        }
        dptr = dstalpha;

        if ((amask == 0xFF) || (amask == 0xFF00) || (amask == 0xFF0000) ||(amask == 0xFF000000)) {
            FIXME("this could be SIMD'd");
        }
        if (dst12->format->BytesPerPixel == 2) {
            const Uint16 *sptr = (const Uint16 *) dst12->pixels;
            for (y = 0; y < h; y++) {
                for (x = 0; x < w; x++) {
                    *(dptr++) = (Uint8) ((sptr[x] & amask) >> ashift);
                }
                sptr = (Uint16 *) (((Uint8 *) sptr) + pitch);
            }
        } else if (dst12->format->BytesPerPixel == 4) {
            const Uint32 *sptr = (const Uint32 *) dst12->pixels;
            for (y = 0; y < h; y++) {
                for (x = 0; x < w; x++) {
                    *(dptr++) = (Uint8) ((sptr[x] & amask) >> ashift);
                }
                sptr = (Uint32 *) (((Uint8 *) sptr) + pitch);
            }
        } else {
            FIXME("Unhandled dest alpha");
        }
    }

    *retval = dstalpha;
    return 0;
}

static void
RestoreDestAlpha(SDL12_Surface *dst12, Uint8 *dstalpha)
{
    if (dstalpha) {
        int x, y;

        const int w = dst12->w;
        const int h = dst12->h;
        const Uint8 *sptr = dstalpha;
        const Uint32 amask = dst12->format->Amask;
        const Uint32 ashift = dst12->format->Ashift;
        const Uint16 pitch = dst12->pitch;

        if ((amask == 0xFF) || (amask == 0xFF00) || (amask == 0xFF0000) ||(amask == 0xFF000000)) {
            FIXME("this could be SIMD'd");
        }
        if (dst12->format->BytesPerPixel == 2) {
            Uint16 *dptr = (Uint16 *) dst12->pixels;
            for (y = 0; y < h; y++) {
                for (x = 0; x < w; x++) {
                    dptr[x] = (Uint16) ((dptr[x] & ~amask) | ((((Uint16) *(sptr++)) << ashift) & amask));
                }
                dptr = (Uint16 *) (((Uint8 *) dptr) + pitch);
            }
        } else if (dst12->format->BytesPerPixel == 4) {
            Uint32 *dptr = (Uint32 *) dst12->pixels;
            for (y = 0; y < h; y++) {
                for (x = 0; x < w; x++) {
                    dptr[x] = (dptr[x] & ~amask) | ((((Uint32) *(sptr++)) << ashift) & amask);
                }
                dptr = (Uint32 *) (((Uint8 *) dptr) + pitch);
            }
        } else {
            FIXME("Unhandled dest alpha");
        }
        SDL_free(dstalpha);
    }
}

DECLSPEC int SDLCALL
SDL_UpperBlit(SDL12_Surface *src12, SDL12_Rect *srcrect12, SDL12_Surface *dst12, SDL12_Rect *dstrect12)
{
    Uint8 *dstalpha;
    SDL_Rect srcrect20, dstrect20;
    int retval;

    if ((src12 == NULL) || (dst12 == NULL)) {
        return SDL20_SetError("SDL_UpperBlit: passed a NULL surface");
    } else if (SaveDestAlpha(src12, dst12, &dstalpha) < 0) {
        return -1;
    }

    retval = SDL20_UpperBlit(src12->surface20,
                             srcrect12 ? Rect12to20(srcrect12, &srcrect20) : NULL,
                             dst12->surface20,
                             dstrect12 ? Rect12to20(dstrect12, &dstrect20) : NULL);

    RestoreDestAlpha(dst12, dstalpha);

    if (dstrect12) {
        Rect20to12(&dstrect20, dstrect12);
    }

    return retval;
}

DECLSPEC int SDLCALL
SDL_LowerBlit(SDL12_Surface *src12, SDL12_Rect *srcrect12, SDL12_Surface *dst12, SDL12_Rect *dstrect12)
{
    Uint8 *dstalpha;
    SDL_Rect srcrect20, dstrect20;
    int retval;

    if (SaveDestAlpha(src12, dst12, &dstalpha) < 0) {
        return -1;
    }

    retval = SDL20_LowerBlit(src12->surface20,
                             srcrect12 ? Rect12to20(srcrect12, &srcrect20) : NULL,
                             dst12->surface20,
                             dstrect12 ? Rect12to20(dstrect12, &dstrect20) : NULL);

    RestoreDestAlpha(dst12, dstalpha);

    if (srcrect12) {
        Rect20to12(&srcrect20, srcrect12);
    }

    if (dstrect12) {
        Rect20to12(&dstrect20, dstrect12);
    }

    return retval;
}

DECLSPEC int SDLCALL
SDL_SetAlpha(SDL12_Surface *surface12, Uint32 flags12, Uint8 value)
{
    const SDL_bool addkey = (flags12 & SDL12_SRCALPHA) ? SDL_TRUE : SDL_FALSE;
    const int retval = SDL20_SetSurfaceAlphaMod(surface12->surface20, addkey ? value : 255);
    if (SDL20_GetSurfaceAlphaMod(surface12->surface20, &surface12->format->alpha) < 0) {
        surface12->format->alpha = 255;
    }
    FIXME("Should this set SDL12_SRCALPHA on the surface?");
    return retval;
}

DECLSPEC int SDLCALL
SDL_LockSurface(SDL12_Surface *surface12)
{
    const int retval = SDL20_LockSurface(surface12->surface20);
    surface12->pixels = surface12->surface20->pixels;
    surface12->pitch = surface12->surface20->pitch;
    return retval;
}

DECLSPEC void SDLCALL
SDL_UnlockSurface(SDL12_Surface *surface12)
{
    SDL20_UnlockSurface(surface12->surface20);
    surface12->pixels = surface12->surface20->pixels;
    surface12->pitch = surface12->surface20->pitch;
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
        } else {
            if (flags12 & SDL12_SRCALPHA) {
                SDL20_SetSurfaceBlendMode(surface20, SDL_BLENDMODE_BLEND);
                retval->flags |= SDL12_SRCALPHA;
            }
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
SDL_DisplayFormatAlpha(SDL12_Surface *surface12)
{
    return SDL_DisplayFormat(surface12);
}

static void
PresentScreen(void)
{
    void *pixels = NULL;
    int pitch = 0;

    SDL_assert(VideoSurface12 != NULL);

    if (SDL20_LockTexture(VideoTexture20, NULL, &pixels, &pitch) < 0) {
        return;  /* oh well */
    }

    FIXME("Maybe lock texture always, until present, if no conversion needed?");
    if (VideoConvertSurface20) {
        VideoConvertSurface20->pixels = pixels;
        VideoConvertSurface20->pitch = pitch;
        SDL20_UpperBlit(VideoSurface12->surface20, NULL, VideoConvertSurface20, NULL);
        VideoConvertSurface20->pixels = NULL;
        VideoConvertSurface20->pitch = 0;
    } else if (pitch == VideoSurface12->pitch) {
        SDL_memcpy(pixels, VideoSurface12->pixels, pitch * VideoSurface12->h);
    } else {
        const int srcpitch = VideoSurface12->pitch;
        const int cpy = SDL_min(srcpitch, pitch);
        const int h = VideoSurface12->h;
        char *dst = (char *) pixels;
        char *src = (char *) VideoSurface12->pixels;
        int i = 0;
        for (; i < h; i++) {
            SDL_memcpy(dst, src, cpy);
            src += srcpitch;
            dst += pitch;
        }
    }

    SDL20_UnlockTexture(VideoTexture20);

    SDL20_RenderClear(VideoRenderer20);
    SDL20_RenderCopy(VideoRenderer20, VideoTexture20, NULL, NULL);
    SDL20_RenderPresent(VideoRenderer20);
    VideoSurfaceLastPresentTicks = SDL20_GetTicks();
    VideoSurfacePresentTicks = 0;
}

DECLSPEC void SDLCALL
SDL_UpdateRects(SDL12_Surface *surface12, int numrects, SDL12_Rect *rects12)
{
    /* strangely, SDL 1.2 doesn't check if surface12 is NULL before touching it */
    /* (UpdateRect, singular, does...) */
    if (surface12->flags & SDL12_OPENGL) {
        SDL20_SetError("Use SDL_GL_SwapBuffers() on OpenGL surfaces");
        return;
    }

    // everything else is marked SDL12_DOUBLEBUF and SHOULD BE a no-op here,
    //  but in practice most apps never got a double-buffered surface and
    //  don't handle it correctly, so we have to work around it.
    if (surface12 == VideoSurface12) {
        SDL_bool whole_screen = SDL_FALSE;
        if (numrects == 1) {
            const SDL12_Rect *r = rects12;
            if (!r->x && !r->y && !r->w && !r->h) {
                whole_screen = SDL_TRUE;
            } else if (!r->x && !r->y && (r->w == surface12->w) && (r->h == surface12->h)) {
                whole_screen = SDL_TRUE;
            }
        }

        if (whole_screen) {
            PresentScreen();  // flip it now.
        } else {
            VideoSurfacePresentTicks = VideoSurfaceLastPresentTicks + 15;  // flip it later.
        }
    }
}

DECLSPEC void SDLCALL
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

DECLSPEC int SDLCALL
SDL_Flip(SDL12_Surface *surface12)
{
    if (surface12->flags & SDL12_OPENGL) {
        return SDL20_SetError("Use SDL_GL_SwapBuffers() on OpenGL surfaces");
    }

    if (surface12 == VideoSurface12) {
        PresentScreen();
    }

    return 0;
}

DECLSPEC void SDLCALL
SDL_PumpEvents(void)
{
    SDL_Event e;

    // If the app is doing dirty rectangles, we set a flag and present the
    //  screen surface when they pump for new events if we're close to 60Hz,
    //  which we consider a sign that they are done rendering for the current
    //  frame and it would make sense to send it to the screen.
    if (VideoSurfacePresentTicks && SDL_TICKS_PASSED(SDL20_GetTicks(), VideoSurfacePresentTicks)) {
        PresentScreen();
    }
    while (SDL20_PollEvent(&e)) {  /* spin to drain the SDL2 event queue. */ }
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
SDL_WM_SetIcon(SDL12_Surface *icon12, Uint8 *mask)
{
    SDL_BlendMode oldmode;
    Uint32 rmask, gmask, bmask, amask;
    SDL_Surface *icon20;
    int bpp;
    int ret;

    if (VideoWindow20) {
        SDL20_SetWindowIcon(VideoWindow20, icon12->surface20);
        return;
    }

    // take the mask and zero out those alpha values.
    oldmode = SDL_BLENDMODE_NONE;
    if (SDL20_GetSurfaceBlendMode(icon12->surface20, &oldmode) < 0) {
        return; // oh well.
    }
    if (!SDL20_PixelFormatEnumToMasks(SDL_PIXELFORMAT_ARGB8888, &bpp, &rmask, &gmask, &bmask, &amask)) {
        return; // oh well.
    }
    icon20 = SDL20_CreateRGBSurface(0, icon12->w, icon12->h, bpp, rmask, gmask, bmask, amask);
    if (!icon20) {
        return; // oh well.
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
            newflags20 = (VideoSurface12->flags & SDL12_OPENGL) ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_FULLSCREEN_DESKTOP;
            retval = (SDL20_SetWindowFullscreen(VideoWindow20, newflags20) == 0);
            if (retval) {
                VideoSurface12->flags |= SDL12_FULLSCREEN;
            }
        }
        if (retval && VideoRenderer20) {
            SDL20_RenderSetLogicalSize(VideoRenderer20, VideoSurface12->w, VideoSurface12->h);
        }
    }
    return retval;
}

typedef enum
{
    SDL12_GRAB_QUERY = -1,
    SDL12_GRAB_OFF = 0,
    SDL12_GRAB_ON = 1
} SDL12_GrabMode;

static void
UpdateRelativeMouseMode(void)
{
    // in SDL 1.2, hiding+grabbing the cursor was like SDL2's relative mouse mode.
    if (VideoWindow20) {
        const SDL_bool enable = (VideoWindowGrabbed && VideoCursorHidden) ? SDL_TRUE : SDL_FALSE;
        if (MouseInputIsRelative != enable) {
            MouseInputIsRelative = enable;
            if (MouseInputIsRelative) {
                // reset position, we'll have to track it ourselves in SDL_MOUSEMOTION events, since 1.2
                //  would give you window coordinates, even in relative mode.
                SDL_GetMouseState(&MousePositionWhenRelative.x, &MousePositionWhenRelative.y);
            }
            SDL20_SetRelativeMouseMode(MouseInputIsRelative);
        }
    }
}

DECLSPEC int SDLCALL
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


DECLSPEC SDL12_GrabMode SDLCALL
SDL_WM_GrabInput(SDL12_GrabMode mode)
{
    if (mode != SDL12_GRAB_QUERY) {
        const SDL_bool wantgrab = (mode == SDL12_GRAB_ON) ? SDL_TRUE : SDL_FALSE;
        if (VideoWindowGrabbed != wantgrab) {
            SDL20_SetWindowGrab(VideoWindow20, wantgrab);
            VideoWindowGrabbed = wantgrab;
            UpdateRelativeMouseMode();
        }
    }
    return VideoWindowGrabbed ? SDL12_GRAB_ON : SDL12_GRAB_OFF;
}

DECLSPEC void SDLCALL
SDL_WarpMouse(Uint16 x, Uint16 y)
{
    if (MouseInputIsRelative) {  /* we have to track this ourselves, in case app calls SDL_GetMouseState(). */
        MousePositionWhenRelative.x = (int) x;
        MousePositionWhenRelative.y = (int) y;
    } else {
        SDL20_WarpMouseInWindow(VideoWindow20, x, y);
    }
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

DECLSPEC int SDLCALL
SDL_SetPalette(SDL12_Surface *surface12, int flags, const SDL_Color *colors,
               int firstcolor, int ncolors)
{
    SDL12_Palette *palette12;
    SDL_Palette *palette20;
    SDL_Color *opaquecolors;
    int i, retval;

    if (!surface12) {
        return 0;  // not an error, a no-op.
    }

    if ((flags & (SDL12_LOGPAL | SDL12_PHYSPAL)) == 0) {
        return 0;  // nothing to do.
    }

    palette12 = surface12->format->palette;
    if (!palette12) {
        return 0;  // not an error, a no-op.
    }

    palette20 = surface12->surface20->format->palette;
    SDL_assert(palette20 != NULL);

    // we need to force the "unused" field to 255, since it's "alpha" in SDL2.
    opaquecolors = (SDL_Color *) SDL20_malloc(sizeof (SDL_Color) * ncolors);
    if (!opaquecolors) {
        return SDL20_OutOfMemory();
    }

    // don't SDL_memcpy in case the 'a' field is uninitialized and upsets
    //  memory tools like Valgrind.
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

    SDL20_free(opaquecolors);

    // in case this pointer changed...
    palette12->colors = palette20->colors;

    FIXME("We need to store the physical palette somewhere");  // it might be different that logical palette and it will be needed for future blits.
    if ((surface12 == VideoSurface12) && (flags & SDL12_PHYSPAL)) {
        SDL_UpdateRect(surface12, 0, 0, 0, 0);   // force screen to reblit with new palette.
    }

    return retval;
}

DECLSPEC int SDLCALL
SDL_SetColors(SDL12_Surface *surface12, const SDL_Color * colors, int firstcolor,
              int ncolors)
{
    return SDL_SetPalette(surface12, SDL12_LOGPAL | SDL12_PHYSPAL, colors, firstcolor, ncolors);
}

DECLSPEC int SDLCALL
SDL_GetWMInfo(SDL_SysWMinfo * info)
{
    //return SDL20_GetWindowWMInfo(VideoWindow20, info);
    FIXME("write me");
    SDL20_Unsupported();
    return 0; /* some programs only test against 0, not -1 */
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
SDL_DisplayYUVOverlay(SDL12_Overlay * overlay, SDL12_Rect * dstrect12)
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
    FIXME("write me");
    return 0;
}

DECLSPEC void SDLCALL
SDL_GetKeyRepeat(int *delay, int *interval)
{
    FIXME("write me");
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
    int old = EnabledUnicode;
    FIXME("implement properly");
    EnabledUnicode = enable;
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

DECLSPEC int SDLCALL
SDL_SetTimer(Uint32 interval, SDL12_TimerCallback callback)
{
    static SDL_TimerID compat_timer;

    if (compat_timer) {
        SDL20_RemoveTimer(compat_timer);
        compat_timer = 0;
    }

    if (interval && callback) {
        interval = RoundTimerTo12Resolution(interval);
        compat_timer = SDL20_AddTimer(interval, SetTimerCallback12, callback);
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
DECLSPEC SDL_Thread * SDLCALL
SDL_CreateThread(int (SDLCALL *fn)(void *), void *data)
{
    return SDL20_CreateThread(fn, NULL, data, NULL, NULL);
}
#else
DECLSPEC SDL_Thread * SDLCALL
SDL_CreateThread(int (SDLCALL *fn)(void *), void *data, pfnSDL_CurrentBeginThread pfnBeginThread, pfnSDL_CurrentEndThread pfnEndThread)
{
    return SDL20_CreateThread(fn, NULL, data, pfnBeginThread, pfnEndThread);
}
#endif
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

DECLSPEC void SDLCALL
SDL_KillThread(SDL_Thread *thread)
{
    FIXME("Removed from 2.0; do nothing. We can't even report failure.");
    SDL20_Log("WARNING: this app used SDL_KillThread(), an unforgivable curse.\n"
              "This program should be fixed. No thread was actually harmed.\n");
}

typedef struct SDL12_TimerID_Data
{
    SDL_TimerID timer_id;
    SDL12_NewTimerCallback callback;
    void *param;
} SDL12_TimerID_Data;

/* This changed from an opaque pointer to an int in 2.0. */
typedef SDL12_TimerID_Data *SDL12_TimerID;
SDL_COMPILE_TIME_ASSERT(timer, sizeof(SDL12_TimerID) >= sizeof(SDL_TimerID));


static Uint32 SDLCALL
AddTimerCallback12(Uint32 interval, void *param)
{
    SDL12_TimerID data = (SDL12_TimerID) param;
    return RoundTimerTo12Resolution(data->callback(interval, data->param));
}

DECLSPEC SDL12_TimerID SDLCALL
SDL_AddTimer(Uint32 interval, SDL12_NewTimerCallback callback, void *param)
{
    SDL12_TimerID data = (SDL12_TimerID) SDL_malloc(sizeof (SDL12_TimerID_Data));
    if (!data) {
        SDL20_OutOfMemory();
        return NULL;
    }

    interval = RoundTimerTo12Resolution(interval);
    data->callback = callback;
    data->param = param;
    data->timer_id = SDL20_AddTimer(interval, AddTimerCallback12, data);

    if (!data->timer_id) {
        SDL_free(data);
        return NULL;
    }

    return data;
}

DECLSPEC SDL_bool SDLCALL
SDL_RemoveTimer(SDL12_TimerID data)
{
    // !!! FIXME: classic 1.2 will safely return SDL_FALSE if this is a bogus
    // !!! FIXME:  timer. This code will dereference a bogus pointer.
    const SDL_bool retval = SDL20_RemoveTimer(data->timer_id);
    if (retval) {
        SDL_free(data);
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
    if (!file || !*file || !mode || !*mode) {
        SDL20_SetError("SDL_RWFromFile(): No file or no mode specified");
        return NULL;
    }
    return RWops20to12(SDL20_RWFromFile(file, mode));
}

DECLSPEC SDL12_RWops * SDLCALL
SDL_RWFromFP(void *io, int autoclose)
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

    pos = rwops12->seek(rwops12, 0, RW_SEEK_CUR);
    if (pos == -1) {
        return SDL20_Error(SDL_EFSEEK);
    }
    size = (Sint64) rwops12->seek(rwops12, 0, RW_SEEK_END);
    rwops12->seek(rwops12, pos, RW_SEEK_SET);
    rwops20->hidden.unknown.data2 = (void *) ((size_t) size);
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
    SDL_RWops *rwops20 = RWops12to20(rwops12);
    const int retval = SDL20_SaveBMP_RW(surface12->surface20, rwops20, freerwops12);
    FIXME("wrap surface");
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

typedef struct
{
    void (SDLCALL *app_callback)(void *userdata, Uint8 *stream, int len);
    void *app_userdata;
    Uint8 silence;
} AudioCallbackWrapperData;

static void SDLCALL
AudioCallbackWrapper(void *userdata, Uint8 *stream, int len)
{
    AudioCallbackWrapperData *data = (AudioCallbackWrapperData *) userdata;
    SDL_memset(stream, data->silence, len);  // SDL2 doesn't clear the stream before calling in here, but 1.2 expects it.
    data->app_callback(data->app_userdata, stream, len);
}


DECLSPEC int SDLCALL
SDL_OpenAudio(SDL_AudioSpec *want, SDL_AudioSpec *obtained)
{
    AudioCallbackWrapperData *data;
    int retval;

    // SDL2 uses a NULL callback to mean "we play to use SDL_QueueAudio()"
    if (want && (want->callback == NULL)) {
        return SDL20_SetError("Callback can't be NULL");
    }

    data = (AudioCallbackWrapperData *) SDL20_calloc(1, sizeof (AudioCallbackWrapperData));
    if (!data) {
        return SDL20_OutOfMemory();
    }
    data->app_callback = want->callback;
    data->app_userdata = want->userdata;
    want->callback = AudioCallbackWrapper;
    want->userdata = data;

    FIXME("Don't allow int32 or float32");
    FIXME("clamp output to mono/stereo");
    retval = SDL20_OpenAudio(want, obtained);
    want->callback = data->app_callback;
    want->userdata = data->app_userdata;
    if (retval < 0) {
        SDL20_free(data);
    } else {
        FIXME("memory leak on callback data");
        if (!obtained) {
            data->silence = want->silence;
        } else {
            data->silence = obtained->silence;
            obtained->callback = data->app_callback;
            obtained->userdata = data->app_userdata;
        }
    }

    return retval;
}


/* SDL_GL_DisableContext and SDL_GL_EnableContext_Thread are not real SDL 1.2
   APIs, but some idTech4 games shipped with a custom SDL 1.2 build that added
   these functions, to let them make a GL context current on a background thread,
   so we supply them as well to be binary compatible for those games. */

DECLSPEC void SDLCALL
SDL_GL_DisableContext(void)
{
    SDL20_GL_MakeCurrent(NULL, NULL);
}

DECLSPEC void SDLCALL
SDL_GL_EnableContext_Thread(void)
{
    const SDL_bool enable = (VideoGLContext20 && VideoWindow20)? SDL_TRUE : SDL_FALSE;
    SDL20_GL_MakeCurrent(enable ? VideoWindow20 : NULL, enable ? VideoGLContext20 : NULL);
}

/* vi: set ts=4 sw=4 expandtab: */
