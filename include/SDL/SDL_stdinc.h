/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2024 Sam Lantinga <slouken@libsdl.org>

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

#ifndef _SDL_stdinc_h
#define _SDL_stdinc_h

#include "SDL_config.h"

/* we are running under the assumption that if you have SDL2, you _probably_
have these standard headers. If not, send patches. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SDL_arraysize(array) (sizeof(array)/sizeof(array[0]))
#define SDL_TABLESIZE(table) SDL_arraysize(table)

#ifdef __cplusplus
#define SDL_reinterpret_cast(type, expression) reinterpret_cast<type>(expression)
#define SDL_static_cast(type, expression) static_cast<type>(expression)
#else
#define SDL_reinterpret_cast(type, expression) ((type)(expression))
#define SDL_static_cast(type, expression) ((type)(expression))
#endif

typedef enum SDL_bool
{
    SDL_FALSE,
    SDL_TRUE
} SDL_bool;

typedef int8_t Sint8;
typedef uint8_t Uint8;
typedef int16_t Sint16;
typedef uint16_t Uint16;
typedef int32_t Sint32;
typedef uint32_t Uint32;
typedef uint64_t Uint64;
typedef int64_t Sint64;

#define SDL_COMPILE_TIME_ASSERT(name, x) typedef int SDL_dummy_ ## name[(x) * 2 - 1]
SDL_COMPILE_TIME_ASSERT(uint8, sizeof(Uint8) == 1);
SDL_COMPILE_TIME_ASSERT(sint8, sizeof(Sint8) == 1);
SDL_COMPILE_TIME_ASSERT(uint16, sizeof(Uint16) == 2);
SDL_COMPILE_TIME_ASSERT(sint16, sizeof(Sint16) == 2);
SDL_COMPILE_TIME_ASSERT(uint32, sizeof(Uint32) == 4);
SDL_COMPILE_TIME_ASSERT(sint32, sizeof(Sint32) == 4);
SDL_COMPILE_TIME_ASSERT(uint64, sizeof(Uint64) == 8);
SDL_COMPILE_TIME_ASSERT(sint64, sizeof(Sint64) == 8);

typedef enum SDL_DUMMY_ENUM
{
    DUMMY_ENUM_VALUE
} SDL_DUMMY_ENUM;

SDL_COMPILE_TIME_ASSERT(enum, sizeof(SDL_DUMMY_ENUM) == sizeof(int));

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#if defined(HAVE_STDLIB_H)
#include <stdlib.h>
#elif defined(HAVE_MALLOC_H)
#include <malloc.h>
#endif
#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif
#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
#ifdef HAVE_ICONV_H
#include <iconv.h>
#endif
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

#include "begin_code.h"

/* sdl12-compat always supplies these functions, but the real SDL 1.2 might
not, so we try to #define them to be the C runtime equivalents as possible,
so things built with these headers will still run against the real SDL 1.2. */

#ifdef HAVE_ICONV
#define SDL_iconv_t iconv_t
#define SDL_iconv_open iconv_open
#define SDL_iconv_close iconv_close
#else
typedef struct _SDL_iconv_t *SDL_iconv_t;
extern DECLSPEC SDL_iconv_t SDLCALL SDL_iconv_open(const char *tocode, const char *fromcode);
extern DECLSPEC int SDLCALL SDL_iconv_close(SDL_iconv_t cd);
#endif
extern DECLSPEC size_t SDLCALL SDL_iconv(SDL_iconv_t cd, const char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft);
extern DECLSPEC char * SDLCALL SDL_iconv_string(const char *tocode, const char *fromcode, const char *inbuf, size_t inbytesleft);

#ifdef HAVE_MALLOC
#define SDL_malloc malloc
#else
extern DECLSPEC void * SDLCALL SDL_malloc(size_t size);
#endif

#ifdef HAVE_CALLOC
#define SDL_calloc calloc
#else
extern DECLSPEC void * SDLCALL SDL_calloc(size_t nmemb, size_t size);
#endif

#ifdef HAVE_REALLOC
#define SDL_realloc realloc
#else
extern DECLSPEC void * SDLCALL SDL_realloc(void *mem, size_t size);
#endif

#ifdef HAVE_FREE
#define SDL_free free
#else
extern DECLSPEC void SDLCALL SDL_free(void *mem);
#endif

#ifdef HAVE_GETENV
#define SDL_getenv getenv
#else
extern DECLSPEC char * SDLCALL SDL_getenv(const char *name);
#endif

#ifdef HAVE_PUTENV
#define SDL_putenv putenv
#else
extern DECLSPEC int SDLCALL SDL_putenv(const char *variable);
#endif

