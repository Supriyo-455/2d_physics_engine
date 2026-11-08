#include "defines.h"
#include "game_platform.h"
#include <sys/stat.h>


#define WINDOW_WIDTH 1300
#define WINDOW_HEIGHT 800

#if defined(_WIN32)

#define GAME_DLL_SOURCE "game.dll"
#define GAME_DLL_TEMP   "game_temp.dll"

#else

#define GAME_DLL_SOURCE "./game.so"
#define GAME_DLL_TEMP   "./game_temp.so"

#endif

struct game_code
{
    void* Library;
    time_t LastWriteTime;
    game_update_and_render* UpdateAndRender;
    bool32 IsValid;
};

time_t GetFileLastWriteTime(const char* Filename)
{
    struct stat Result;
    if(stat(Filename, &Result) == 0)
    {
        return Result.st_mtime;
    }
    return 0;
}

bool32 CopyFile(const char* Source, const char* Dest)
{
    FILE* SrcFile = fopen(Source, "rb");
    if(!SrcFile)
    {
        LOG_WARNING("Could not open source file %s for copying", Source);
        return false;
    }
    
    FILE* DstFile = fopen(Dest, "wb");
    if(!DstFile) 
    { 
        fclose(SrcFile);
        LOG_WARNING("Could not open destination file %s for copying", Dest);
        return false; 
    }
    
    char Buffer[4096];
    size_t Bytes;
    while((Bytes = fread(Buffer, 1, sizeof(Buffer), SrcFile)) > 0)
    {
        fwrite(Buffer, 1, Bytes, DstFile);
    }
    
    fclose(SrcFile);
    fclose(DstFile);
    return true;
}

game_code LoadGameCode(const char* SourceDLL, const char* TempDLL)
{
    game_code Result = {};
    Result.LastWriteTime = GetFileLastWriteTime(SourceDLL);
    
    if(!CopyFile(SourceDLL, TempDLL))
    {
        LOG_WARNING("Failed to copy %s to %s", SourceDLL, TempDLL);
    }
    
    Result.Library = SDL_LoadObject(TempDLL);
    if(Result.Library)
    {
        Result.UpdateAndRender = (game_update_and_render*)
            SDL_LoadFunction(Result.Library, "GameUpdateAndRender");
        
        Result.IsValid = (Result.UpdateAndRender != NULL);
    }
    else
    {
        LOG_ERROR("SDL_LoadObject failed for %s! Error: %s", TempDLL, SDL_GetError());
    }
    
    if(Result.IsValid)
    {
        LOG_INFO("Successfully loaded game code from %s (running as %s)", SourceDLL, TempDLL);
    }
    else
    {
        LOG_ERROR("Failed to resolve 'GameUpdateAndRender' symbol from %s", TempDLL);
    }
    
    return Result;
}

void UnloadGameCode(game_code* Code)
{
    if(Code->Library)
    {
        SDL_UnloadObject(Code->Library);
        Code->Library = NULL;
    }
    
    Code->IsValid = false;
    Code->UpdateAndRender = NULL;
    LOG_INFO("Unloaded game code library");
}

