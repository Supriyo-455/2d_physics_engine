#include "timer.h"

void 
TimerStart(timer* Timer)
{
    Timer->Started = true;
    Timer->Paused = false;
    
    Timer->StartTicks = SDL_GetTicks();
    Timer->PausedTicks = 0;
}

void 
TimerEnd(timer* Timer)
{
    Timer->Started = false;
    Timer->Paused = false;
    
    Timer->StartTicks = 0;
    Timer->PausedTicks = 0;
}

void 
TimerPause(timer* Timer)
{
    if(!Timer->Paused && Timer->Started)
    {
        Timer->Paused = true;
        
        Timer->PausedTicks = SDL_GetTicks() - Timer->StartTicks;
        Timer->StartTicks = 0;
    }
}

void 
TimerUnPause(timer* Timer)
{
    if(Timer->Paused && Timer->Started)
    {
        Timer->Paused = false;
        
        Timer->StartTicks = SDL_GetTicks() - Timer->PausedTicks;
        Timer->PausedTicks = 0;
    }
}

uint32
TimerGetTicks(timer* Timer)
{
    uint32 Time = 0;
    
    if(Timer->Started)
    {
        if(Timer->Paused)
        {
            Time = Timer->PausedTicks;
        }
        else
        {
            Time = SDL_GetTicks() - Timer->StartTicks; 
        }
    }
    
    return Time;
}

void 
FPSTimerInit(fpsTimer* FPSTimer)
{
	FPSTimer->Timer = {};
	TimerStart(&FPSTimer->Timer);
	
	// TODO: Compress this into the fps timer struct
	FPSTimer->LastFrameTicks = TimerGetTicks(&FPSTimer->Timer);
	FPSTimer->CurrentFrameTicks = TimerGetTicks(&FPSTimer->Timer);
	FPSTimer->DeltaTicks = FPSTimer->CurrentFrameTicks - FPSTimer->LastFrameTicks;
}

void
FPSTimerUpdate(fpsTimer* FPSTimer)
{
	FPSTimer->CurrentFrameTicks = TimerGetTicks(&FPSTimer->Timer);
	FPSTimer->DeltaTicks = FPSTimer->CurrentFrameTicks - FPSTimer->LastFrameTicks;
	FPSTimer->LastFrameTicks = FPSTimer->CurrentFrameTicks;
}

uint32
GetFPS(fpsTimer* FPSTimer)
{
	uint32 FPS = 0;
	if (FPSTimer->DeltaTicks > 0)
	{
		FPS = 1000 / FPSTimer->DeltaTicks;
	}
	return FPS;
}