#ifdef HAVE_QSORT
#define SDL_qsort qsort
#else
extern DECLSPEC void SDLCALL SDL_qsort(void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));
#endif

#ifdef HAVE_MEMSET
#define SDL_memset memset
#else
extern DECLSPEC void * SDLCALL SDL_memset(void *dst, int c, size_t len);
#endif

#ifdef HAVE_MEMCPY
#define SDL_memcpy memcpy
#else
extern DECLSPEC void * SDLCALL SDL_memcpy(void *dst, const void *src, size_t len);
#endif

#ifdef HAVE_REVCPY
#define SDL_revcpy revcpy
#else
extern DECLSPEC void * SDLCALL SDL_revcpy(void *dst, const void *src, size_t len);
#endif

#ifdef HAVE_MEMCMP
#define SDL_memcmp memcmp
#else
extern DECLSPEC int SDLCALL SDL_memcmp(const void *s1, const void *s2, size_t len);
#endif

#ifdef HAVE_STRLEN
#define SDL_strlen strlen
#else
extern DECLSPEC size_t SDLCALL SDL_strlen(const char *string);
#endif

#ifdef HAVE_STRLCPY
#define SDL_strlcpy strlcpy
#else
extern DECLSPEC size_t SDLCALL SDL_strlcpy(char *dst, const char *src, size_t maxlen);
#endif

#ifdef HAVE_STRLCAT
#define SDL_strlcat strlcat
#else
extern DECLSPEC size_t SDLCALL SDL_strlcat(char *dst, const char *src, size_t maxlen);
#endif

#ifdef HAVE_STRDUP
#define SDL_strdup strdup
#else
extern DECLSPEC char * SDLCALL SDL_strdup(const char *string);
#endif

#ifdef HAVE__STRREV
#define SDL_strrev _strrev
#else
extern DECLSPEC char * SDLCALL SDL_strrev(char *string);
#endif

#ifdef HAVE__STRUPR
#define SDL_strupr _strupr
#else
extern DECLSPEC char * SDLCALL SDL_strupr(char *string);
#endif

#ifdef HAVE__STRLWR
#define SDL_strlwr _strlwr
#else
extern DECLSPEC char * SDLCALL SDL_strlwr(char *string);
#endif

#ifdef HAVE_STRCHR
#define SDL_strchr strchr
#else
extern DECLSPEC char * SDLCALL SDL_strchr(const char *string, int c);
#endif

#ifdef HAVE_STRRCHR
#define SDL_strrchr strrchr
#else
extern DECLSPEC char * SDLCALL SDL_strrchr(const char *string, int c);
#endif

#ifdef HAVE_STRSTR
#define SDL_strstr strstr
#else
extern DECLSPEC char * SDLCALL SDL_strstr(const char *haystack, const char *needle);
#endif

#ifdef HAVE__LTOA
#define SDL_ltoa _ltoa
#else
extern DECLSPEC char * SDLCALL SDL_ltoa(long value, char *string, int radix);
#endif

#ifdef HAVE__ULTOA
#define SDL_ultoa _ultoa
#else
extern DECLSPEC char * SDLCALL SDL_ultoa(unsigned long value, char *string, int radix);
#endif

#ifdef HAVE_STRTOL
#define SDL_strtol strtol
#else
extern DECLSPEC long SDLCALL SDL_strtol(const char *string, char **endp, int base);
#endif

#ifdef HAVE_STRTOUL
#define SDL_strtoul strtoul
#else
extern DECLSPEC unsigned long SDLCALL SDL_strtoul(const char *string, char **endp, int base);
#endif

#ifdef HAVE__I64TOA
#define SDL_lltoa _i64toa
#else
extern DECLSPEC char* SDLCALL SDL_lltoa(Sint64 value, char *string, int radix);
#endif

#ifdef HAVE__UI64TOA
#define SDL_ulltoa _ui64toa
#else
extern DECLSPEC char* SDLCALL SDL_ulltoa(Uint64 value, char *string, int radix);
#endif

#ifdef HAVE__STRTOI64
#define SDL_strtoll _strtoi64
#elif defined(HAVE_STRTOLL)
#define SDL_strtoll strtoll
#else
extern DECLSPEC Sint64 SDLCALL SDL_strtoll(const char *string, char **endp, int base);
#endif

#ifdef HAVE__STRTOUI64
#define SDL_strtoull _strtoui64
#elif defined(HAVE_STRTOULL)
#define SDL_strtoull strtoull
#else
extern DECLSPEC Uint64 SDLCALL SDL_strtoull(const char *string, char **endp, int base);
#endif

#ifdef HAVE_STRTOD
#define SDL_strtod strtod
#else
extern DECLSPEC double SDLCALL SDL_strtod(const char *string, char **endp);
#endif

