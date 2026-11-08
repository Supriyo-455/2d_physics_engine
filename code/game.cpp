#include "game.h"

#include "memory.cpp"
#include "transform_2d.cpp"
#include "physics_body_2d.cpp"
#include "collision_2d.cpp"
#include "camera.cpp"

internal void ExportAndRenderDebugRecords(game_memory* Memory, game_render_commands* RenderCommands);

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
    for(int i=0; 
        i < ArrayCount; 
        i++)
    {
        ColorArray[i] = GenerateDarkRandomColor();
    }
}

void
UpdatePhysicsWorld2d(physics_world2D* World, real32 ElapsedTime, int Iterations)
{
    TIMED_BLOCK(PhysicsUpdateTotal);
    
	Iterations = Clamp(Iterations, World->MinIteration, World->MaxIteration);
	
	real32 SubStepTime = ElapsedTime / Iterations;
	
	for(int it = 0;
		it < Iterations; 
		it++)
	{
		// NOTE: Movement step
		for(uint32 i = 0; 
			i < World->BodyCount; 
			i++)
		{
			ApplyGravity(&World->Bodies[i],
						 World->Gravity, 
						 World->ForceMultiplier, 
						 SubStepTime);
			
			ApplyForce(&World->Bodies[i], SubStepTime);
			RotatePhysicsBody(&World->Bodies[i], SubStepTime);
			MovePhysicsBody(&World->Bodies[i], SubStepTime);
			
			// NOTE: Reset the force to zero
			World->Bodies[i].Force = vec(0.0f, 0.0f);
		}
		
		// NOTE: Reset collision state
		for(uint32 i = 0; 
			i < World->BodyCount; 
			i++)
		{
			World->Bodies[i].IsCollided = false;
		}
		
		
		World->CollisionManifoldsCount = 0;
		World->ContactPairsCount = 0;
		World->ContactPointsCount = 0;
		
		CollisionResolutionBroadPhase(World);
		CollisionResolutionNarrowPhase(World);
	}
}

