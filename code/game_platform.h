#ifndef GAME_PLATFORM_H
#define GAME_PLATFORM_H

#include "defines.h"
#include "simple_math.h"
#include "performance.h"

struct game_button_state
{
    int HalfTransitionCount;
    bool32 EndedDown;
};

struct game_input
{
    real32 dtForFrame;
    
    vec2 MousePos;
    vec2 MouseRel;
    
    int32 MouseWheelDelta;
    game_button_state MouseButtons[3];
    
    union
    {
        game_button_state Buttons[16];
        struct
        {
            game_button_state SpawnCircle;
            game_button_state SpawnBox;
            game_button_state Reset;
            game_button_state ToggleWireframe;
            game_button_state HighlightCollisions;
            game_button_state MoveLeft;
            game_button_state MoveRight;
            game_button_state MoveUp;
            game_button_state MoveDown;
        };
    };
};

inline bool32 
WasPressed(game_button_state State)
{
    return (State.HalfTransitionCount > 1 || 
            (State.HalfTransitionCount == 1 && State.EndedDown));
}

struct game_memory
{
    bool32 IsInitialized;
    
    uint64 PermanentStorageSize;
    void*  PermanentStorage;
    
    uint64 TransientStorageSize;
    void*  TransientStorage;
    
    uint32 DebugRecordCount;
    debug_record* DebugRecords;
};

enum render_command_type
{
    RenderCommand_Clear,
    RenderCommand_Quad,
    RenderCommand_Circle,
    RenderCommand_Line,
    RenderCommand_Text,
};

struct render_command_header
{
    render_command_type Type;
};

struct render_command_clear
{
    vec4 Color;
};

struct render_command_quad
{
    vec2 Vertices[4];
    
    vec4 FillColor;
    vec4 OutlineColor;
    bool32 HasOutline;
};

struct render_command_circle
{
    vec2 Position;
    real32 Radius;
    real32 Angle;
    
    vec4 FillColor;
    vec4 OutlineColor;
    bool32 HasOutline;
};

struct render_command_line
{
    vec2 From;
    vec2 To;
    vec4 Color;
    
    // TODO: Line Thickness?
    // real32 Thickness;
};

struct render_command_text
{
    vec2 Position;
    vec4 Color;
    uint32 FontSize;
    char Text[128];
};

struct game_render_commands
{
    uint32 Width;
    uint32 Height;
    
    uint32 MaxBufferSize;
    uint32 CurrentBufferSize;
    uint8* BufferBase;
    
    uint32 CommandCount;
};

#define PushRenderCommand(Commands, type) \
(type*)PushRenderCommand_(Commands, sizeof(type), RenderCommand_##type)

inline void* 
PushRenderCommand_(game_render_commands* Commands, uint32 Size, render_command_type Type)
{
    void* Result = NULL;
    
    uint32 TotalSize = sizeof(render_command_header) + Size;
    if(Commands->CurrentBufferSize + TotalSize <= Commands->MaxBufferSize)
    {
        render_command_header* Header = (render_command_header*)(Commands->BufferBase + Commands->CurrentBufferSize);
        Header->Type = Type;
        Result = (void*)(Header + 1);
        
        Commands->CurrentBufferSize += TotalSize;
        Commands->CommandCount++;
    }
    else
    {
        LOG_ERROR("Render command buffer overflow!");
    }
    
    return Result;
}

inline void 
PushClear(game_render_commands* Commands, vec4 Color)
{
    render_command_clear* Cmd = (render_command_clear*)PushRenderCommand_(Commands, sizeof(render_command_clear), RenderCommand_Clear);
    
    if(Cmd)
    {
        Cmd->Color = Color;
    }
}

inline void 
PushQuad(game_render_commands* Commands, vec2 Verts[4], vec4 FillColor, vec4 OutlineColor = {}, bool32 HasOutline = false)
{
    render_command_quad* Cmd = (render_command_quad*)PushRenderCommand_(Commands, sizeof(render_command_quad), RenderCommand_Quad);
    
    if(Cmd)
    {
        for(int i = 0; i < 4; i++) 
        {
            Cmd->Vertices[i] = Verts[i];
        }
        Cmd->FillColor = FillColor;
        Cmd->OutlineColor = OutlineColor;
        Cmd->HasOutline = HasOutline;
    }
}

inline void 
PushCircle(game_render_commands* Commands, vec2 Pos, real32 Radius, real32 Angle, vec4 FillColor, vec4 OutlineColor = {}, bool32 HasOutline = false)
{
    render_command_circle* Cmd = (render_command_circle*)PushRenderCommand_(Commands, sizeof(render_command_circle), RenderCommand_Circle);
    
    if(Cmd)
    {
        Cmd->Position = Pos;
        Cmd->Radius = Radius;
        Cmd->Angle = Angle;
        Cmd->FillColor = FillColor;
        Cmd->OutlineColor = OutlineColor;
        Cmd->HasOutline = HasOutline;
    }
}

inline void 
PushBox(game_render_commands* Commands, 
        vec2 Center, 
        real32 Width, 
        real32 Height, 
        real32 Angle, 
        vec4 FillColor, 
        vec4 OutlineColor = {}, 
        bool32 HasOutline = false)
{
    real32 HalfW = Width * 0.5f;
    real32 HalfH = Height * 0.5f;
    real32 C = Cos(Angle);
    real32 S = Sin(Angle);
    
    vec2 Local[4] = {
        vec(-HalfW, -HalfH),
        vec( HalfW, -HalfH),
        vec( HalfW,  HalfH),
        vec(-HalfW,  HalfH)
    };
    
    vec2 Verts[4];
    for(int i = 0; i < 4; i++)
    {
        Verts[i].x = (C * Local[i].x - S * Local[i].y) + Center.x;
        Verts[i].y = (S * Local[i].x + C * Local[i].y) + Center.y;
    }
    
    PushQuad(Commands, Verts, FillColor, OutlineColor, HasOutline);
}

inline void 
PushLine(game_render_commands* Commands, vec2 From, vec2 To, vec4 Color)
{
    render_command_line* Cmd = (render_command_line*)PushRenderCommand_(Commands, sizeof(render_command_line), RenderCommand_Line);
    
    if(Cmd)
    {
        Cmd->From = From;
        Cmd->To = To;
        Cmd->Color = Color;
    }
}

inline void 
PushText(game_render_commands* Commands, vec2 Pos, const char* Text, vec4 Color, uint32 FontSize = 24)
{
    render_command_text* Cmd = (render_command_text*)PushRenderCommand_(Commands, sizeof(render_command_text), RenderCommand_Text);
    
    if(Cmd)
    {
        Cmd->Position = Pos;
        Cmd->Color = Color;
        Cmd->FontSize = FontSize;
        snprintf(Cmd->Text, sizeof(Cmd->Text), "%s", Text);
    }
}

#define GAME_UPDATE_AND_RENDER(name) void name(game_memory* Memory, game_input* Input, game_render_commands* RenderCommands)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);

#endif