#ifdef HAVE_STRCMP
#define SDL_strcmp strcmp
#else
extern DECLSPEC int SDLCALL SDL_strcmp(const char *str1, const char *str2);
#endif

#ifdef HAVE_STRNCMP
#define SDL_strncmp strncmp
#else
extern DECLSPEC int SDLCALL SDL_strncmp(const char *str1, const char *str2, size_t maxlen);
#endif

#ifdef HAVE_STRCASECMP
#define SDL_strcasecmp strcasecmp
#elif defined(HAVE__STRICMP)
#define SDL_strcasecmp _stricmp
#else
extern DECLSPEC int SDLCALL SDL_strcasecmp(const char *str1, const char *str2);
#endif

#ifdef HAVE_STRNCASECMP
#define SDL_strncasecmp strncasecmp
#elif defined(HAVE__STRNICMP)
#define SDL_strncasecmp _strnicmp
#else
extern DECLSPEC int SDLCALL SDL_strncasecmp(const char *str1, const char *str2, size_t maxlen);
#endif

#ifdef HAVE_SSCANF
#define SDL_sscanf sscanf
#else
extern DECLSPEC int SDLCALL SDL_sscanf(const char *text, const char *fmt, ...);
#endif

#ifdef HAVE_SNPRINT
#define SDL_snprintf snprintf
#else
extern DECLSPEC int SDLCALL SDL_snprintf(char *text, size_t maxlen, const char *fmt, ...);
#endif

#ifdef HAVE_VSNPRINTF
#define SDL_vsnprintf vsnprintf
#else
extern DECLSPEC int SDLCALL SDL_vsnprintf(char *text, size_t maxlen, const char *fmt, va_list ap);
#endif


/* taken from SDL2's zlib-licensed headers. */
#define SDL_memset4(dst, val, dwords) do { \
    if (dwords > 0) { \
      size_t _n = (dwords + 3) / 4; \
      Uint32 *_p = SDL_static_cast(Uint32 *, dst); \
      const Uint32 _val = (val); \
      switch (dwords % 4) { \
        case 0: do {    *_p++ = _val;   /* fallthrough */ \
        case 3:         *_p++ = _val;   /* fallthrough */ \
        case 2:         *_p++ = _val;   /* fallthrough */ \
        case 1:         *_p++ = _val;   /* fallthrough */ \
        } while ( --_n ); \
      } \
    } \
  } while (0)

#ifdef HAVE_ALLOCA
#define SDL_stack_alloc(type, count) (type*)alloca(sizeof(type)*(count))
#define SDL_stack_free(data)
#else
#define SDL_stack_alloc(type, count) (type*)SDL_malloc(sizeof(type)*(count))
#define SDL_stack_free(data) SDL_free(data)
#endif

/* we're currently assuming you actually have these parts of the C runtime. Send patches if not. */
#define SDL_abs abs
#define SDL_min(x, y) (((x) < (y)) ? (x) : (y))
#define SDL_max(x, y) (((x) > (y)) ? (x) : (y))

#define SDL_isdigit(X)  isdigit(X)
#define SDL_isspace(X)  isspace(X)
#define SDL_toupper(X)  toupper(X)
#define SDL_tolower(X)  tolower(X)

#define SDL_memcpy4(dst, src, len) SDL_memcpy(dst, src, (len) << 2)

#ifdef HAVE_MEMMOVE
#define SDL_memmove memmove
#else
#define SDL_memmove(dst, src, len) do { if ( dst < src ) { SDL_memcpy(dst, src, len); } else { SDL_revcpy(dst, src, len); } } while(0)
#endif

#define SDL_itoa(value, string, radix) SDL_ltoa((long)value, string, radix)
#define SDL_uitoa(value, string, radix) SDL_ultoa((long)value, string, radix)
#define SDL_atoi(X)     SDL_strtol(X, NULL, 10)
#define SDL_atof(X)     SDL_strtod(X, NULL)

#define SDL_ICONV_ERROR (size_t)-1
#define SDL_ICONV_E2BIG (size_t)-2
#define SDL_ICONV_EILSEQ (size_t)-3
#define SDL_ICONV_EINVAL (size_t)-4

#define SDL_iconv_utf8_locale(S) SDL_iconv_string("", "UTF-8", S, SDL_strlen(S)+1)
#define SDL_iconv_utf8_ucs2(S) (Uint16 *)SDL_iconv_string("UCS-2", "UTF-8", S, SDL_strlen(S)+1)
#define SDL_iconv_utf8_ucs4(S) (Uint32 *)SDL_iconv_string("UCS-4", "UTF-8", S, SDL_strlen(S)+1)

#include "close_code.h"

#endif /* _SDL_stdinc_h */
