#ifndef GAME_H
#define GAME_H

#include "game_platform.h"
#include "memory.h"
#include "transform_2d.h"
#include "physics_body_2d.h"
#include "collision_2d.h"
#include "physics_2d.h"
#include "camera.h"
#include "performance.h"

global vec4 WHITE = {
	.r=1.0f,
	.g=1.0f,
	.b=1.0f,
	.a=1.0f
};

global vec4 GRAY = {
	.r=0.5f,
	.g=0.5f,
	.b=0.5f,
	.a=1.0f
};

global vec4 BLACK = {
	.r=0.0f,
	.g=0.0f,
	.b=0.0f,
	.a=1.0f
};

global vec4 RED = {
	.r=1.0f,
	.g=0.0f,
	.b=0.0f,
	.a=1.0f
};

global vec4 YELLOW = {
	.r=1.0f,
	.g=1.0f,
	.b=0.0f,
	.a=1.0f
};

struct game_state
{
    memory_arena WorldArena;
    
    physics_world2D* World;
    simple_camera Camera;
    
    // TODO: Why using body colors struct separatly? Can we create entities instead?
    vec4* BodyColors;
    uint32 BodyColorsCount;
    
    bool32 HighlightCollisions;
    bool32 WireframeMode;
	
	profiler_mode ProfilerMode;
};

#endif //GAME_H