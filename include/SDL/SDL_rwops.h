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

#ifndef _SDL_rwops_h
#define _SDL_rwops_h

#include "SDL_stdinc.h"
#include "SDL_error.h"

#include "begin_code.h"

typedef struct SDL_RWops
{
    int (SDLCALL *seek)(struct SDL_RWops *context, int offset, int whence);
    int (SDLCALL *read)(struct SDL_RWops *context, void *ptr, int size, int maxnum);
    int (SDLCALL *write)(struct SDL_RWops *context, const void *ptr, int size, int num);
    int (SDLCALL *close)(struct SDL_RWops *context);

    Uint32 type;
    union
    {
#if defined(__WIN32__)
        struct
        {
            int append;
            void *h;
            struct
            {
                void *data;
                int size;
                int left;
            } buffer;
        } win32io;
#endif

        struct
        {
            int autoclose;
            FILE *fp;
        } stdio;

        struct
        {
            Uint8 *base;
            Uint8 *here;
            Uint8 *stop;
        } mem;

        struct
        {
            void *data1;
        } unknown;
    } hidden;
} SDL_RWops;


extern DECLSPEC SDL_RWops * SDLCALL SDL_RWFromFP(FILE *fp, int autoclose);
extern DECLSPEC SDL_RWops * SDLCALL SDL_RWFromFile(const char *file, const char *mode);
extern DECLSPEC SDL_RWops * SDLCALL SDL_RWFromMem(void *mem, int size);
extern DECLSPEC SDL_RWops * SDLCALL SDL_RWFromConstMem(const void *mem, int size);
extern DECLSPEC SDL_RWops * SDLCALL SDL_AllocRW(void);
extern DECLSPEC void SDLCALL SDL_FreeRW(SDL_RWops *area);

extern DECLSPEC Uint16 SDLCALL SDL_ReadLE16(SDL_RWops *src);
extern DECLSPEC Uint16 SDLCALL SDL_ReadBE16(SDL_RWops *src);
extern DECLSPEC Uint32 SDLCALL SDL_ReadLE32(SDL_RWops *src);
extern DECLSPEC Uint32 SDLCALL SDL_ReadBE32(SDL_RWops *src);
extern DECLSPEC Uint64 SDLCALL SDL_ReadLE64(SDL_RWops *src);
extern DECLSPEC Uint64 SDLCALL SDL_ReadBE64(SDL_RWops *src);

extern DECLSPEC int SDLCALL SDL_WriteLE16(SDL_RWops *dst, Uint16 value);
extern DECLSPEC int SDLCALL SDL_WriteBE16(SDL_RWops *dst, Uint16 value);
extern DECLSPEC int SDLCALL SDL_WriteLE32(SDL_RWops *dst, Uint32 value);
extern DECLSPEC int SDLCALL SDL_WriteBE32(SDL_RWops *dst, Uint32 value);
extern DECLSPEC int SDLCALL SDL_WriteLE64(SDL_RWops *dst, Uint64 value);
extern DECLSPEC int SDLCALL SDL_WriteBE64(SDL_RWops *dst, Uint64 value);

#define RW_SEEK_SET 0
#define RW_SEEK_CUR 1
#define RW_SEEK_END 2

#define SDL_RWseek(ctx, offset, whence) (ctx)->seek(ctx, offset, whence)
#define SDL_RWtell(ctx) (ctx)->seek(ctx, 0, RW_SEEK_CUR)
#define SDL_RWread(ctx, ptr, size, n) (ctx)->read(ctx, ptr, size, n)
#define SDL_RWwrite(ctx, ptr, size, n) (ctx)->write(ctx, ptr, size, n)
#define SDL_RWclose(ctx) (ctx)->close(ctx)

#include "close_code.h"

#endif

