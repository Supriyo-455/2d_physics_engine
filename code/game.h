#ifndef GAME_H
#define GAME_H

#include "defines.h"
#include "simple_math.h"
#include "memory.h"
#include "transform_2d.h"
#include "physics_body_2d.h"
#include "collision_2d.h"
#include "physics_2d.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"

struct game
{
    memory_arena* MemoryArena;
	uint64 MemoryArenaSize;
	
	SDL_Window* Window;
    SDL_Renderer* Renderer;
    TTF_Font* Font;
    SDL_Event Event;
	SDL_GLContext GLContext;
	
    simple_camera Camera;
    
    real32 dx;
    real32 dy;
    real32 Speed;
    real32 RotationalVelocity;
    real32 ForceMagnitude;
    
	physics_world2D* World;
	
	vec4* BodyColors;
	uint32 BodyColorsCount;
	
	bool32 HighlightCollisions;
	
    bool32 Running;
};

// NOTE: Unity Build dont re-arrange these files
#include "memory.cpp"
#include "transform_2d.cpp"
#include "physics_body_2d.cpp"
#include "collision_2d.cpp"
#include "renderer.cpp"
#include "camera.cpp"
#include "input.cpp"
#include "timer.cpp"


#endif //GAME_H
