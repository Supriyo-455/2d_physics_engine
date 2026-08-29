#include "game.h"

/* TODO:
   1) Camera - (Simple camera done, Need more advanced camera with actual frustum culling)
   2) Sound - (No sound features yet)
   3) Opengl - (As soon as the basic 2d physics engine completed, need to work on opengl 
   rendering)
   4) Dynamic arrays
   5) Memory Arena
*/

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

// TODO: Need to replace this cpu based calls with Opengl
// TODO: Generalize texture
// TODO: Texture Scalling is not optimized
void RenderTextFromCenter(int XPos,
                          int YPos,
                          const char* TextureText,
                          SDL_Renderer* Renderer,
                          TTF_Font* Font,
                          vec4 TextColor,
                          int FontSize = 25,
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
        LOG_ERROR("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
    }
    else
    {
        Texture.SdlTexture = SDL_CreateTextureFromSurface(Renderer, TextSurface);
        if(Texture.SdlTexture == NULL)
        {
            LOG_ERROR("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            Texture.Width = TextSurface->w;
            Texture.Height = TextSurface->h;
            
            Assert(Texture.Height > 0);
            
            real32 AspectRatio = (real32)Texture.Width / (real32)Texture.Height;
            
            SDL_Rect RenderQuad = {
                XPos - (int)(AspectRatio * FontSize) / 2,
                YPos - FontSize / 2,
                (int)(AspectRatio * FontSize),
                FontSize
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
}

// TODO: Need to replace this cpu based calls with Opengl
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

bool32 
IsVisible(real32 ObjectX, real32 ObjectY,
          real32 ObjectWidth, real32 ObjectHeight,
          simple_camera* Camera) {
    
    real32 ObjectLeft = ObjectX - (ObjectWidth / 2.0f); 
    real32 ObjectRight = ObjectLeft + ObjectWidth;
    real32 ObjectTop = ObjectY - (ObjectHeight / 2.0f);
    real32 ObjectBottom = ObjectTop + ObjectHeight;
    
    real32 ViewPortWidth = Camera->Width / Camera->Zoom;
    real32 ViewPortHeight = Camera->Height / Camera->Zoom;
    real32 ViewPortLeft = Camera->Position.x - (ViewPortWidth / 2.0f);
    real32 ViewPortRight = ViewPortLeft + ViewPortWidth;
    real32 ViewPortTop = Camera->Position.y - (ViewPortHeight / 2.0f);
    real32 ViewPortBottom = ViewPortTop + ViewPortHeight;
    
    return (ObjectRight > ViewPortLeft &&
            ObjectBottom > ViewPortTop &&
            ObjectLeft < ViewPortRight &&
            ObjectTop < ViewPortBottom);
}

// TODO: Need to replace this cpu based calls with Opengl
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
RenderPhysicsBody(simple_camera* Camera, SDL_Renderer* Renderer, physics_body2D* Body, vec4 FillColor, vec4 BorderColor)
{
    switch(Body->Shape)
    {
        case CIRCLE:
        {
            uint32 X = RoundReal32ToUint32((Body->Position.x - Camera->Position.x) * Camera->Zoom + (Camera->Width / 2.0f));
            uint32 Y = RoundReal32ToUint32((Body->Position.y - Camera->Position.y) * Camera->Zoom + (Camera->Height / 2.0f));
            
            uint32 Radius = RoundReal32ToUint32(Body->Radius * Camera->Zoom);
            
            RenderFilledCircle(Renderer, X, Y, Radius, FillColor);
            
            if(Body->IsCollided)
                BorderColor = vec(1.0f, 0.0f, 0.0f, 1.0f);
            
            RenderHollowCircle(Renderer, X, Y, Radius, BorderColor);
            
            // TODO: This is ugly and hacky, no actual rotation of circle
            SDL_Color Border = ConvertToSDLColor(BorderColor);
            SDL_SetRenderDrawColor(Renderer, Border.r, Border.g, Border.b, Border.a);
            
            real32 RCos = X  + Radius * cosf(Body->Rotation);
            real32 RSin = Y  + Radius * sinf(Body->Rotation);
            
            SDL_RenderDrawLineF(Renderer,
                                (real32) X, (real32) Y,
                                RCos, RSin);
        }
        break;
        case BOX:
        {
            vec2* WorldVerts = GetPhysicsBodyTransformedVertices(Body);
            
            SDL_Vertex SdlVerts[4] = {};
            SDL_Color Fill = ConvertToSDLColor(FillColor);
            
            for(int i = 0; i < 4; i++)
            {
                SdlVerts[i].position.x = (WorldVerts[i].x - Camera->Position.x) * Camera->Zoom + (Camera->Width / 2.0f);
                
                SdlVerts[i].position.y = (WorldVerts[i].y - Camera->Position.y) * Camera->Zoom + (Camera->Height / 2.0f);
                
                SdlVerts[i].color = Fill;
            }
            
            SDL_RenderGeometry(Renderer, NULL, SdlVerts, 4, Body->Triangles, 6);
            
            
            if(Body->IsCollided)
                BorderColor = vec(1.0f, 0.0f, 0.0f, 1.0f);
            
            SDL_Color Border = ConvertToSDLColor(BorderColor);
            SDL_SetRenderDrawColor(Renderer, Border.r, Border.g, Border.b, Border.a);
            
            for(int i = 0; i < 4; i++)
            {
                int Next = (i + 1) % 4;
                SDL_RenderDrawLineF(Renderer,
                                    SdlVerts[i].position.x, SdlVerts[i].position.y,
                                    SdlVerts[Next].position.x, SdlVerts[Next].position.y);
            }
        }
        break;
        default:
	    break;
    }
}

void 
GenerateRandomColor(vec4* ColorArray, int ArrayCount)
{
    for(int i=0; i<ArrayCount; i++)
    {
        float R = RandomUnilateral();
        float G = RandomUnilateral();
        float B = RandomUnilateral();
        ColorArray[i] = vec(R, G, B, 1.0f);
    }
}

bool32
InitializeGame(game* Game)
{
    bool32 success = true;
    
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        LOG_ERROR("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        Game->Window = SDL_CreateWindow("2D Physics Engine",
                                        SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,
                                        SCREEN_WIDTH,
                                        SCREEN_HEIGHT,
                                        SDL_WINDOW_SHOWN);
        
        if(Game->Window == NULL)
        {
            LOG_ERROR("Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            Game->Renderer = SDL_CreateRenderer(Game->Window,
                                                -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            
            if(Game->Renderer == NULL)
            {
                LOG_ERROR("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                SDL_SetRenderDrawColor(Game->Renderer, 0, 0, 0, 0);
                
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags))
                {
                    LOG_ERROR("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                    return false;
                }
                
                // NOTE:Initialize SDL_ttf
                if (TTF_Init() == -1)
                {
                    LOG_ERROR("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
                    return false;
                }
                else
                {
                    Game->Font = TTF_OpenFont("assets/font/lazy.ttf", 28);
                    if (Game->Font == NULL)
                    {
                        LOG_ERROR("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
                        return false;
                    }
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
PanCamera(simple_camera* Camera, real32 NewX, real32 NewY)
{
    real32 NewCamPositionX = Camera->Position.x - (NewX / Camera->Zoom);
    real32 NewCamPositionY = Camera->Position.y - (NewY / Camera->Zoom);
    
    Camera->Position = vec(NewCamPositionX, NewCamPositionY);
}

// TODO: Need to set bounds for zoom
void 
ZoomCamera(simple_camera* Camera, real32 ZoomXPos, real32 ZoomYPos, int32 ZoomWeight)
{
    real32 OffsetX = ZoomXPos - (Camera->Width / 2.0f);
    real32 OffsetY = ZoomYPos - (Camera->Height / 2.0f);
    
    real32 WorldXBefore = Camera->Position.x + (OffsetX / Camera->Zoom);
    real32 WorldYBefore = Camera->Position.y + (OffsetY / Camera->Zoom);
    
    if(ZoomWeight > 0)
        Camera->Zoom *= 1.1f;
    else
        Camera->Zoom /= 1.1f;
    
    Camera->Position.x = WorldXBefore - (OffsetX / Camera->Zoom);
    Camera->Position.y = WorldYBefore - (OffsetY / Camera->Zoom);
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
        
        if(Game->Event.type == SDL_MOUSEMOTION && 
           (Game->Event.motion.state & SDL_BUTTON_RMASK))
        {
            PanCamera(&Game->Camera, 
                      (real32)Game->Event.motion.xrel, 
                      (real32)Game->Event.motion.yrel);
        }
        
        if(Game->Event.type == SDL_MOUSEWHEEL)
        {
            int MouseX, MouseY;
            SDL_GetMouseState(&MouseX, &MouseY);
            
            ZoomCamera(&Game->Camera, (real32)MouseX, (real32)MouseY, Game->Event.wheel.y);
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
                    LOG_INFO("Space pressed!!\n");
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
    Game->RotationalVelocity = 0.0f;
    
    const Uint8* KeyState = SDL_GetKeyboardState(NULL);
    
    // TODO: Movement code is not working properly!!!
    if(KeyState[SDL_SCANCODE_RIGHT]) Game->dx += 1.0f;
    if(KeyState[SDL_SCANCODE_LEFT])  Game->dx += -1.0f;
    if(KeyState[SDL_SCANCODE_UP])    Game->dy += -1.0f;
    if(KeyState[SDL_SCANCODE_DOWN])  Game->dy += 1.0f;
    
    if(KeyState[SDL_SCANCODE_R]) 
        Game->RotationalVelocity = 1.0f;
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
        LOG_ERROR("Failed to initialize!\n");
        return -1;
    }
    else
    {
        InitializeRandomNumbers();
        
        Game.Running = true;
        Game.dx = 0.0f;
        Game.dy = 0.0f;
        Game.Speed = 100.0f;
        Game.ForceMagnitude = 10.0f;
        
        Game.Camera = {};
        Game.Camera.Zoom = 1.0f;
        Game.Camera.Pan = 0.0f;
        Game.Camera.Width = SCREEN_WIDTH;
        Game.Camera.Height = SCREEN_HEIGHT;
        Game.Camera.Position.x = SCREEN_WIDTH / 2.0f;
        Game.Camera.Position.y = SCREEN_HEIGHT / 2.0f;
        
        vec4 WHITE = {
            .r=1.0f,
            .g=1.0f,
            .b=1.0f,
            .a=1.0f
        };
        
        vec4 GRAY = {
            .r=0.5f,
            .g=0.5f,
            .b=0.5f,
            .a=1.0f
        };
        
        vec4 BLACK = {
            .r=0.0f,
            .g=0.0f,
            .b=0.0f,
            .a=0.0f
        };
        
        physics_world2D World = {0};
        World.BodyCount = PHYSICS_BODY_COUNT;
        
        vec4 Colors[PHYSICS_BODY_COUNT] = {0};
        GenerateRandomColor(Colors, PHYSICS_BODY_COUNT);
        
        physics_body2D Bodies[PHYSICS_BODY_COUNT] = {0};
        for(int i=0; 
            i<PHYSICS_BODY_COUNT; 
            i+=2)
        {
            int BoxIndex = i;
            int CircleIndex = i + 1;
            
            real32 X = RandomUnilateral() * SCREEN_WIDTH;
            real32 Y = RandomUnilateral() * SCREEN_HEIGHT;
            
            real32 Radius = 10.0f;
            
            Bodies[CircleIndex] = CreateCirclePhysicsBody2D(&World,
                                                            vec(X, Y),
                                                            Radius,
                                                            0.5f,
                                                            0.0f,
                                                            false);
            
            X = RandomUnilateral() * SCREEN_WIDTH;
            Y = RandomUnilateral() * SCREEN_HEIGHT;
            
            real32 Width = 20.0f;
            real32 Height = 20.0f;
            
            Bodies[BoxIndex] = CreateBoxPhysicsBody2D(&World,
                                                      vec(X, Y),
                                                      Width,
                                                      Height,
                                                      0.5f,
                                                      0.0f,
                                                      false);
        }
        World.Bodies = Bodies;
        
        timer FPSTimer = {};
        TimerStart(&FPSTimer);
        
        // TODO: Compress this into the fps timer struct
        uint32 LastFrameTicks = TimerGetTicks(&FPSTimer);
        uint32 CurrentFrameTicks = TimerGetTicks(&FPSTimer);
        uint32 DeltaTicks = CurrentFrameTicks - LastFrameTicks;
        real32 DeltaTimeSeconds = DeltaTicks / 1000.0f;
        real32 CurrentFrameTicksSeconds = CurrentFrameTicks / 1000.0f;
        
        while(Game.Running)
        {
            // TODO: Compress this into the fps timer struct
            CurrentFrameTicks = TimerGetTicks(&FPSTimer);
            DeltaTicks = CurrentFrameTicks - LastFrameTicks;
            LastFrameTicks = CurrentFrameTicks;
            DeltaTimeSeconds = DeltaTicks / 1000.0f;
            
            HandleInput(&Game);
            
            if(Game.dx != 0.0f || Game.dy != 0.0f)
            {            
                // vec2 Direction = Normalize(vec(Game.dx, Game.dy));
                // vec2 Velocity = (Direction * Game.Speed);
                // World.Bodies[0].LinearVelocity = Velocity;
                
                vec2 ForceDirection = Normalize(vec(Game.dx, Game.dy));
                vec2 Force = (ForceDirection * Game.ForceMagnitude);
                World.Bodies[0].Force = Force;
            }
            
            if(Game.RotationalVelocity != 0.0f)
                World.Bodies[0].RotationalVelocity = Game.RotationalVelocity;
            else
                World.Bodies[0].RotationalVelocity = 0.0f;
            
            UpdatePhysicsWorld2d(&World, DeltaTimeSeconds);
            
            ClearRenderer(Game.Renderer, GRAY);
            
            // NOTE: Render all the physics bodies
            for(int i=0; i<World.BodyCount; i++)
            {
                real32 Width, Height;
                if(World.Bodies[i].Shape == CIRCLE)
                {
                    Width = 2.0f * World.Bodies[i].Radius;
                    Height = 2.0f * World.Bodies[i].Radius;
                }
                else
                {
                    Width = World.Bodies[i].Width;
                    Height = World.Bodies[i].Height;
                }
                
                
                if(IsVisible(World.Bodies[i].Position.x, World.Bodies[i].Position.y, 
                             Width, Height, &Game.Camera))
                {
                    RenderPhysicsBody(&Game.Camera, Game.Renderer, &World.Bodies[i], 
                                      Colors[i], WHITE);
                }
            }
            
            // NOTE: Camera Info display
            {
                char buf[256] = {};
                sprintf_s(buf, "CamX: %0.3f, CamY: %0.3f, Zoom: %0.3f", Game.Camera.Position.x, Game.Camera.Position.y, Game.Camera.Zoom);
                RenderTextFromCenter(SCREEN_WIDTH / 2,
                                     PADDING_20,
                                     buf,
                                     Game.Renderer,
                                     Game.Font,
                                     WHITE);
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
