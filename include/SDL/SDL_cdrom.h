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

#ifndef _SDL_cdrom_h
#define _SDL_cdrom_h

#include "SDL_stdinc.h"
#include "SDL_error.h"

#include "begin_code.h"

#define SDL_MAX_TRACKS 99

typedef enum CDstatus
{
    CD_TRAYEMPTY,
    CD_STOPPED,
    CD_PLAYING,
    CD_PAUSED,
    CD_ERROR = -1
} CDstatus;

typedef struct SDL_CDtrack
{
    Uint8 id;
    Uint8 type;
    Uint16 unused;
    Uint32 length;
    Uint32 offset;
} SDL_CDtrack;

typedef struct SDL_CD
{
    int id;
    CDstatus status;
    int numtracks;
    int cur_track;
    int cur_frame;
    SDL_CDtrack track[SDL_MAX_TRACKS+1];
} SDL_CD;

#define SDL_AUDIO_TRACK 0x00
#define SDL_DATA_TRACK 0x04
#define CD_INDRIVE(status) (status > CD_TRAYEMPTY)
#define CD_FPS 75

#define FRAMES_TO_MSF(f, M,S,F) { \
    int value = f; \
    *(F) = value % CD_FPS; \
    value /= CD_FPS; \
    *(S) = value % 60; \
    value /= 60; \
    *(M) = value; \
}

#define MSF_TO_FRAMES(M, S, F) ((M) * 60 * CD_FPS + (S) * CD_FPS + (F))

extern DECLSPEC int SDLCALL SDL_CDNumDrives(void);
extern DECLSPEC const char * SDLCALL SDL_CDName(int drive);
extern DECLSPEC SDL_CD * SDLCALL SDL_CDOpen(int drive);
extern DECLSPEC CDstatus SDLCALL SDL_CDStatus(SDL_CD *cdrom);
extern DECLSPEC int SDLCALL SDL_CDPlayTracks(SDL_CD *cdrom, int start_track, int start_frame, int ntracks, int nframes);
extern DECLSPEC int SDLCALL SDL_CDPlay(SDL_CD *cdrom, int start, int length);
extern DECLSPEC int SDLCALL SDL_CDPause(SDL_CD *cdrom);
extern DECLSPEC int SDLCALL SDL_CDResume(SDL_CD *cdrom);
extern DECLSPEC int SDLCALL SDL_CDStop(SDL_CD *cdrom);
extern DECLSPEC int SDLCALL SDL_CDEject(SDL_CD *cdrom);
extern DECLSPEC void SDLCALL SDL_CDClose(SDL_CD *cdrom);

#include "close_code.h"

#endif

