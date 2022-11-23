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

#ifndef _SDL_audio_h
#define _SDL_audio_h

#include "SDL_stdinc.h"
#include "SDL_error.h"
#include "SDL_endian.h"
#include "SDL_mutex.h"
#include "SDL_thread.h"
#include "SDL_rwops.h"

#include "begin_code.h"

typedef struct SDL_AudioSpec {
    int freq;
    Uint16 format;
    Uint8  channels;
    Uint8  silence;
    Uint16 samples;
    Uint16 padding;
    Uint32 size;
    void (SDLCALL *callback)(void *userdata, Uint8 *stream, int len);
    void  *userdata;
} SDL_AudioSpec;

#define AUDIO_U8 0x0008
#define AUDIO_S8 0x8008
#define AUDIO_U16LSB 0x0010
#define AUDIO_S16LSB 0x8010
#define AUDIO_U16MSB 0x1010
#define AUDIO_S16MSB 0x9010
#define AUDIO_U16 AUDIO_U16LSB
#define AUDIO_S16 AUDIO_S16LSB

#if SDL_BYTEORDER == SDL_LIL_ENDIAN
#define AUDIO_U16SYS AUDIO_U16LSB
#define AUDIO_S16SYS AUDIO_S16LSB
#else
#define AUDIO_U16SYS AUDIO_U16MSB
#define AUDIO_S16SYS AUDIO_S16MSB
#endif

typedef struct SDL_AudioCVT
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
    void (SDLCALL *filters[10])(struct SDL_AudioCVT *cvt, Uint16 format);
    int filter_index;
} SDL_AudioCVT;

typedef enum SDL_audiostatus
{
    SDL_AUDIO_STOPPED,
    SDL_AUDIO_PLAYING,
    SDL_AUDIO_PAUSED
} SDL_audiostatus;

#define SDL_MIX_MAXVOLUME 128

extern DECLSPEC int SDLCALL SDL_AudioInit(const char *driver_name);
extern DECLSPEC void SDLCALL SDL_AudioQuit(void);
extern DECLSPEC char * SDLCALL SDL_AudioDriverName(char *namebuf, int maxlen);
extern DECLSPEC int SDLCALL SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained);
extern DECLSPEC void SDLCALL SDL_PauseAudio(int pause_on);
extern DECLSPEC void SDLCALL SDL_CloseAudio(void);
extern DECLSPEC SDL_audiostatus SDLCALL SDL_GetAudioStatus(void);
extern DECLSPEC void SDLCALL SDL_LockAudio(void);
extern DECLSPEC void SDLCALL SDL_UnlockAudio(void);
extern DECLSPEC SDL_AudioSpec * SDLCALL SDL_LoadWAV_RW(SDL_RWops *src, int freesrc, SDL_AudioSpec *spec, Uint8 **audio_buf, Uint32 *audio_len);
extern DECLSPEC void SDLCALL SDL_FreeWAV(Uint8 *audio_buf);
extern DECLSPEC int SDLCALL SDL_BuildAudioCVT(SDL_AudioCVT *cvt, Uint16 src_format, Uint8 src_channels, int src_rate, Uint16 dst_format, Uint8 dst_channels, int dst_rate);
extern DECLSPEC int SDLCALL SDL_ConvertAudio(SDL_AudioCVT *cvt);
extern DECLSPEC void SDLCALL SDL_MixAudio(Uint8 *dst, const Uint8 *src, Uint32 len, int volume);

#define SDL_LoadWAV(file, spec, audio_buf, audio_len) SDL_LoadWAV_RW(SDL_RWFromFile(file, "rb"),1, spec,audio_buf,audio_len)

#include "close_code.h"

#endif
