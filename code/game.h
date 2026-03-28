#ifndef GAME_H
#define GAME_H

#include "defines.h"
#include "simple_math.h"
#include "physics2D.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800

#define PADDING_20 20

typedef struct
{
    // NOTE: Actual hardware texture
    SDL_Texture* SdlTexture;
    
    uint32 Width;
    uint32 Height;
}texture;

// TODO: SDL Indepent user input and output
typedef struct
{
    int XPos;
    int YPos;
    
    real32 ScrollWheel;
    
    // NOTE: For now we support 3 mouse buttons
    bool32 ButtonState[3];
}mouse;

typedef struct
{
    real32 Pan;
    real32 Zoom;
    
    real32 UpperLeftX;
    real32 UpperLeftY;
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
    
    bool32 Running;
}game;

#endif //GAME_H
