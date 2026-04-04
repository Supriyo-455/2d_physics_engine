/* date = March 22nd 2026 8:57 pm */

#ifndef PHYSICS2_D_H
#define PHYSICS2_D_H

#include "simple_math.h"

// NOTE: Size unit - Meters
const real32 MinBodySize = 0.01f * 0.01f;
const real32 MaxBodySize = 64.0f * 64.0f;

// NOTE: Density unit - gm/cm^3
const real32 MinDensity = 0.5f;
const real32 MaxDensity = 21.4f;

typedef enum
{
    CIRCLE = 0,
    BOX = 1
}shape;

typedef struct
{
    vec2 LinearVelocity;
    vec2 Position;
    
    real32 Rotation;
    real32 RotationalVelocity;
    
    real32 Mass;
    real32 Density;
    real32 Restitution;
    real32 Area;
    
    bool32 IsStatic;
    bool32 IsCollided;
    
    shape Shape;
    
    // NOTE: This informations will change based on shape
    real32 Radius;
    real32 Width;
    real32 Height;
    
    // NOTE: For now we only supporting boxes
    vec2 Vertices[4];
    vec2 TransformedVertices[4];
    bool32 TransformUpdateRequired;
    
    // NOTE: 2 triangles, each has 3 vertices
    int Triangles[6];
}physics_body2D;

bool32
IsPhysicsBodyValid(real32 Area, real32 Density)
{    
    if(Area < MinBodySize)
    {
        LOG_ERROR("Unable to initialize physics body with area, %0.2f; Where min-area allowed is %0.2f\n", Area, MinBodySize);
        
        // NOTE: Empty body 
        return false;
    }
    
    if(Area > MaxBodySize)
    {
        LOG_ERROR("Unable to initialize physics body with area, %0.2f; Where max-area allowed is %0.2f\n", Area, MaxBodySize);
        
        // NOTE: Empty body 
        return false;
    }
    
    if(Density < MinDensity)
    {
        LOG_ERROR("Unable to initialize physics body with density, %0.2f; Where min-density allowed is %0.2f\n", Density, MinDensity);
        
        // NOTE: Empty body 
        return false;
    }
    
    if(Density > MaxDensity)
    {
        LOG_ERROR("Unable to initialize physics body with density, %0.2f; Where max-density allowed is %0.2f\n", Density, MaxDensity);
        
        // NOTE: Empty body 
        return false;
    }
    
    return true;
}

physics_body2D 
CreateCirclePhysicsBody2D(vec2 Position, real32 Radius, real32 Density, real32 Restitution = 0.0f, bool32 IsStatic = false)
{   
    physics_body2D Body = {0};
    real32 Area = PI * Radius * Radius;
    real32 Mass = Density * Area;
    
    Assert(IsPhysicsBodyValid(Area, Density));
    
    Body.RotationalVelocity = 0.0f;
    Body.LinearVelocity = vec(0.0f, 0.0f);
    Body.Position = Position;
    Body.Rotation = 0.0f;
    Body.Mass = Mass;
    Body.Density = Density;
    Body.Restitution = Clamp(Restitution, 0.0f, 1.0f);
    Body.Area = Area;
    Body.IsStatic = IsStatic;
    Body.Shape = CIRCLE;
    Body.Radius = Radius;
    
    return Body;
}

