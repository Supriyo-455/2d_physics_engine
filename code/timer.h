/* date = March 24th 2026 1:44 pm */

#ifndef TIMER_H
#define TIMER_H

#include "defines.h"

typedef struct
{
    uint32 StartTicks;
    uint32 PausedTicks;
    
    bool32 Paused;
    bool32 Started;
}timer;

#endif //TIMER_H
