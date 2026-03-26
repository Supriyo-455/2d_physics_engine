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
    SDL_Window* Window;
    SDL_Renderer* Renderer;
    TTF_Font* Font;
    SDL_Event Event;
    
    real32 dx;
    real32 dy;
    real32 Speed;
    
    mouse Mouse;
    bool32 Running;
}game;

#endif //GAME_H