physics_body2D 
CreateBoxPhysicsBody2D(vec2 Position, real32 Width, real32 Height, real32 Density, real32 Restitution = 0.0f, bool32 IsStatic = false)
{   
    physics_body2D Body = {0};
    real32 Area = Width * Height;
    real32 Mass = Density * Area;
    
    Assert(IsPhysicsBodyValid(Area, Density));
    
    Body.RotationalVelocity = 0.0f;
    Body.LinearVelocity = vec(0.0f, 0.0f);
    Body.Position = Position;
    Body.Rotation = 0.0f;
    Body.Mass = Mass;
    Body.Density = Density;
    Body.Restitution = Clamp(Restitution, 0.0f, 1.0f);
    Body.Area = Area;
    Body.IsStatic = IsStatic;
    Body.Shape = BOX;
    Body.Width = Width;
    Body.Height = Height;
    
    Body.IsCollided = false;
    
    real32 Left = Body.Position.x - Width / 2.0f;
    real32 Rigth = Left + Width;
    real32 Top = Body.Position.y - Height / 2.0f;
    real32 Bottom = Top + Height;
    
    Body.Vertices[0] = vec(Left, Top);
    Body.Vertices[1] = vec(Left, Bottom);
    Body.Vertices[2] = vec(Rigth, Bottom);
    Body.Vertices[3] = vec(Rigth, Top);
    
    memset(Body.TransformedVertices, 0, ARRAY_COUNT(Body.TransformedVertices));
    Body.TransformUpdateRequired = true;
    
    Body.Triangles[0] = 0;
    Body.Triangles[1] = 1;
    Body.Triangles[2] = 2;
    Body.Triangles[3] = 2;
    Body.Triangles[4] = 3;
    Body.Triangles[5] = 0;
    
    return Body;
}

inline void
MovePhysicsBody(physics_body2D* Body)
{
    Body->Position = Body->Position + Body->LinearVelocity;
    Body->TransformUpdateRequired = true;
}

inline void
RotatePhysicsBody(physics_body2D* Body)
{
    Body->Rotation = Body->Rotation + Body->RotationalVelocity;
    Body->TransformUpdateRequired = true;
}

vec2*
GetPhysicsBodyTransformedVertices(physics_body2D* Body)
{
    if(Body->TransformUpdateRequired)
    {
        for(int i=0; i<ARRAY_COUNT(Body->Vertices); i++)
        {
            Body->TransformedVertices[i] = Transform(Body->Vertices[i], Body->Position, Body->LinearVelocity, Body->RotationalVelocity);
        }
        Body->TransformUpdateRequired = false;
    }
    
    return Body->TransformedVertices;
}

inline void
ProjectVertices(vec2* Vertices, int VerticesCount, vec2 Axis, real32* Min, real32* Max)
{
    *Min = FLT_MAX;
    *Max = FLT_MIN;
    
    for(int i=0; i<VerticesCount; i++)
    {
        vec2 V = Vertices[i];
        real32 Proj = Dot(V, Axis);
        
        if(Proj < *Min)
            *Min = Proj;
        if(Proj > *Max)
            *Max = Proj;
    }
}

inline bool32
IntersectPolygons(physics_body2D* BodyA, physics_body2D* BodyB,
                  vec2* OutNormal, real32* OutDepth)
{
    *OutNormal = vec(0.0f, 0.0f);
    *OutDepth = FLT_MAX;
    
    for(int i=0; i<ARRAY_COUNT(BodyA->Vertices); i++)
    {
        int Next = (i + 1) % ARRAY_COUNT(BodyA->Vertices);
        vec2 Va = BodyA->Vertices[i];
        vec2 Vb = BodyA->Vertices[Next];
        
        vec2 Edge = Vb - Va;
        // NOTE: Axis is the normal vector of Edge
        vec2 Axis = vec(-Edge.y, Edge.x);
        Axis = Normalize(Axis);
        
        real32 MinA, MaxA, MinB, MaxB;
        
        ProjectVertices(BodyA->Vertices, ARRAY_COUNT(BodyA->Vertices), Axis, &MinA, &MaxA);
        ProjectVertices(BodyB->Vertices, ARRAY_COUNT(BodyB->Vertices), Axis, &MinB, &MaxB);
        
        if(MinA >= MaxB || MinB >= MaxA)
        {
            return false;
        }
        
        real32 AxisDepth = MIN(MaxB - MinA, MaxA - MinB);
        if(AxisDepth < *OutDepth)
        {
            *OutDepth = AxisDepth;
            *OutNormal = Axis;
        }
    }
    
    for(int i=0; i<ARRAY_COUNT(BodyB->Vertices); i++)
    {
        int Next = (i + 1) % ARRAY_COUNT(BodyB->Vertices);
        vec2 Va = BodyB->Vertices[i];
        vec2 Vb = BodyB->Vertices[Next];
        
        vec2 Edge = Vb - Va;
        // NOTE: Axis is the normal vector of Edge
        vec2 Axis = vec(-Edge.y, Edge.x);
        Axis = Normalize(Axis);
        
        real32 MinA, MaxA, MinB, MaxB;
        
        ProjectVertices(BodyA->Vertices, ARRAY_COUNT(BodyA->Vertices), Axis, &MinA, &MaxA);
        ProjectVertices(BodyB->Vertices, ARRAY_COUNT(BodyB->Vertices), Axis, &MinB, &MaxB);
        
        if(MinA >= MaxB || MinB >= MaxA)
        {
            return false;
        }
        
        
        real32 AxisDepth = MIN(MaxB - MinA, MaxA - MinB);
        if(AxisDepth < *OutDepth)
        {
            *OutDepth = AxisDepth;
            *OutNormal = Axis;
        }
    }
    
    vec2 Direction = BodyB->Position - BodyA->Position;
    if(Dot(Direction, *OutNormal) < 0.0f)
    {
        *OutNormal *= -1.0f;
    }
    
    return true;
}

