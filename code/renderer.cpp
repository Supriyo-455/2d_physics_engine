#include "renderer.h"

SDL_Color ConvertToSDLColor(vec4 Color)
{
    SDL_Color SdlColor = {};
    
    SdlColor.r = (uint8) (Color.r * 255);
    SdlColor.g = (uint8) (Color.g * 255);
    SdlColor.b = (uint8) (Color.b * 255);
    SdlColor.a = (uint8) (Color.a * 255);
    
    return SdlColor;
}

vec4
GenerateDarkRandomColor()
{
	real32 R = 0.5f * RandomUnilateral();
	real32 G = 0.5f * RandomUnilateral();
	real32 B = 0.5f * RandomUnilateral();
	
	return vec(R, G, B, 1.0f);
}

void 
GenerateDarkRandomColors(vec4* ColorArray, int ArrayCount)
{
    for(int i=0; i<ArrayCount; i++)
    {
        ColorArray[i] = GenerateDarkRandomColor();
    }
}

// TODO: Need to replace this cpu based calls with Opengl
// TODO: Generalize texture
// TODO: Texture Scalling is not optimized
// TODO: This texture rendering causing so much performance issue, NEED TO REPLACE WITH OPENGL ASAP!!
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
				  vec4 BorderColor,
				  bool32 HighlightCollisions)
{
    switch(Body->Shape)
    {
        case CIRCLE:
        {
            uint32 X = RoundReal32ToUint32((Body->Position.x - Camera->Position.x) * Camera->Zoom + (Camera->Width / 2.0f));
            uint32 Y = RoundReal32ToUint32((Body->Position.y - Camera->Position.y) * Camera->Zoom + (Camera->Height / 2.0f));
            
            uint32 Radius = RoundReal32ToUint32(Body->Radius * Camera->Zoom);
            
            RenderFilledCircle(Renderer, X, Y, Radius, FillColor);
            
			if(HighlightCollisions)
			{
				if(Body->IsCollided)
                {
					BorderColor = RED;
				}
			}
            
            RenderHollowCircle(Renderer, X, Y, Radius, BorderColor);
            
            // TODO: This is ugly and hacky, no actual rotation of circle
#if 0
			SDL_Color Border = ConvertToSDLColor(BorderColor);
            SDL_SetRenderDrawColor(Renderer, Border.r, Border.g, Border.b, Border.a);
            
            real32 RCos = X  + Radius * cosf(Body->Rotation);
            real32 RSin = Y  + Radius * sinf(Body->Rotation);
            
            SDL_RenderDrawLineF(Renderer,
                                (real32) X, (real32) Y,
                                RCos, RSin);
#endif
        }
        break;
        case BOX:
        {
            vec2* WorldVerts = GetPhysicsBodyTransformedVertices(Body);
            
            SDL_Vertex SdlVerts[4] = {};
            SDL_Color Fill = ConvertToSDLColor(FillColor);
            
            for(int i = 0; 
				i < 4; 
				i++)
            {
                SdlVerts[i].position.x = (WorldVerts[i].x - Camera->Position.x) * Camera->Zoom + (Camera->Width / 2.0f);
                
                SdlVerts[i].position.y = (WorldVerts[i].y - Camera->Position.y) * Camera->Zoom + (Camera->Height / 2.0f);
                
                SdlVerts[i].color = Fill;
            }
            
            SDL_RenderGeometry(Renderer, NULL, SdlVerts, 4, Body->Triangles, 6);
            
			if(HighlightCollisions)
			{
				if(Body->IsCollided)
                {
					BorderColor = RED;
				}
			}
            
            SDL_Color Border = ConvertToSDLColor(BorderColor);
            SDL_SetRenderDrawColor(Renderer, Border.r, Border.g, Border.b, Border.a);
            
            for(int i = 0; 
				i < 4; 
				i++)
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
ClearRenderer(SDL_Renderer* Renderer, vec4 Color)
{
    SDL_Color SdlColor = ConvertToSDLColor(Color);
    SDL_SetRenderDrawColor(Renderer, SdlColor.r, SdlColor.g, SdlColor.b, SdlColor.a);
    SDL_RenderClear(Renderer);
}

bool32
__InitializeGPURenderedEngine__(game* Game)
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
                                        SDL_WINDOW_OPENGL);
        
        if(Game->Window == NULL)
        {
            LOG_ERROR("Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
		else
		{
			Game->GLContext = SDL_GL_CreateContext(Game->Window);
		}
    }
    
    return success;
}

bool32
__InitializeCPURenderedEngine__(game* Game)
{
    bool32 success = true;
    
    // NOTE: Force OpenGL driver and Render Batching to drastically reduce draw calls
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    SDL_SetHint(SDL_HINT_RENDER_BATCHING, "1");
    
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
                    Game->Font = TTF_OpenFont("assets/font/Consolas-Regular.ttf", 128);
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

#if GPU_RENDERED
#define InitializeEngine(Game) __InitializeGPURenderedEngine__(Game)
#else
#define InitializeEngine(Game) __InitializeCPURenderedEngine__(Game)
#endif
