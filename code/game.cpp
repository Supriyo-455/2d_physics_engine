#include "game.h"

// NOTE: Including the cpp files for the function definitions
#include "timer.cpp"

SDL_Color ConvertToSDLColor(vec4 Color)
{
    SDL_Color SdlColor = {};
    
    SdlColor.r = (uint8) (Color.r * 255);
    SdlColor.g = (uint8) (Color.g * 255);
    SdlColor.b = (uint8) (Color.b * 255);
    SdlColor.a = (uint8) (Color.a * 255);
    
    return SdlColor;
}

// TODO: Generalize texture
texture RenderTextFromCenter(int XPos,
                             int YPos,
                             const char* TextureText,
                             SDL_Renderer* Renderer,
                             TTF_Font* Font,
                             vec4 TextColor,
                             SDL_Rect* Clip = NULL,
                             double Angle = 0.0,
                             SDL_Point* Center = NULL,
                             SDL_RendererFlip Flip = SDL_FLIP_NONE)
{
    
    texture Texture = {};
    
    SDL_Color SdlColor = ConvertToSDLColor(TextColor); 
    
    SDL_Surface* TextSurface = TTF_RenderText_Solid(Font, TextureText, SdlColor);
    if(TextSurface == NULL)
    {
        // TODO: Logging library
        fprintf(stderr,"Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    else
    {
        Texture.SdlTexture = SDL_CreateTextureFromSurface(Renderer, TextSurface);
        if(Texture.SdlTexture == NULL)
        {
            fprintf(stderr,"Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            Texture.Width = TextSurface->w;
            Texture.Height = TextSurface->h;
            
            SDL_Rect RenderQuad = {
                XPos - (int) Texture.Width / 2,
                YPos - (int) Texture.Height / 2,
                (int) Texture.Width,
                (int) Texture.Height
            };
            
            if(Clip != NULL)
            {
                RenderQuad.w = Clip->w;
                RenderQuad.h = Clip->h;
            }
            
            // NOTE: Finally render the text
            SDL_RenderCopyEx(Renderer,
                             Texture.SdlTexture,
                             Clip,
                             &RenderQuad,
                             Angle,
                             Center,
                             Flip);
        }
        
        SDL_DestroyTexture(Texture.SdlTexture);
        SDL_FreeSurface(TextSurface);
    }
    
    return Texture;
}

void RenderFilledRect(SDL_Renderer* Renderer, SDL_Rect* Rect, vec4 Color)
{
    SDL_Color SdlColor = ConvertToSDLColor(Color);
    SDL_SetRenderDrawColor(Renderer, SdlColor.r, SdlColor.g, SdlColor.b, SdlColor.a);
    SDL_RenderFillRect(Renderer, Rect);
}

void 
RenderHollowCircle(SDL_Renderer* Renderer, int32 CX, int32 CY, int32 Radius, vec4 Color)
{
    SDL_Color SdlColor = ConvertToSDLColor(Color);
    SDL_SetRenderDrawColor(Renderer, SdlColor.r, SdlColor.g, SdlColor.b, SdlColor.a);
    
    int32 X = 0;
    int32 Y = -Radius;
    int32 P = -Radius;
    
    while(X < -Y)
    {
        if(P > 0)
        {
            Y++;
            P += 2*(X + Y) + 1;
        }
        else
        {
            P += 2*X + 1;
        }
        
        SDL_RenderDrawPoint(Renderer, CX + X, CY + Y);
        SDL_RenderDrawPoint(Renderer, CX - X, CY + Y);
        
        SDL_RenderDrawPoint(Renderer, CX + X, CY - Y);
        SDL_RenderDrawPoint(Renderer, CX - X, CY - Y);
        
        SDL_RenderDrawPoint(Renderer, CX + Y, CY + X);
        SDL_RenderDrawPoint(Renderer, CX + Y, CY - X);
        
        SDL_RenderDrawPoint(Renderer, CX - Y, CY + X);
        SDL_RenderDrawPoint(Renderer, CX - Y, CY - X);
        
        X++;
    }
}

void 
RenderFilledCircle(SDL_Renderer* Renderer, int32 CX, int32 CY, int32 Radius, vec4 Color)
{
    SDL_Color SdlColor = ConvertToSDLColor(Color);
    SDL_SetRenderDrawColor(Renderer, SdlColor.r, SdlColor.g, SdlColor.b, SdlColor.a);
    
    int32 X = 0;
    int32 Y = -Radius;
    int32 P = -Radius;
    
    while(X < -Y)
    {
        if(P > 0)
        {
            Y++;
            P += 2*(X + Y) + 1;
        }
        else
        {
            P += 2*X + 1;
        }
        
        SDL_RenderDrawPoint(Renderer, CX - X, CY + Y);
        SDL_RenderDrawPoint(Renderer, CX + X, CY + Y);
        SDL_RenderDrawLine(Renderer, CX - X, CY + Y, CX + X, CY + Y);
        
        SDL_RenderDrawPoint(Renderer, CX + X, CY - Y);
        SDL_RenderDrawPoint(Renderer, CX - X, CY - Y);
        SDL_RenderDrawLine(Renderer, CX + X, CY - Y, CX - X, CY - Y);
        
        SDL_RenderDrawPoint(Renderer, CX + Y, CY + X);
        SDL_RenderDrawPoint(Renderer, CX - Y, CY + X);
        SDL_RenderDrawLine(Renderer, CX + Y, CY + X, CX - Y, CY + X);
        
        SDL_RenderDrawPoint(Renderer, CX + Y, CY - X);
        SDL_RenderDrawPoint(Renderer, CX - Y, CY - X);
        SDL_RenderDrawLine(Renderer, CX + Y, CY - X, CX - Y, CY - X);
        
        X++;
    }
}

void
RenderPhysicsBody(SDL_Renderer* Renderer, physics_body2D* Body, vec4 Color)
{
    switch(Body->Shape)
    {
        case CIRCLE:
        RenderFilledCircle(Renderer, (int) Body->Position.x, (int) Body->Position.y, (int) Body->Radius, Color);
        break;
        case BOX:
        break;
        default:
        break;
    }
}

bool32
InitializeGame(game* Game)
{
    bool32 success = true;
    
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        // TODO: Logging library
        fprintf(stderr,"SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        Game->Window = SDL_CreateWindow("Pong",
                                        SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,
                                        SCREEN_WIDTH,
                                        SCREEN_HEIGHT,
                                        SDL_WINDOW_SHOWN);
        
        if(Game->Window == NULL)
        {
            // TODO: Logging library
            fprintf(stderr,"Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            Game->Renderer = SDL_CreateRenderer(Game->Window,
                                                -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            
            if(Game->Renderer == NULL)
            {
                // TODO: Logging library
                fprintf(stderr,"Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                SDL_SetRenderDrawColor(Game->Renderer, 0, 0, 0, 0);
                
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags))
                {
                    // TODO: Logging library
                    fprintf(stderr,"SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                    return false;
                }
                
                //Initialize SDL_ttf
                if (TTF_Init() == -1)
                {
                    // TODO: Logging library
                    fprintf(stderr, "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
                    return false;
                }
                else
                {
                    Game->Font = TTF_OpenFont("assets/font/lazy.ttf", 28);
                    if (Game->Font == NULL)
                    {
                        // TODO: Logging library
                        fprintf(stderr, "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
                        return false;
                    }
                    
                    // NOTE: Initialize other parameters of game
                    Game->Mouse = {0};
                }
            }
        }
    }
    
    return success;
}

void
CloseGame(game* Game)
{
    SDL_DestroyWindow( Game->Window );
    Game->Window = NULL;
    
    SDL_Quit();
}

void
HandleInput(game* Game)
{
    while(SDL_PollEvent(&Game->Event) != 0)
    {
        if(Game->Event.type == SDL_QUIT)
        {
            Game->Running = false;
        }
        
        if(Game->Event.type == SDL_MOUSEMOTION || Game->Event.type == SDL_MOUSEBUTTONDOWN || Game->Event.type == SDL_MOUSEBUTTONUP)
        {
            SDL_GetMouseState(&Game->Mouse.XPos, &Game->Mouse.YPos);
        }
        
        if(Game->Event.type == SDL_KEYDOWN)
        {
            switch(Game->Event.key.keysym.sym)
            {
                case SDLK_r:
                {
                    break;
                }
                
                case SDLK_c:
                {
                    break;
                }
                
                case SDLK_SPACE:
                {
                    break;
                }
                
                default:
                {
                    break;
                }
            }
        }
    }
    
    Game->dx = 0.0f;
    Game->dy = 0.0f;
    
    const Uint8* KeyState = SDL_GetKeyboardState(NULL);
    
    // TODO: Movement code is not working properly!!!
    if(KeyState[SDL_SCANCODE_RIGHT]) Game->dx += 1.0f;
    if(KeyState[SDL_SCANCODE_LEFT])  Game->dx += -1.0f;
    if(KeyState[SDL_SCANCODE_UP])    Game->dy += -1.0f;
    if(KeyState[SDL_SCANCODE_DOWN])  Game->dy += 1.0f;
}

void
ClearRenderer(SDL_Renderer* Renderer, vec4 Color)
{
    SDL_Color SdlColor = ConvertToSDLColor(Color);
    SDL_SetRenderDrawColor(Renderer, SdlColor.r, SdlColor.g, SdlColor.b, SdlColor.a);
    SDL_RenderClear(Renderer);
}

int
main(int argc, char* args[])
{
    game Game = {};
    
    if(!InitializeGame(&Game))
    {
        // TODO: Logging library
        fprintf(stderr, "Failed to initialize!\n");
        return -1;
    }
    else
    {
        Game.Running = true;
        Game.dx = 0.0f;
        Game.dy = 0.0f;
        Game.Speed = 100.0f;
        
        vec4 WHITE = {
            .r=1.0f,
            .g=1.0f,
            .b=1.0f,
            .a=1.0f
        };
        
        vec4 BLACK = {
            .r=0.0f,
            .g=0.0f,
            .b=0.0f,
            .a=0.0f
        };
        
        physics_body2D Circles[20] = {0};
        for(int i=0; i<20; i++)
        {
            int X = (int) (RandomUnilateral() * SCREEN_WIDTH);
            int Y = (int) (RandomUnilateral() * SCREEN_HEIGHT);
            
            real32 Radius = 20.0f;
            
            Circles[i] = CreateCirclePhysicsBody2D(vec((real32) X, (real32) Y),
                                                   Radius,
                                                   0.60f,
                                                   0.0f,
                                                   false);
        }
        
        timer FPSTimer = {};
        TimerStart(&FPSTimer);
        
        uint32 LastFrameTicks = TimerGetTicks(&FPSTimer);
        uint32 CurrentFrameTicks = TimerGetTicks(&FPSTimer);
        uint32 DeltaTicks = CurrentFrameTicks - LastFrameTicks;
        
        while(Game.Running)
        {
            uint32 CurrentFrameTicks = SDL_GetTicks();
            DeltaTicks = CurrentFrameTicks - LastFrameTicks;
            LastFrameTicks = CurrentFrameTicks;
            
            HandleInput(&Game);
            
            real32 DeltaTimeSeconds = DeltaTicks / 1000.0f;
            
            if(Game.dx != 0.0f || Game.dy != 0.0f)
            {            
                vec2 Direction = Normalize(vec(Game.dx, Game.dy));
                vec2 Velocity = (Direction * Game.Speed) * DeltaTimeSeconds ;
                Circles[0].LinearVelocity = Velocity;
            }
            else
            {
                Circles[0].LinearVelocity = vec(0.0f, 0.0f);
            }
            
            for(int i = 0; i < ARRAY_COUNT(Circles); i++)
            {
                MovePhysicsBodyByLinearVelocity(&Circles[i]);
            }
            
            Update2DPhysicsBodies(Circles, ARRAY_COUNT(Circles));
            
            ClearRenderer(Game.Renderer, BLACK);
            
            for(int i=0; i<ARRAY_COUNT(Circles); i++)
            {
                RenderPhysicsBody(Game.Renderer, &Circles[i], WHITE);
            }
            
            // NOTE: FPS Display
            {
                uint32 FPS = 0;
                if (DeltaTicks > 0)
                {
                    FPS = 1000 / DeltaTicks;
                }
                
                char buf[256] = {};
                sprintf_s(buf, "FPS: %d", FPS);
                RenderTextFromCenter(SCREEN_WIDTH / 2,
                                     SCREEN_HEIGHT - PADDING_20,
                                     buf,
                                     Game.Renderer,
                                     Game.Font,
                                     WHITE);
            }
            
            SDL_RenderPresent(Game.Renderer);
        }
    }
    
    CloseGame(&Game);
    
    return 0;
}