inline bool32
IntersectCircles(vec2 PosA, real32 RadiusA, 
                 vec2 PosB, real32 RadiusB, 
                 vec2* OutNormal, real32* OutDepth)
{
    *OutNormal = vec(0.0f, 0.0f);
    *OutDepth = 0.0f;
    
    real32 distance = Distance(PosA, PosB);
    real32 Radii = RadiusA + RadiusB;
    
    if(distance >= Radii)
    {
        return false;
    }
    
    *OutNormal = Normalize(PosB - PosA);
    *OutDepth = Radii - distance;
    
    return true;
}

inline bool32
CheckCollision2D(physics_body2D* A, physics_body2D* B, vec2* OutNormal, real32* OutDepth)
{
    bool32 Result = false;
    
    if(A->Shape == CIRCLE && B->Shape == CIRCLE)
    {
        Result = IntersectCircles(A->Position, A->Radius, 
                                  B->Position, B->Radius, 
                                  OutNormal, OutDepth);
    }
    else if(A->Shape == BOX && B->Shape == BOX)
    {
        Result = IntersectPolygons(A, B,
                                   OutNormal, OutDepth);
    }
    else
    {
        // NOTE: Either one of them is box and circle
    }
    
    return Result;
}

void
Update2DPhysicsBodies(physics_body2D* Bodies, int BodyCount)
{
    vec2 OutNormal = vec(0.0f, 0.0f);
    real32 OutDepth = 0.0f;
    
    // NOTE: Reset collision state
    for(int i=0; i<BodyCount; i++)
    {
        Bodies[i].IsCollided = false;
    }
    
    for(int i=0; i<BodyCount-1; i++)
    {
        physics_body2D* BodyA = &Bodies[i];
        
        for(int j=i+1; j<BodyCount; j++)
        {
            physics_body2D* BodyB = &Bodies[j];
            
            if(CheckCollision2D(BodyA, BodyB, &OutNormal, &OutDepth))
            {
                BodyA->IsCollided = true;
                BodyB->IsCollided = true;
                
                // TODO: Box collisions not working properly
                BodyB->LinearVelocity = OutNormal * (OutDepth / 2.0f);
                BodyA->LinearVelocity = -OutNormal * (OutDepth / 2.0f);
#if 0
                // NOTE: Fix overlap immediately
                BodyA->Position = BodyA->Position - (OutNormal * (OutDepth / 2.0f));
                BodyB->Position = BodyB->Position + (OutNormal * (OutDepth / 2.0f));
                
                // NOTE: Reflect velocities so they bounce apart,
                // Project each velocity onto the collision normal
                real32 VelAlongNormalA = Dot(BodyA->LinearVelocity, OutNormal);
                real32 VelAlongNormalB = Dot(BodyB->LinearVelocity, OutNormal);
                
                // NOTE: Check if bodies are actually moving toward each other
                if(VelAlongNormalA - VelAlongNormalB > 0)
                {
                    BodyA->LinearVelocity = BodyA->LinearVelocity - (OutNormal * (VelAlongNormalA * BodyA->Restitution));
                    BodyB->LinearVelocity = BodyB->LinearVelocity + (OutNormal * (VelAlongNormalB * BodyB->Restitution));
                }
#endif
            }
        }
    }
}

#endif //PHYSICS2_D_H
