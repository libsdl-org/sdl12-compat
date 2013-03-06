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

/* don't #pragma once this file, it gets #included multiple times! */

SDL20_SYM(int,Init,(Uint32 a),(a),return)
SDL20_SYM(char*,GetError,(void),(),return)
/*SDL20_SYM_PASSTHROUGH(void,SetError,(const char *fmt, ...);*/
SDL20_SYM_PASSTHROUGH(void,ClearError,(void),(),)
SDL20_SYM(void,Quit,(void),(),)

SDL20_SYM_PASSTHROUGH(Uint32,GetThreadID,(SDL_Thread *a),(a),return)
SDL20_SYM_PASSTHROUGH(Uint32,ThreadID,(void),(),return)
SDL20_SYM_PASSTHROUGH(void,WaitThread,(SDL_Thread *a, int *b),(a,b),)

SDL20_SYM_PASSTHROUGH(SDL_mutex*,CreateMutex,(void),(),return)
SDL20_SYM_PASSTHROUGH(int,mutexP,(SDL_mutex *a),(a),return)
SDL20_SYM_PASSTHROUGH(int,mutexV,(SDL_mutex *a),(a),return)
SDL20_SYM_PASSTHROUGH(void,DestroyMutex,(SDL_mutex *a),(a),)
SDL20_SYM_PASSTHROUGH(SDL_sem*,CreateSemaphore,(Uint32 a),(a),return)
SDL20_SYM_PASSTHROUGH(void,DestroySemaphore,(SDL_sem *a),(a),)
SDL20_SYM_PASSTHROUGH(int,SemWait,(SDL_sem *a),(a),return)
SDL20_SYM_PASSTHROUGH(int,SemTryWait,(SDL_sem *a),(a),return)
SDL20_SYM_PASSTHROUGH(int,SemWaitTimeout,(SDL_sem *a, Uint32 b),(a,b),return)
SDL20_SYM_PASSTHROUGH(int,SemPost,(SDL_sem *a),(a),return)
SDL20_SYM_PASSTHROUGH(Uint32,SemValue,(SDL_sem *a),(a),return)
SDL20_SYM_PASSTHROUGH(SDL_cond*,CreateCond,(void),(),return)
SDL20_SYM_PASSTHROUGH(void,DestroyCond,(SDL_cond *a),(a),)
SDL20_SYM_PASSTHROUGH(int,CondSignal,(SDL_cond *a),(a),return)
SDL20_SYM_PASSTHROUGH(int,CondBroadcast,(SDL_cond *a),(a),return)
SDL20_SYM_PASSTHROUGH(int,CondWait,(SDL_cond *a, SDL_mutex *b),(a,b),return)
SDL20_SYM_PASSTHROUGH(int,CondWaitTimeout,(SDL_cond *a, SDL_mutex *b, Uint32 c),(a,b,c),return)

SDL20_SYM_PASSTHROUGH(int,OpenAudio,(SDL_AudioSpec *a, SDL_AudioSpec *b),(a,b),return)
SDL20_SYM_PASSTHROUGH(SDL_audiostatus,GetAudioStatus,(void),(),return)
SDL20_SYM_PASSTHROUGH(void,PauseAudio,(int a),(a),)
SDL20_SYM_PASSTHROUGH(void,FreeWAV,(Uint8 *a),(a),)
SDL20_SYM_PASSTHROUGH(int,BuildAudioCVT,(SDL_AudioCVT *a, Uint16 b, Uint8 c, int d, Uint16 e, Uint8 f, int g),(a,b,c,d,e,f,g),return)
SDL20_SYM_PASSTHROUGH(int,ConvertAudio,(SDL_AudioCVT *a),(a),return)
SDL20_SYM_PASSTHROUGH(void,MixAudio,(Uint8 *a, const Uint8 *b, Uint32 c, int d),(a,b,c,d),)
SDL20_SYM_PASSTHROUGH(void,LockAudio,(void),(),)
SDL20_SYM_PASSTHROUGH(void,UnlockAudio,(void),(),)
SDL20_SYM_PASSTHROUGH(void,CloseAudio,(void),(),)

SDL20_SYM_PASSTHROUGH(void*,LoadObject,(const char *a),(a),return)
SDL20_SYM_PASSTHROUGH(void*,LoadFunction,(void *a, const char *b),(a,b),return)
SDL20_SYM_PASSTHROUGH(void,UnloadObject,(void *a),(a),)

SDL20_SYM_PASSTHROUGH(SDL_bool,HasRDTSC,(void),(),return)
SDL20_SYM_PASSTHROUGH(SDL_bool,HasMMX,(void),(),return)
SDL20_SYM_PASSTHROUGH(SDL_bool,HasMMXExt,(void),(),return)
SDL20_SYM_PASSTHROUGH(SDL_bool,Has3DNow,(void),(),return)
SDL20_SYM_PASSTHROUGH(SDL_bool,Has3DNowExt,(void),(),return)
SDL20_SYM_PASSTHROUGH(SDL_bool,HasSSE,(void),(),return)
SDL20_SYM_PASSTHROUGH(SDL_bool,HasSSE2,(void),(),return)
SDL20_SYM_PASSTHROUGH(SDL_bool,HasAltiVec,(void),(),return)

SDL20_SYM_PASSTHROUGH(Uint32,GetTicks,(void),(),return)
SDL20_SYM_PASSTHROUGH(void,Delay,(Uint32 a),(a),)

SDL20_SYM_PASSTHROUGH(int,NumJoysticks,(void),(),return)
SDL20_SYM_PASSTHROUGH(const char *,JoystickName,(int a),(a),return)
SDL20_SYM_PASSTHROUGH(SDL_Joystick *,JoystickOpen,(int a),(a),return)
SDL20_SYM_PASSTHROUGH(int,JoystickOpened,(int a),(a),return)
SDL20_SYM_PASSTHROUGH(int,JoystickIndex,(SDL_Joystick *a),(a),return)
SDL20_SYM_PASSTHROUGH(int,JoystickNumAxes,(SDL_Joystick *a),(a),return)
SDL20_SYM_PASSTHROUGH(int,JoystickNumBalls,(SDL_Joystick *a),(a),return)
SDL20_SYM_PASSTHROUGH(int,JoystickNumHats,(SDL_Joystick *a),(a),return)
SDL20_SYM_PASSTHROUGH(int,JoystickNumButtons,(SDL_Joystick *a),(a),return)
SDL20_SYM_PASSTHROUGH(void,JoystickUpdate,(void),(),)
SDL20_SYM_PASSTHROUGH(int,JoystickEventState,(int a),(a),return)
SDL20_SYM_PASSTHROUGH(Sint16,JoystickGetAxis,(SDL_Joystick *a, int b),(a,b),return)
SDL20_SYM_PASSTHROUGH(Uint8,JoystickGetHat,(SDL_Joystick *a, int b),(a,b),return)
SDL20_SYM_PASSTHROUGH(int,JoystickGetBall,(SDL_Joystick *a, int b, int *c, int *d),(a,b,c,d),return)
SDL20_SYM_PASSTHROUGH(Uint8,JoystickGetButton,(SDL_Joystick *a, int b),(a,b),return)
SDL20_SYM_PASSTHROUGH(void,JoystickClose,(SDL_Joystick *a),(a),return)


/* vi: set ts=4 sw=4 expandtab: */

