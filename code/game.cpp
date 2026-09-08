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
void RenderTextFromCenter(SDL_Renderer* Renderer,
						  int XPos,
                          int YPos,
                          const char* TextureText,
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
    
    SDL_Surface* TextSurface = TTF_RenderText_Blended(Font, TextureText, SdlColor);
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
RenderPhysicsBody(SDL_Renderer* Renderer, 
				  physics_body2D* Body, 
				  simple_camera* Camera, 
				  vec4 FillColor, 
				  vec4 BorderColor)
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
                BorderColor = RED;
            
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
                BorderColor = RED;
            
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

vec4
GenerateRandomColor()
{
	real32 R = RandomUnilateral();
	real32 G = RandomUnilateral();
	real32 B = RandomUnilateral();
	
	return vec(R, G, B, 1.0f);
}

void 
GenerateRandomColors(vec4* ColorArray, int ArrayCount)
{
    for(int i=0; i<ArrayCount; i++)
    {
        ColorArray[i] = GenerateRandomColor();
    }
}

bool32
InitializeEngine(game* Game)
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
			// NOTE: Set linear filtering BEFORE renderer creation
			SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
            
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
                
                // NOTE: Initialize SDL_ttf
				// TODO: Font customization functionality
                if (TTF_Init() == -1)
                {
                    LOG_ERROR("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
                    return false;
                }
                else
                {
                    Game->Font = TTF_OpenFont("assets/font/Lightweight.ttf", 128);
                    if (Game->Font == NULL)
                    {
                        LOG_ERROR("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
                        return false;
                    }
					else
					{
						TTF_SetFontStyle(Game->Font, TTF_STYLE_NORMAL);
						TTF_SetFontHinting(Game->Font, TTF_HINTING_LIGHT);
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

vec2
GetRelativeWorldPosition(simple_camera* Camera, real32 NewX, real32 NewY)
{
	real32 OffsetX = NewX - (Camera->Width / 2.0f);
	real32 OffsetY = NewY - (Camera->Height / 2.0f);
	
	real32 NewWorldX = Camera->Position.x + (OffsetX / Camera->Zoom);
    real32 NewWorldY = Camera->Position.y + (OffsetY / Camera->Zoom);
    
	return vec(NewWorldX, NewWorldY);
}

void 
PanCamera(simple_camera* Camera, real32 NewX, real32 NewY)
{
	real32 NewCameraPositionX = Camera->Position.x - (NewX / Camera->Zoom);
    real32 NewCameraPositionY = Camera->Position.y - (NewY / Camera->Zoom);
	
    Camera->Position = vec(NewCameraPositionX, NewCameraPositionY);
}

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
    
    // NOTE: Clamp zoom level to the requested bounds
    if(Camera->Zoom < Camera->MinZoom) Camera->Zoom = Camera->MinZoom;
    if(Camera->Zoom > Camera->MaxZoom)  Camera->Zoom = Camera->MaxZoom;
    
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
				// TODO: Generate physics body with random size and color?
                case SDLK_c:
                {
					int MouseX, MouseY;
					SDL_GetMouseState(&MouseX, &MouseY);
					
					vec2 MousePos = GetRelativeWorldPosition(&Game->Camera, (real32)MouseX, (real32)MouseY); 
					
					physics_body2D Body = CreateCirclePhysicsBody2D(Game->World, vec(MousePos.x, MousePos.y), 10.0f, 0.6f, 0.90f, false);
					
					Game->World->Bodies.push_back(Body);
                    
					break;
                }
				
				case SDLK_b:
                {
					int MouseX, MouseY;
					SDL_GetMouseState(&MouseX, &MouseY);
					
					vec2 MousePos = GetRelativeWorldPosition(&Game->Camera, (real32)MouseX, (real32)MouseY); 
					
					physics_body2D Body = CreateBoxPhysicsBody2D(Game->World, vec(MousePos.x, MousePos.y), 20.0f, 20.0f, 0.6f, 0.60f, false);
					
					Game->World->Bodies.push_back(Body);
                    
					break;
                }
                
                case SDLK_r:
                {
					while(Game->World->Bodies.size() > 1)
					{
						Game->World->Bodies.pop_back();
					}
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
    
    const Uint8* KeyState = SDL_GetKeyboardState(NULL);
    
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

void
InitializeGameAndWorld(game* Game, physics_world2D* World)
{
	InitializeRandomNumbers();
	
	Game->Running = true;
	Game->dx = 0.0f;
	Game->dy = 0.0f;
	Game->Speed = 100.0f;
	Game->ForceMagnitude = 15000.0f;
	
	Game->Camera = {0};
	Game->Camera.Zoom = 1.0f;
	Game->Camera.Pan = 0.0f;
	Game->Camera.Width = SCREEN_WIDTH;
	Game->Camera.Height = SCREEN_HEIGHT;
	Game->Camera.Position.x = SCREEN_WIDTH / 2.0f;
	Game->Camera.Position.y = SCREEN_HEIGHT / 2.0f;
	Game->Camera.MaxZoom = 10.0f;
	Game->Camera.MinZoom = 0.30f;
	
	Game->World = World;
	
	physics_body2D BottomPlatform = CreateBoxPhysicsBody2D(Game->World,
														   vec(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - (PADDING_20 / 2.0f)),
														   SCREEN_WIDTH,
														   PADDING_20 + 1.0f,
														   0.5f,
														   0.5f,
														   true);
	Game->World->Bodies.push_back(BottomPlatform);
	
	physics_body2D Player = CreateCirclePhysicsBody2D(Game->World, vec(SCREEN_WIDTH/2, SCREEN_HEIGHT/2), 10.0f, 0.6f, 0.50f, false);
	
	Game->World->Bodies.push_back(Player);
}

void
DeleteOutofReachPhysicsBodies(game* Game)
{
	// NOTE: Delete physics objects if they leave the zoom level 0.030 bounds
	real32 MaxViewWidth = Game->Camera.Width / Game->Camera.MinZoom;
	real32 MaxViewHeight = Game->Camera.Height / Game->Camera.MinZoom;
	real32 BoundLeft = Game->Camera.Position.x - (MaxViewWidth / 2.0f);
	real32 BoundRight = Game->Camera.Position.x + (MaxViewWidth / 2.0f);
	real32 BoundTop = Game->Camera.Position.y - (MaxViewHeight / 2.0f);
	real32 BoundBottom = Game->Camera.Position.y + (MaxViewHeight / 2.0f);
	
	for(uint32 i = 0; i < Game->World->Bodies.size();)
	{
		if(!Game->World->Bodies[i].IsStatic)
		{
			// NOTE: CRAZY ACCESS OF VECTOR ELEMENT!!!
			AABB aabb = GetAABBFromPhysicsBody(&Game->World->Bodies.data()[i]);
			if(aabb.Min.x < BoundLeft || aabb.Max.x > BoundRight || aabb.Min.y < BoundTop || aabb.Max.y > BoundBottom)
			{
				Game->World->Bodies.erase(Game->World->Bodies.begin() + i);
				continue;
			}
		}
		i++;
	}
}

int
main(int argc, char* args[])
{
    game Game = {};
    
    if(!InitializeEngine(&Game))
    {
        LOG_ERROR("Failed to initialize!\n");
        return -1;
    }
    else
    {
		physics_world2D World = {};
		InitializeGameAndWorld(&Game, &World);
        
        fpsTimer FPSTimer = {};
		FPSTimerInit(&FPSTimer);
		
        while(Game.Running)
        {
            // TODO: Compress this into the fps timer struct
            FPSTimerUpdate(&FPSTimer);
            
            HandleInput(&Game);
            
            /*if(Game.dx != 0.0f || Game.dy != 0.0f)
            {            
                // vec2 Direction = Normalize(vec(Game.dx, Game.dy));
                // vec2 Velocity = (Direction * Game.Speed);
                // World.Bodies[0].LinearVelocity = Velocity;
                
                vec2 ForceDirection = Normalize(vec(Game.dx, Game.dy));
                vec2 Force = (ForceDirection * Game.ForceMagnitude);
                Game.World->Bodies[1].Force = Force;
            }
            
            if(Game.RotationalVelocity != 0.0f)
                Game.World->Bodies[1].RotationalVelocity = Game.RotationalVelocity;
            else
                Game.World->Bodies[1].RotationalVelocity = 0.0f;
            */
			
            UpdatePhysicsWorld2d(Game.World, (real32)(FPSTimer.DeltaTicks / 1000.0f), 20);
            
			DeleteOutofReachPhysicsBodies(&Game);
			
            ClearRenderer(Game.Renderer, GRAY);
            
            // NOTE: Render all the physics bodies
            for(int i=0; i<Game.World->Bodies.size(); i++)
            {
                real32 Width, Height;
                if(Game.World->Bodies[i].Shape == CIRCLE)
                {
                    Width = 2.0f * Game.World->Bodies[i].Radius;
                    Height = 2.0f * Game.World->Bodies[i].Radius;
                }
                else
                {
                    Width = Game.World->Bodies[i].Width;
                    Height = Game.World->Bodies[i].Height;
                }
                
                
                if(IsVisible(Game.World->Bodies[i].Position.x, Game.World->Bodies[i].Position.y, 
                             Width, Height, &Game.Camera))
                {
                    if(Game.World->Bodies[i].IsStatic)
                        RenderPhysicsBody(Game.Renderer, 
										  &Game.World->Bodies[i], 
										  &Game.Camera, 
                                          RED, BLACK);
                    else
					{
						RenderPhysicsBody(Game.Renderer, 
										  &Game.World->Bodies[i], 
										  &Game.Camera, 
                                          BLACK, WHITE);
					}
                }
            }
			
			for(int i=0; i<Game.World->ContactPoints.size(); i++)
			{
				vec2 ContactPoint = Game.World->ContactPoints.data()[i];
				uint32 X = RoundReal32ToUint32((ContactPoint.x - Game.Camera.Position.x) * Game.Camera.Zoom + (Game.Camera.Width / 2.0f));
				uint32 Y = RoundReal32ToUint32((ContactPoint.y - Game.Camera.Position.y) * Game.Camera.Zoom + (Game.Camera.Height / 2.0f));
				
				uint32 Radius = RoundReal32ToUint32(2.0f * Game.Camera.Zoom);
				
				RenderFilledCircle(Game.Renderer, X, Y, Radius, RED);
			}
			
            
            // NOTE: Camera Info display
            {
                char buf[256] = {};
                sprintf_s(buf, "CamX: %0.3f, CamY: %0.3f, Zoom: %0.3f", Game.Camera.Position.x, Game.Camera.Position.y, Game.Camera.Zoom);
                RenderTextFromCenter(Game.Renderer,
									 SCREEN_WIDTH / 2,
                                     PADDING_20,
                                     buf,
                                     Game.Font,
                                     WHITE,
									 40);
            }
            
            
            // NOTE: FPS Display
            {
                char buf[256] = {};
                sprintf_s(buf, "FPS: %d, Total Physics Bodies: %ld", GetFPS(&FPSTimer), Game.World->Bodies.size());
                RenderTextFromCenter(Game.Renderer,
									 SCREEN_WIDTH / 2,
                                     SCREEN_HEIGHT - PADDING_20,
                                     buf,
                                     Game.Font,
                                     WHITE,
									 40);
            }
            
            SDL_RenderPresent(Game.Renderer);
        }
    }
    
    CloseGame(&Game);
    
    return 0;
}