void
InitGameState(game_memory* Memory, game_state* GameState)
{
    if(!Memory->IsInitialized)
    {
        InitializeRandomNumbers();
        
        MemoryArenaInit(&GameState->WorldArena, 
                        (uint8*)Memory->PermanentStorage + sizeof(game_state),
                        Memory->PermanentStorageSize - sizeof(game_state));
        
        GameState->Camera = {};
        GameState->Camera.Zoom = 1.0f;
        GameState->Camera.Width = 1300.0f;
        GameState->Camera.Height = 800.0f;
        GameState->Camera.Position = vec(650.0f, 400.0f);
        GameState->Camera.MaxZoom = 10.0f;
        GameState->Camera.MinZoom = 1.0f;
        GameState->HighlightCollisions = false;
        GameState->WireframeMode = false;
        
        GameState->BodyColors = PushArray(&GameState->WorldArena, MAX_BODIES, vec4);
        GameState->BodyColorsCount = 0;
        
        GameState->World = PushStruct(&GameState->WorldArena, physics_world2D);
        GameState->World->Bodies = PushArray(&GameState->WorldArena, MAX_BODIES, physics_body2D);
        GameState->World->BodyCount = 0;
        GameState->World->ContactPairs = PushArray(&GameState->WorldArena, MAX_CONTACT_PAIRS, contact_pair);
        GameState->World->ContactPairsCount = 0;
        GameState->World->ContactPoints = PushArray(&GameState->WorldArena, MAX_CONTACT_POINTS, vec2);
        GameState->World->ContactPointsCount = 0;
        GameState->World->CollisionManifolds = PushArray(&GameState->WorldArena, MAX_COLLISION_MANIFOLDS, collision_manifold);
        GameState->World->CollisionManifoldsCount = 0;
        
        // Spawn default platforms
        real32 ScreenWidth = 1300.0f;
        real32 ScreenHeight = 800.0f;
        real32 Padding20 = 20.0f;
        
        physics_body2D BottomPlatform = CreateBoxPhysicsBody2D(GameState->World,
                                                               vec(ScreenWidth / 2.0f, ScreenHeight - 6.0f * Padding20),
                                                               ScreenWidth / 2.0f,
                                                               Padding20 + 1.0f,
                                                               0.5f,
                                                               0.2f,
                                                               true);
        BottomPlatform.Angle = PI;
        GameState->World->Bodies[GameState->World->BodyCount++] = BottomPlatform;
        GameState->BodyColors[GameState->BodyColorsCount++] = GenerateDarkRandomColor();
        
        physics_body2D LeftPaddle = CreateBoxPhysicsBody2D(GameState->World,
                                                           vec(ScreenWidth / 2.5f, ScreenHeight / 2.0f),
                                                           ScreenWidth / 4.0f,
                                                           Padding20 + 1.0f,
                                                           0.5f,
                                                           0.5f,
                                                           true);
        LeftPaddle.Angle = PI / 4.0f;
        GameState->World->Bodies[GameState->World->BodyCount++] = LeftPaddle;
        GameState->BodyColors[GameState->BodyColorsCount++] = GenerateDarkRandomColor();
        
        physics_body2D RightPaddle = CreateBoxPhysicsBody2D(GameState->World,
                                                            vec(ScreenWidth / 1.5f, ScreenHeight / 2.0f),
                                                            ScreenWidth / 4.0f,
                                                            Padding20 + 1.0f,
                                                            0.5f,
                                                            0.5f,
                                                            true);
        RightPaddle.Angle = -PI / 4.0f;
        GameState->World->Bodies[GameState->World->BodyCount++] = RightPaddle;
        GameState->BodyColors[GameState->BodyColorsCount++] = GenerateDarkRandomColor();
        
        // Spawn a couple of demo falling bodies
        physics_body2D DemoCircle = CreateCirclePhysicsBody2D(GameState->World, 
                                                              vec(ScreenWidth / 2.0f - 60.0f, 150.0f), 
                                                              16.0f, 0.6f, 0.85f, false);
        GameState->World->Bodies[GameState->World->BodyCount++] = DemoCircle;
        GameState->BodyColors[GameState->BodyColorsCount++] = GenerateDarkRandomColor();
        
        physics_body2D DemoBox = CreateBoxPhysicsBody2D(GameState->World, 
                                                        vec(ScreenWidth / 2.0f + 60.0f, 120.0f), 
                                                        32.0f, 32.0f, 0.6f, 0.60f, false);
        GameState->World->Bodies[GameState->World->BodyCount++] = DemoBox;
        GameState->BodyColors[GameState->BodyColorsCount++] = GenerateDarkRandomColor();
        
        Memory->IsInitialized = true;
    }
}

void 
HandleInput(game_input* Input, game_state* GameState)
{
    if(Input->MouseButtons[1].EndedDown)
    {
        PanCamera(&GameState->Camera, Input->MouseRel.x, Input->MouseRel.y);
    }
    
    if(Input->MouseWheelDelta != 0)
    {
        ZoomCamera(&GameState->Camera, Input->MousePos.x, Input->MousePos.y, Input->MouseWheelDelta);
    }
    
    vec2 MouseWorldPos = GetRelativeWorldPosition(&GameState->Camera, Input->MousePos.x, Input->MousePos.y);
    
    if(WasPressed(Input->SpawnCircle) || WasPressed(Input->MouseButtons[0]))
    {
        physics_body2D Body = CreateCirclePhysicsBody2D(GameState->World, MouseWorldPos, 14.0f, 0.6f, 0.85f, false);
        GameState->World->Bodies[GameState->World->BodyCount++] = Body;
        GameState->BodyColors[GameState->BodyColorsCount++] = GenerateDarkRandomColor();
    }
    
    if(WasPressed(Input->SpawnBox))
    {
        physics_body2D Body = CreateBoxPhysicsBody2D(GameState->World, MouseWorldPos, 28.0f, 28.0f, 0.6f, 0.60f, false);
        GameState->World->Bodies[GameState->World->BodyCount++] = Body;
        GameState->BodyColors[GameState->BodyColorsCount++] = GenerateDarkRandomColor();
    }
    
    if(WasPressed(Input->Reset))
    {
        GameState->World->BodyCount = 3;
        GameState->BodyColorsCount = 3;
    }
    
    if(WasPressed(Input->ToggleWireframe))
    {
        GameState->WireframeMode = !GameState->WireframeMode;
    }
    
    if(WasPressed(Input->HighlightCollisions))
    {
        GameState->HighlightCollisions = !GameState->HighlightCollisions;
    }
}

