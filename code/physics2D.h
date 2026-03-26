/* date = March 22nd 2026 8:57 pm */

#ifndef PHYSICS2_D_H
#define PHYSICS2_D_H

#include "simple_math.h"

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
    
    shape Shape;
    
    // NOTE: This informations will change based on shape
    real32 Radius;
    real32 Width;
    real32 Height;
}physics_body2D;

bool32
IsPhysicsBodyValid(real32 Area, real32 Density)
{    
    if(Area < MinBodySize)
    {
        fprintf(stderr, "Unable to initialize physics body with area, %0.2f; Where min-area allowed is %0.2f\n", Area, MinBodySize);
        
        // NOTE: Empty body 
        return false;
    }
    
    if(Area > MaxBodySize)
    {
        fprintf(stderr, "Unable to initialize physics body with area, %0.2f; Where max-area allowed is %0.2f\n", Area, MaxBodySize);
        
        // NOTE: Empty body 
        return false;
    }
    
    if(Density < MinDensity)
    {
        fprintf(stderr, "Unable to initialize physics body with density, %0.2f; Where min-density allowed is %0.2f\n", Density, MinDensity);
        
        // NOTE: Empty body 
        return false;
    }
    
    if(Density > MaxDensity)
    {
        fprintf(stderr, "Unable to initialize physics body with density, %0.2f; Where max-density allowed is %0.2f\n", Density, MaxDensity);
        
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
    
    if(!IsPhysicsBodyValid(Area, Density))
    {
        // NOTE: Return empty body
        return Body;
    }
    
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
    
    if(!IsPhysicsBodyValid(Area, Density))
    {
        // NOTE: Return empty body
        return Body;
    }
    
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
    
    return Body;
}

inline void
MovePhysicsBodyByLinearVelocity(physics_body2D* Body)
{
    Body->Position = Body->Position + Body->LinearVelocity;
}

inline bool32
IntersectCircles(physics_body2D* A, physics_body2D* B, vec2* OutNormal, real32* OutDepth)
{
    Assert(A->Shape == CIRCLE);
    Assert(B->Shape == CIRCLE);
    
    *OutNormal = vec(0.0f, 0.0f);
    *OutDepth = 0.0f;
    
    real32 distance = Distance(A->Position, B->Position);
    real32 Radii = A->Radius + B->Radius;
    
    if(distance >= Radii)
    {
        return false;
    }
    
    *OutNormal = Normalize(B->Position - A->Position);
    *OutDepth = Radii - distance;
    
    return true;
}

inline bool32
CheckCollision2D(physics_body2D* A, physics_body2D* B, vec2* OutNormal, real32* OutDepth)
{
    bool32 Result = false;
    
    if(A->Shape == CIRCLE && B->Shape == CIRCLE)
    {
        Result = IntersectCircles(A, B, OutNormal, OutDepth);
    }
    else if(A->Shape == BOX && B->Shape == BOX)
    {
        // Result = IntersectBoxes(A, B, OutNormal, OutDepth);
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
    for(int i = 0; i < BodyCount - 1; i++)
    {
        physics_body2D* BodyA = &Bodies[i];
        
        for(int j = i + 1; j < BodyCount; j++)
        {
            physics_body2D* BodyB = &Bodies[j];
            
            if(CheckCollision2D(BodyA, BodyB, &OutNormal, &OutDepth))
            {
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
            }
        }
    }
}

#endif //PHYSICS2_D_H
