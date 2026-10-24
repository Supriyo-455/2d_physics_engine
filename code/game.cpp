#include "game.h"

/* TODO:
   1) Camera - (Simple camera done, Need more advanced camera with actual frustum culling)
   
2) Sound - (No sound features yet)
   
3) Opengl - (As soon as the basic 2d physics engine completed, need to work on opengl 
   rendering) (Currently working on this)

[Working]
* Basic Triangle rendering using fragment and vertex shader

[Planned]
* Hardware accelerated basic 2d shapes drawing, boxes, circles, lines.
* Hardware accelerated text rendering
* Hardware accelerated texture rendering
* Bloom effect.
* Multi-threading

4) Dynamic arrays
   
5) Memory Arena - Linear memory arena done, still there are other types of arenas, stack, free list research on those. And use memory arena for as much as of allocations in the engine.

6) Profiler

7) Optimization; currently it is performing very very bad.....

8) Robust logging functionality -> Currently it has verbosity, add filters on verbosity, add logs based on channels, save logs before crash in a file. Different colors for different verbosity?? (Optional)

9) Physics enhancemnet -
[Done]
* Broad phase and narrow phase separation.
* Collision resolution with rotation
* Friction

[Bugs Fixed]
* Circles falling on the bottom platform making jumps higher and higher from some invisible force, investigate on this and fix this issue.

[Planned]
* Fix jittering by put the system on rest when collision resolution is very small.
* Damping -> Research on Simple harmonic motion and Damped harmonic motion, how they can be effective on making the engine stable.
* Spring Physics -> Add spring physics
* Pendulums ??? For hangling ropes in game.
* Softbody physics

10) Engine enhancement -

[Planned]
* Tilemap editor
* Level designer
* Separate engine from the game for re-usability
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
	Game->World->ContactPairs = PushArray(Game->MemoryArena, MAX_CONTACT_PAIRS, contact_pair);
	Game->World->ContactPoints = PushArray(Game->MemoryArena, MAX_CONTACT_POINTS, vec2);
	Game->World->CollisionManifolds = PushArray(Game->MemoryArena, MAX_COLLISION_MANIFOLDS, collision_manifold);
	
	physics_body2D BottomPlatform = CreateBoxPhysicsBody2D(Game->World,
														   vec(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT - 6.0f * PADDING_20),
														   SCREEN_WIDTH / 2,
														   PADDING_20 + 1.0f,
														   0.5f,
														   0.2f,
														   true);
	
	BottomPlatform.Angle = PI;
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

const char* 
LoadFileIntoBuffer(memory_arena* Arena, const char* Filename) {
    FILE* File = fopen(Filename, "rb");
    if(!File)
	{
		LOG_ERROR("File: %s, does not exist!", Filename);
		return NULL;
	}
	
    fseek(File, 0, SEEK_END);
	uint64 Length = ftell(File);
    fseek(File, 0, SEEK_SET);
	
    char* Buffer = PushArray(Arena, Length + 1, char);
    if (Buffer) {
        fread(Buffer, 1, Length, File);
        Buffer[Length] = '\0';
    }
	
    fclose(File);
    return Buffer;
}

uint32
LoadTexture(const char* TexturePath, uint32 TextureUnit)
{
	uint32 TextureID = NULL;
	
	int Width, Height, Channels;
	uint8* Data = stbi_load(TexturePath, &Width, &Height, &Channels, 0);
	if(!Data)
	{
		LOG_ERROR("Failed to load texture from %s\n", TexturePath);
	}
	else
	{
		glGenTextures(1, &TextureID);
		
		glActiveTexture(TextureUnit);
		glBindTexture(GL_TEXTURE_2D, TextureID);
		
		// NOTE: Set texture wrapping and filtering parameters AFTER binding
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		real32 BorderColor[] = {1.0f, 1.0f, 0.0f, 1.0f};
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, BorderColor);
		
		
		uint32 Format = (Channels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, Data);
		
		glGenerateMipmap(GL_TEXTURE_2D);
		
		stbi_set_flip_vertically_on_load(true);
		stbi_image_free(Data);
	}
	
	return TextureID;
}

int
main(int argc, char* args[])
{
    uint64 BackingBufferSize = Megabytes(500);
	void* BackingBuffer = malloc(BackingBufferSize);
	memory_arena Arena = {};
	MemoryArenaInit(&Arena, BackingBuffer, BackingBufferSize);
	
	game* Game = PushStruct(&Arena, game);
	Assert(Game);
	Game->MemoryArena = &Arena;
	Game->MemoryArenaSize = BackingBufferSize;
	
	if(!InitializeEngine(Game))
    {
        LOG_ERROR("Failed to initialize Game!\n");
        return -1;
    }
    else
    {
		// NOTE: OPENGL TESTING
		InitializeGame(Game);
		
		fpsTimer* FPSTimer = PushStruct(Game->MemoryArena, fpsTimer);
		FPSTimerInit(FPSTimer);
		
#if GPU_RENDERED
		const char* VertexShaderSource = LoadFileIntoBuffer(Game->MemoryArena, "assets/shaders/vertex.glsl");
		
		const char* FragmentShaderSource = LoadFileIntoBuffer(Game->MemoryArena, "assets/shaders/fragment.glsl");
		
		int32 CountAttributes;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &CountAttributes);
		LOG_INFO("Maximum number of vertex attributes supported: %d\n", CountAttributes);
		
		// TODO: Resizable screens
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		real32 Vertices[] = {
			// positions          // colors           // texture coords
			0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
			0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
			-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
			-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
		};
		
		uint32 Indices[] = {
			0, 1, 3,
			1, 2, 3
		};
		
		uint32 VAO;
		glGenVertexArrays(1, &VAO);
		
		uint32 VBO;
		glGenBuffers(1, &VBO);
		
		uint32 EBO;
		glGenBuffers(1, &EBO);
		
		glBindVertexArray(VAO);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
		
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
		
		// NOTE: Vertex position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(real32), (void*)0);
		glEnableVertexAttribArray(0);
		
		// NOTE: Vertex color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(real32), (void*)(3* sizeof(real32)));
		glEnableVertexAttribArray(1);
		
		// NOTE: Vertex texture attibute
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(real32), (void*)(6 * sizeof(real32)));
		glEnableVertexAttribArray(2);  
		
		uint32 Texture1 = LoadTexture("assets/wall.jpg", GL_TEXTURE0);
		uint32 Texture2 = LoadTexture("assets/awesomeface.png", GL_TEXTURE1);
		
		// TODO: Unbind everything after setup, we will bind required things in game loop
		
		// TODO: Smart way to unbind all textures before game loop?
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
		
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		
		uint32 VertexShader;
		VertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(VertexShader, 1, &VertexShaderSource, NULL);
		glCompileShader(VertexShader);
		
		temp_memory_arena TempArena = TempMemoryArenaBegin(Game->MemoryArena);
		
		bool32 Success;
		uint32 InfoLogLength = 512;
		char* InfoLog = PushArray(TempArena.Arena, InfoLogLength, char);
		
		glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &Success);
		if(!Success)
		{
			glGetShaderInfoLog(VertexShader, InfoLogLength, NULL, InfoLog);
			LOG_ERROR("Vertex shader compilation failed.. %s/n", InfoLog);
		}
		
		uint32 FragmentShader;
		FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(FragmentShader, 1, &FragmentShaderSource, NULL);
		glCompileShader(FragmentShader);
		
		glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &Success);
		if(!Success)
		{
			glGetShaderInfoLog(FragmentShader, InfoLogLength, NULL, InfoLog);
			LOG_ERROR("Fragment shader compilation failed.. %s/n", InfoLog);
		}
		
		uint32 ShaderProgram;
		ShaderProgram = glCreateProgram();
		
		glAttachShader(ShaderProgram, VertexShader);
		glAttachShader(ShaderProgram, FragmentShader);
		glLinkProgram(ShaderProgram);
		
		glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
		if(!Success)
		{
			glGetProgramInfoLog(ShaderProgram, InfoLogLength, NULL, InfoLog);
			LOG_ERROR("Shader program linking failed.. %s/n", InfoLog);
		}
		
		TempMemoryArenaEnd(TempArena);
		
		glUseProgram(ShaderProgram);
		glUniform1i(glGetUniformLocation(ShaderProgram, "texture1"), 0);
		glUniform1i(glGetUniformLocation(ShaderProgram, "texture2"), 1);
#endif
		
        while(Game->Running)
        {
            FPSTimerUpdate(FPSTimer);
            
            HandleInput(Game);
            
#if GPU_RENDERED
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			
			glUseProgram(ShaderProgram);
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, Texture1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, Texture2);
			
			glBindVertexArray(VAO);
			
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			
			glBindVertexArray(0);
			glBindTexture(GL_TEXTURE_2D, 0);
			
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
			
            UpdatePhysicsWorld2d(Game->World, (real32)(FPSTimer->DeltaTicks / 1000.0f), 128);
            
			DeleteOutofReachPhysicsBodies(Game);
			
            ClearRenderer(Game->Renderer, BLACK);
            
            // NOTE: Render all the physics bodies
            for(uint32 i = 0; 
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
			
#if SHOW_CONTACT_POINTS
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