extern "C" 
#ifdef _WIN32
__declspec(dllexport)
#else
__attribute__((visibility("default")))
#endif
GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    Assert(sizeof(game_state) <= Memory->PermanentStorageSize);
    game_state* GameState = (game_state*)Memory->PermanentStorage;
    
    InitGameState(Memory, GameState);
    
    HandleInput(Input, GameState);
    
    UpdatePhysicsWorld2d(GameState->World, Input->dtForFrame, 128);
    DeleteOutofReachPhysicsBodies(GameState);
    
    PushClear(RenderCommands, vec(0.00f, 0.00f, 0.00f, 1.0f));
    
    for(uint32 i = 0; 
        i < GameState->World->BodyCount; 
        i++)
    {
        physics_body2D* Body = &GameState->World->Bodies[i];
        
        vec4 OutlineColor = WHITE;
        if(GameState->HighlightCollisions && Body->IsCollided)
        {
            OutlineColor = RED;
        }
        
        if(Body->Shape == BOX)
        {
            vec2* Verts = GetPhysicsBodyTransformedVertices(Body);
            vec2 ScreenVerts[4];
            for(int v = 0; v < 4; v++)
            {
                ScreenVerts[v] = WorldToScreen(&GameState->Camera, Verts[v]);
            }
            PushQuad(RenderCommands, ScreenVerts, GameState->BodyColors[i], OutlineColor, true);
        }
        else if(Body->Shape == CIRCLE)
        {
            vec2 ScreenPos = WorldToScreen(&GameState->Camera, Body->Position);
            real32 ScaledRadius = Body->Radius * GameState->Camera.Zoom;
            PushCircle(RenderCommands, ScreenPos, ScaledRadius, 
                       Body->Angle, GameState->BodyColors[i], OutlineColor, true);
        }
    }
    
    char Buf[128];
    int FPS = (Input->dtForFrame > 0.0001f) ? (int)(1.0f / Input->dtForFrame) : 0;
    snprintf(Buf, sizeof(Buf), "FPS: %d | Bodies: %d | Zoom: %0.2f  [C/Click: Circle, B: Box, R: Reset]", 
             FPS, GameState->World->BodyCount, GameState->Camera.Zoom);
    PushText(RenderCommands, vec(20.0f, 20.0f), Buf, WHITE, 24);
    
    ExportAndRenderDebugRecords(Memory, RenderCommands);
}

debug_record DebugRecordArray[__COUNTER__];

internal void
ExportAndRenderDebugRecords(game_memory* Memory, game_render_commands* RenderCommands)
{
    Memory->DebugRecordCount = (uint32)ARRAY_COUNT(DebugRecordArray);
    Memory->DebugRecords     = DebugRecordArray;
    
    real32 DebugY = 50.0f;
    for(uint32 RecordIndex = 0; RecordIndex < (uint32)ARRAY_COUNT(DebugRecordArray); ++RecordIndex)
    {
        debug_record* Record = DebugRecordArray + RecordIndex;
        if(Record->BlockName)
        {
            char RecordBuf[256];
            
            real32 MegaCycles = (real32)Record->CycleCount / (1000.0f * 1000.0f);
            uint32 CyclesPerHit = (Record->HitCount > 0) ? (uint32)(Record->CycleCount / Record->HitCount) : 0;
            
            snprintf(RecordBuf, sizeof(RecordBuf),
                     "%s: %.2f Mcy (%u hits, %u cy/hit)",
                     Record->BlockName, MegaCycles, Record->HitCount, CyclesPerHit);
            
            PushText(RenderCommands, vec(20.0f, DebugY), RecordBuf, YELLOW, 20);
            
            DebugY += 24.0f;
            
            Record->CycleCount = 0;
            Record->HitCount = 0;
        }
    }
}