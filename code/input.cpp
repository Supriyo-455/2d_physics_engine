#include "input.h"

void
AddPhysicsBodyToGameWithRandomColor(game* Game, physics_body2D Body)
{
	Game->World->Bodies[Game->World->BodyCount++] = Body;
	Game->BodyColors[Game->BodyColorsCount++] = GenerateDarkRandomColor();
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
					
					if(Game->World->BodyCount < MAX_BODIES)
					{
						physics_body2D Body = CreateCirclePhysicsBody2D(Game->World, vec(MousePos.x, MousePos.y), 10.0f, 0.6f, 0.90f, false);
						
						AddPhysicsBodyToGameWithRandomColor(Game, Body);
					}
					
					break;
                }
				
				case SDLK_b:
                {
					int MouseX, MouseY;
					SDL_GetMouseState(&MouseX, &MouseY);
					
					vec2 MousePos = GetRelativeWorldPosition(&Game->Camera, (real32)MouseX, (real32)MouseY); 
					
					if(Game->World->BodyCount < MAX_BODIES)
					{
						physics_body2D Body = CreateBoxPhysicsBody2D(Game->World, vec(MousePos.x, MousePos.y), 20.0f, 20.0f, 0.6f, 0.60f, false);
						
						AddPhysicsBodyToGameWithRandomColor(Game, Body);
					}
					
					break;
                }
                
                case SDLK_r:
                {
					Game->World->BodyCount = 3;
					Game->BodyColorsCount = 3;
                    break;
                }
                
                case SDLK_SPACE:
                {
					Game->WireframeMode = !Game->WireframeMode;
					if(Game->WireframeMode)
					{
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					}
					else
					{
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					}
					
					Game->HighlightCollisions = !Game->HighlightCollisions;
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
