/* date = March 24th 2026 1:44 pm */

#ifndef TIMER_H
#define TIMER_H

#include "defines.h"

struct timer
{
    uint32 StartTicks;
    uint32 PausedTicks;
	
    bool32 Paused;
    bool32 Started;
};

struct fpsTimer
{
	timer Timer;
	
	uint32 LastFrameTicks;
	uint32 CurrentFrameTicks;
	uint32 DeltaTicks;
};

#endif //TIMER_H
