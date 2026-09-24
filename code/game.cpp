#include "game.h"

/* TODO:
   1) Camera - (Simple camera done, Need more advanced camera with actual frustum culling)
   2) Sound - (No sound features yet)
   3) Opengl - (As soon as the basic 2d physics engine completed, need to work on opengl 
   rendering)
   4) Dynamic arrays
   5) Memory Arena
6) Profiler
7) Optimization; currently it is performing very very bad.....
*/

void
CloseGame(game* Game)
{
    SDL_DestroyWindow( Game->Window );
    Game->Window = NULL;
    
	MemoryArenaFreeAll(Game->MemoryArena);
	
    SDL_Quit();
}

void
InitializeGame(game* Game)
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
	Game->Camera.MinZoom = 1.00f;
	
	Game->BodyColors = PushArray(Game->MemoryArena, MAX_BODIES, vec4);
	
	Game->World = PushStruct(Game->MemoryArena, physics_world2D);
	Game->World->Bodies = PushArray(Game->MemoryArena, MAX_BODIES, physics_body2D);
	Game->World->ContactPoints = PushArray(Game->MemoryArena, MAX_CONTACT_POINTS, vec2);
	Game->World->CollisionManifolds = PushArray(Game->MemoryArena, MAX_COLLISION_MANIFOLDS, collision_manifold);
	
	physics_body2D BottomPlatform = CreateBoxPhysicsBody2D(Game->World,
														   vec(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 6.0f * PADDING_20),
														   SCREEN_WIDTH / 2,
														   PADDING_20 + 1.0f,
														   0.5f,
														   0.5f,
														   true);
	
	BottomPlatform.Angle = PI / 9;
	AddPhysicsBodyToGameWithRandomColor(Game, BottomPlatform);
	
	physics_body2D LeftPaddle = CreateBoxPhysicsBody2D(Game->World,
													   vec(SCREEN_WIDTH / 2.5f, SCREEN_HEIGHT / 2.0f),
													   SCREEN_WIDTH / 4.0f,
													   PADDING_20 + 1.0f,
													   0.5f,
													   0.5f,
													   true);
	LeftPaddle.Angle = PI / 4;
	AddPhysicsBodyToGameWithRandomColor(Game, LeftPaddle);
	
	physics_body2D RightPaddle = CreateBoxPhysicsBody2D(Game->World,
														vec(SCREEN_WIDTH / 1.5f, SCREEN_HEIGHT / 2.0f),
														SCREEN_WIDTH / 4.0f,
														PADDING_20 + 1.0f,
														0.5f,
														0.5f,
														true);
	RightPaddle.Angle = -PI / 4;
	AddPhysicsBodyToGameWithRandomColor(Game, RightPaddle);
}

int
main(int argc, char* args[])
{
    uint64 MemoryArenaSize = Gigabytes(1);
	game* Game = BootstrapPushStruct(game, MemoryArenaSize);
	Assert(Game);
	Game->MemoryArenaSize = MemoryArenaSize;
	
    if(!InitializeEngine(Game))
    {
        LOG_ERROR("Failed to initialize!\n");
        return -1;
    }
    else
    {
		
		// NOTE: OPENGL TESTING
		InitializeGame(Game);
		
		fpsTimer* FPSTimer = PushStruct(Game->MemoryArena, fpsTimer);
		FPSTimerInit(FPSTimer);
		
        while(Game->Running)
        {
            FPSTimerUpdate(FPSTimer);
            
            HandleInput(Game);
            
#if GPU_RENDERED
			
			glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			glClearColor(1.f, 0.f, 1.f, 0.f);
			glClear(GL_COLOR_BUFFER_BIT);
			
			SDL_GL_SwapWindow(Game->Window);
			
			// NOTE: End of game loop
#else
			
            /*if(Game->dx != 0.0f || Game->dy != 0.0f)
            {            
                // vec2 Direction = Normalize(vec(Game->dx, Game->dy));
                // vec2 Velocity = (Direction * Game->Speed);
                // World.Bodies[0].LinearVelocity = Velocity;
                
                vec2 ForceDirection = Normalize(vec(Game->dx, Game->dy));
                vec2 Force = (ForceDirection * Game->ForceMagnitude);
                Game->World->Bodies[1].Force = Force;
            }
            
            if(Game->AnglealVelocity != 0.0f)
                Game->World->Bodies[1].AngularVelocity = Game->AngularVelocity;
            else
                Game->World->Bodies[1].AngularVelocity = 0.0f;
            */
			
            UpdatePhysicsWorld2d(Game->World, (real32)(FPSTimer->DeltaTicks / 1000.0f), 20);
            
			DeleteOutofReachPhysicsBodies(Game);
			
            ClearRenderer(Game->Renderer, BLACK);
            
            // NOTE: Render all the physics bodies
            for(int i = 0; 
				i < Game->World->BodyCount; 
				i++)
            {
                real32 Width, Height;
                if(Game->World->Bodies[i].Shape == CIRCLE)
                {
                    Width = 2.0f * Game->World->Bodies[i].Radius;
                    Height = 2.0f * Game->World->Bodies[i].Radius;
                }
                else
                {
                    Width = Game->World->Bodies[i].Width;
                    Height = Game->World->Bodies[i].Height;
                }
                
                
                if(IsVisible(Game->World->Bodies[i].Position.x, Game->World->Bodies[i].Position.y, 
                             Width, Height, &Game->Camera))
                {
                    RenderPhysicsBody(Game->Renderer, 
									  &Game->World->Bodies[i], 
									  &Game->Camera, 
									  Game->BodyColors[i], 
									  WHITE, 
									  Game->HighlightCollisions);
                }
            }
			
#if 0
			for(int i = 0; 
				i < Game->World->ContactPointsCount; 
				i++)
			{
				vec2 ContactPoint = Game->World->ContactPoints[i];
				uint32 X = RoundReal32ToUint32((ContactPoint.x - Game->Camera.Position.x) * Game->Camera.Zoom + (Game->Camera.Width / 2.0f));
				uint32 Y = RoundReal32ToUint32((ContactPoint.y - Game->Camera.Position.y) * Game->Camera.Zoom + (Game->Camera.Height / 2.0f));
				
				uint32 Radius = RoundReal32ToUint32(2.0f * Game->Camera.Zoom);
				
				RenderFilledCircle(Game->Renderer, X, Y, Radius, RED);
			}
#endif
			
            // TODO: Use temporary memory arena to store string value
            // NOTE: Camera Info display
            {
                char buf[256] = {};
                sprintf_s(buf, "CamX: %0.3f, CamY: %0.3f, Zoom: %0.3f", Game->Camera.Position.x, Game->Camera.Position.y, Game->Camera.Zoom);
                
				RenderTextFromCenter(Game->Renderer,
									 SCREEN_WIDTH / 2,
                                     PADDING_20,
                                     buf,
                                     Game->Font,
                                     WHITE,
									 30);
            }
            
            // TODO: Use temporary memory arena to store string value
            // NOTE: FPS Display
            {
                char buf[256] = {};
                sprintf_s(buf, "FPS: %d, Total Physics Bodies: %d", GetFPS(FPSTimer), Game->World->BodyCount);
                
				RenderTextFromCenter(Game->Renderer,
									 SCREEN_WIDTH / 2,
                                     SCREEN_HEIGHT - PADDING_20,
                                     buf,
                                     Game->Font,
                                     WHITE,
									 30);
            }
			
			SDL_RenderPresent(Game->Renderer);
#endif
        }
	}
    
	// CloseGame(Game);
	
    return 0;
}
