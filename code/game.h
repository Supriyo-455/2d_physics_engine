#ifndef GAME_H
#define GAME_H

#include "defines.h"
#include "simple_math.h"
#include "physics2D.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define PADDING_20 20

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

struct
texture
{
    // NOTE: Actual hardware texture
    SDL_Texture* SdlTexture;
    
    uint32 Width;
    uint32 Height;
};

// TODO: SDL Indepent user input and output
struct
mouse
{
    int XPos;
    int YPos;
    
    real32 ScrollWheel;
    
    // NOTE: For now we support 3 mouse buttons
    bool32 ButtonState[3];
};

typedef struct
{
    real32 Pan;
    real32 Zoom;
    
    real32 Width;
    real32 Height;
    
    vec2 Position;
}simple_camera;

typedef struct
{
    SDL_Window* Window;
    SDL_Renderer* Renderer;
    TTF_Font* Font;
    SDL_Event Event;
    
    simple_camera Camera;
    
    real32 dx;
    real32 dy;
    real32 Speed;
    real32 RotationalVelocity;
    real32 ForceMagnitude;
    
	physics_world2D* World;
	
    bool32 Running;
}game;

#endif //GAME_H