inline SDL_Color ToSDLColor(vec4 C)
{
    SDL_Color Color;
    
    Color.r = (uint8)(C.r * 255.0f);
    Color.g = (uint8)(C.g * 255.0f);
    Color.b = (uint8)(C.b * 255.0f);
    Color.a = (uint8)(C.a * 255.0f);
    
    return Color;
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

void DrawFilledCircle(SDL_Renderer* Renderer, int32 CX, int32 CY, int32 Radius, vec4 Color)
{
    SDL_Color C = ToSDLColor(Color);
    
    SDL_SetRenderDrawColor(Renderer, C.r, C.g, C.b, C.a);
    
    int32 X = 0;
    int32 Y = -Radius;
    int32 P = -Radius;
    
    while(X < -Y)
    {
        if(P > 0)
        {
            Y++;
            P += 2 * (X + Y) + 1;
        }
        
        else
        {
            P += 2 * X + 1;
        }
        
        SDL_RenderDrawLine(Renderer, CX - X, CY + Y, CX + X, CY + Y);
        SDL_RenderDrawLine(Renderer, CX - X, CY - Y, CX + X, CY - Y);
        SDL_RenderDrawLine(Renderer, CX - Y, CY + X, CX + Y, CY + X);
        SDL_RenderDrawLine(Renderer, CX - Y, CY - X, CX + Y, CY - X);
        
        X++;
    }
}

void DrawHollowCircle(SDL_Renderer* Renderer, int32 CX, int32 CY, int32 Radius, vec4 Color)
{
    SDL_Color C = ToSDLColor(Color);
    
    SDL_SetRenderDrawColor(Renderer, C.r, C.g, C.b, C.a);
    
    int32 X = 0;
    int32 Y = -Radius;
    int32 P = -Radius;
    
    while(X < -Y)
    {
        if(P > 0)
        {
            Y++;
            P += 2 * (X + Y) + 1;
        }
        
        else
        {
            P += 2 * X + 1;
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

void ExecuteRenderCommands(SDL_Renderer* Renderer, TTF_Font* Font, game_render_commands* Commands)
{
    uint32 Offset = 0;
    
    for(uint32 i = 0; i < Commands->CommandCount; i++)
    {
        render_command_header* Header = (render_command_header*)(Commands->BufferBase + Offset);
        Offset += sizeof(render_command_header);
        
        switch(Header->Type)
        {
            case RenderCommand_Clear:
            {
                render_command_clear* Cmd = (render_command_clear*)(Commands->BufferBase + Offset);
                
                SDL_Color C = ToSDLColor(Cmd->Color);
                
                SDL_SetRenderDrawColor(Renderer, C.r, C.g, C.b, C.a);
                
                SDL_RenderClear(Renderer);
                
                Offset += sizeof(render_command_clear);
            }
            break;
            
            case RenderCommand_Quad:
            {
                render_command_quad* Cmd = (render_command_quad*)(Commands->BufferBase + Offset);
                
                SDL_Vertex Verts[4] = {};
                SDL_Color Fill = ToSDLColor(Cmd->FillColor);
                
                for(int v = 0; v < 4; v++)
                {
                    Verts[v].position = { Cmd->Vertices[v].x, Cmd->Vertices[v].y };
                    Verts[v].color = Fill;
                }
                
                int Indices[] = { 0, 1, 3, 1, 2, 3 };
                SDL_RenderGeometry(Renderer, NULL, Verts, 4, Indices, 6);
                if(Cmd->HasOutline)
                {
                    SDL_Color Out = ToSDLColor(Cmd->OutlineColor);
                    SDL_SetRenderDrawColor(Renderer, Out.r, Out.g, Out.b, Out.a);
                    
                    for(int v = 0; v < 4; v++)
                    {
                        int next = (v + 1) % 4;
                        SDL_RenderDrawLineF(Renderer, Cmd->Vertices[v].x, Cmd->Vertices[v].y,
                                            Cmd->Vertices[next].x, Cmd->Vertices[next].y);
                    }
                }
                
                Offset += sizeof(render_command_quad);
            } 
            break;
            
            case RenderCommand_Circle:
            {
                render_command_circle* Cmd = (render_command_circle*)(Commands->BufferBase + Offset);
                
                int32 CX = (int32)Cmd->Position.x;
                int32 CY = (int32)Cmd->Position.y;
                
                int32 R  = (int32)Cmd->Radius;
                
                DrawFilledCircle(Renderer, CX, CY, R, Cmd->FillColor);
                
                if(Cmd->HasOutline)
                {
                    DrawHollowCircle(Renderer, CX, CY, R, Cmd->OutlineColor);
                }
                
                // NOTE: Orientation line for the circle
                SDL_Color Out = ToSDLColor(Cmd->OutlineColor);
                SDL_SetRenderDrawColor(Renderer, Out.r, Out.g, Out.b, Out.a);
                
                real32 TargetX = Cmd->Position.x + Cmd->Radius * Cos(Cmd->Angle);
                real32 TargetY = Cmd->Position.y + Cmd->Radius * Sin(Cmd->Angle);
                
                SDL_RenderDrawLineF(Renderer, Cmd->Position.x, Cmd->Position.y, TargetX, TargetY);
                
                Offset += sizeof(render_command_circle);
            } 
            break;
            
            case RenderCommand_Line:
            {
                render_command_line* Cmd = (render_command_line*)(Commands->BufferBase + Offset);
                
                SDL_Color C = ToSDLColor(Cmd->Color);
                
                SDL_SetRenderDrawColor(Renderer, C.r, C.g, C.b, C.a);
                
                SDL_RenderDrawLineF(Renderer, Cmd->From.x, Cmd->From.y, Cmd->To.x, Cmd->To.y);
                
                Offset += sizeof(render_command_line);
            }
            break;
            
            case RenderCommand_Text:
            {
                render_command_text* Cmd = (render_command_text*)(Commands->BufferBase + Offset);
                
                if(Font)
                {
                    SDL_Color SdlColor = ToSDLColor(Cmd->Color);
                    SDL_Surface* Surface = TTF_RenderText_Blended(Font, Cmd->Text, SdlColor);
                    
                    if(Surface)
                    {
                        SDL_Texture* Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
                        
                        if(Texture)
                        {
                            SDL_Rect DestRect = { (int)Cmd->Position.x, (int)Cmd->Position.y, Surface->w, Surface->h };
                            SDL_RenderCopy(Renderer, Texture, NULL, &DestRect);
                            SDL_DestroyTexture(Texture);
                        }
                        
                        SDL_FreeSurface(Surface);
                    }
                }
                Offset += sizeof(render_command_text);
            }
            break;
        }
    }
}

inline void ProcessButton(game_button_state* NewState, bool32 IsDown)
{
    if(NewState->EndedDown != IsDown)
    {
        NewState->HalfTransitionCount++;
        NewState->EndedDown = IsDown;
    }
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    LOG_INFO("Starting 2D Physics Engine Platform Host...");
    
    uint64 PermanentStorageSize = Megabytes(256);
    uint64 TransientStorageSize = Megabytes(64);
    uint64 TotalSize = PermanentStorageSize + TransientStorageSize;
    
    void* MemoryBlock = calloc(1, TotalSize);
    if(!MemoryBlock)
    {
        LOG_ERROR("Failed to allocate %llu MB of memory!", TotalSize / (1024 * 1024));
        return -1;
    }
    
    game_memory Memory = {};
    Memory.PermanentStorageSize = PermanentStorageSize;
    Memory.PermanentStorage     = MemoryBlock;
    Memory.TransientStorageSize = TransientStorageSize;
    Memory.TransientStorage     = (uint8*)MemoryBlock + PermanentStorageSize;
    Memory.IsInitialized        = false;
    
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        LOG_ERROR("SDL_Init failed: %s", SDL_GetError());
        return -1;
    }
    
    if(TTF_Init() == -1)
    {
        LOG_WARNING("SDL_ttf could not initialize! SDL_ttf Error: %s", TTF_GetError());
    }
    
    SDL_Window* Window = SDL_CreateWindow("2D Physics Engine - Live Coding",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if(!Window)
    {
        LOG_ERROR("Window creation failed! SDL Error: %s", SDL_GetError());
        return -1;
    }
    
    SDL_Renderer* Renderer = SDL_CreateRenderer(Window, -1, 
                                                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if(!Renderer)
    {
        LOG_ERROR("Renderer creation failed! SDL Error: %s", SDL_GetError());
        return -1;
    }
    
    char* BasePath = SDL_GetBasePath();
    char SourceDLLPath[1024];
    char TempDLLPath[1024];
    char LockFilePath[1024];
    if(BasePath)
    {
        snprintf(SourceDLLPath, sizeof(SourceDLLPath), "%s%s", BasePath, GAME_DLL_SOURCE);
        snprintf(TempDLLPath, sizeof(TempDLLPath), "%s%s", BasePath, GAME_DLL_TEMP);
        snprintf(LockFilePath, sizeof(LockFilePath), "%slock.tmp", BasePath);
    }
    else
    {
        snprintf(SourceDLLPath, sizeof(SourceDLLPath), "%s", GAME_DLL_SOURCE);
        snprintf(TempDLLPath, sizeof(TempDLLPath), "%s", GAME_DLL_TEMP);
        snprintf(LockFilePath, sizeof(LockFilePath), "lock.tmp");
    }
    
    char FontPath[1024] = "assets/font/Consolas-Regular.ttf";
    TTF_Font* Font = TTF_OpenFont(FontPath, 20);
    if(!Font && BasePath)
    {
        snprintf(FontPath, sizeof(FontPath), "%sassets/font/Consolas-Regular.ttf", BasePath);
        Font = TTF_OpenFont(FontPath, 20);
    }
    if(!Font)
    {
        LOG_WARNING("Failed to load font from %s", FontPath);
    }
    
    game_code GameCode = LoadGameCode(SourceDLLPath, TempDLLPath);
    
    uint64 PerfFreq = SDL_GetPerformanceFrequency();
    uint64 LastCounter = SDL_GetPerformanceCounter();
    
    game_input Input[2] = {};
    game_input* OldInput = &Input[0];
    game_input* NewInput = &Input[1];
    
    bool32 Running = true;
    
    while(Running)
    {
        time_t WriteTime = GetFileLastWriteTime(SourceDLLPath);
        if(WriteTime != GameCode.LastWriteTime && WriteTime != 0)
        {
            struct stat LockStat;
            bool32 IsLocked = (stat(LockFilePath, &LockStat) == 0);
            
            if(!IsLocked)
            {
                LOG_INFO("Code change detected on disk. Hot-reloading %s...", SourceDLLPath);
                UnloadGameCode(&GameCode);
                GameCode = LoadGameCode(SourceDLLPath, TempDLLPath);
            }
        }
        
        for(int i = 0; i < (int)ARRAY_COUNT(NewInput->Buttons); ++i)
        {
            NewInput->Buttons[i].EndedDown = OldInput->Buttons[i].EndedDown;
            NewInput->Buttons[i].HalfTransitionCount = 0;
        }
        for(int i = 0; i < 3; ++i)
        {
            NewInput->MouseButtons[i].EndedDown = OldInput->MouseButtons[i].EndedDown;
            NewInput->MouseButtons[i].HalfTransitionCount = 0;
        }
        
        NewInput->MouseWheelDelta = 0;
        NewInput->MouseRel = vec(0.0f, 0.0f);
        
        int MouseX = 0, MouseY = 0;
        SDL_GetMouseState(&MouseX, &MouseY);
        NewInput->MousePos = vec((real32)MouseX, (real32)MouseY);
        
        SDL_Event Event;
        while(SDL_PollEvent(&Event))
        {
            switch(Event.type)
            {
                case SDL_QUIT:
                {
                    LOG_INFO("Quit event received. Exiting...");
                    Running = false;
                } 
                break;
                
                case SDL_MOUSEMOTION:
                {
                    NewInput->MousePos = vec((real32)Event.motion.x, (real32)Event.motion.y);
                    NewInput->MouseRel.x += (real32)Event.motion.xrel;
                    NewInput->MouseRel.y += (real32)Event.motion.yrel;
                } 
                break;
                
                case SDL_MOUSEWHEEL:
                {
                    NewInput->MouseWheelDelta += Event.wheel.y;
                } 
                break;
                
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                {
                    bool32 IsDown = (Event.type == SDL_MOUSEBUTTONDOWN);
                    if(Event.button.button == SDL_BUTTON_LEFT)
                        ProcessButton(&NewInput->MouseButtons[0], IsDown);
                    else if(Event.button.button == SDL_BUTTON_RIGHT)
                        ProcessButton(&NewInput->MouseButtons[1], IsDown);
                    else if(Event.button.button == SDL_BUTTON_MIDDLE)
                        ProcessButton(&NewInput->MouseButtons[2], IsDown);
                } 
                break;
                
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                {
                    bool32 IsDown = (Event.key.state == SDL_PRESSED);
                    SDL_Keycode Key = Event.key.keysym.sym;
                    if(Key == SDLK_c)         ProcessButton(&NewInput->SpawnCircle, IsDown);
                    if(Key == SDLK_b)         ProcessButton(&NewInput->SpawnBox, IsDown);
                    if(Key == SDLK_r)         ProcessButton(&NewInput->Reset, IsDown);
                    if(Key == SDLK_SPACE)     ProcessButton(&NewInput->ToggleWireframe, IsDown);
                    if(Key == SDLK_LEFT)      ProcessButton(&NewInput->MoveLeft, IsDown);
                    if(Key == SDLK_RIGHT)     ProcessButton(&NewInput->MoveRight, IsDown);
                    if(Key == SDLK_UP)        ProcessButton(&NewInput->MoveUp, IsDown);
                    if(Key == SDLK_DOWN)      ProcessButton(&NewInput->MoveDown, IsDown);
                    if(Key == SDLK_ESCAPE)
                    {
                        LOG_INFO("Escape pressed. Exiting...");
                        Running = false;
                    }
                } 
                break;
            }
        }
        
        uint64 CurrentCounter = SDL_GetPerformanceCounter();
        NewInput->dtForFrame = (real32)((real64)(CurrentCounter - LastCounter) / (real64)PerfFreq);
        LastCounter = CurrentCounter;
        
        game_render_commands RenderCommands = {};
        RenderCommands.Width = WINDOW_WIDTH;
        RenderCommands.Height = WINDOW_HEIGHT;
        RenderCommands.MaxBufferSize = Megabytes(4);
        RenderCommands.BufferBase = (uint8*)Memory.TransientStorage;
        RenderCommands.CurrentBufferSize = 0;
        RenderCommands.CommandCount = 0;
        
        if(GameCode.IsValid)
        {
            GameCode.UpdateAndRender(&Memory, NewInput, &RenderCommands);
        }
        
        ExecuteRenderCommands(Renderer, Font, &RenderCommands);
        
        SDL_RenderPresent(Renderer);
        
        game_input* Temp = OldInput;
        OldInput = NewInput;
        NewInput = Temp;
    }
    
    LOG_INFO("Cleaning up and shutting down...");
    UnloadGameCode(&GameCode);
    if(Font) TTF_CloseFont(Font);
    if(BasePath) SDL_free(BasePath);
    TTF_Quit();
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
    SDL_Quit();
    free(MemoryBlock);
    
    return 0;
}