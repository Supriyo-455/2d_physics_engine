/* date = March 22nd 2026 8:57 pm */

#ifndef PHYSICS2_D_H
#define PHYSICS2_D_H

#include "simple_math.h"

#define PHYSICS_BODY_COUNT 20

enum
shape
{
    CIRCLE = 0,
    BOX = 1
};

struct
physics_body2D
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
};

struct
physics_world2D
{
    physics_body2D* Bodies;
    int BodyCount;
    
    // NOTE: Gravity unit - meter/sec^2
    inline local_persist const vec2 Gravity = vec(0.0f, 9.81f);
    
    // NOTE: Size unit - meter^2
    inline local_persist const real32 MinBodySize = 0.01f * 0.01f;
    inline local_persist const real32 MaxBodySize = 64.0f * 64.0f;
    
    // NOTE: Density unit - gm/cm^3
    inline local_persist const real32 MinDensity = 0.5f;
    inline local_persist const real32 MaxDensity = 21.4f;
};

bool32
IsPhysicsBodyValid(physics_world2D* World, physics_body2D* Body)
{    
    if(Body->Area < World->MinBodySize)
    {
        LOG_ERROR("Unable to initialize physics body with area, %0.2f; Where min-area allowed is %0.2f\n", Body->Area, World->MinBodySize);
        
        // NOTE: Empty body 
        return false;
    }
    
    if(Body->Area > World->MaxBodySize)
    {
        LOG_ERROR("Unable to initialize physics body with area, %0.2f; Where max-area allowed is %0.2f\n", Body->Area, World->MaxBodySize);
        
        // NOTE: Empty body 
        return false;
    }
    
    if(Body->Density < World->MinDensity)
    {
        LOG_ERROR("Unable to initialize physics body with density, %0.2f; Where min-density allowed is %0.2f\n", Body->Density, World->MinDensity);
        
        // NOTE: Empty body 
        return false;
    }
    
    if(Body->Density > World->MaxDensity)
    {
        LOG_ERROR("Unable to initialize physics body with density, %0.2f; Where max-density allowed is %0.2f\n", Body->Density, World->MaxDensity);
        
        // NOTE: Empty body 
        return false;
    }
    
    return true;
}

physics_body2D 
CreateCirclePhysicsBody2D(physics_world2D* World, vec2 Position, real32 Radius, real32 Density, real32 Restitution = 0.0f, bool32 IsStatic = false)
{   
    physics_body2D Body = {0};
    real32 Area = PI * Radius * Radius;
    real32 Mass = Density * Area;
    
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
    
    Assert(IsPhysicsBodyValid(World, &Body));
    
    return Body;
}

physics_body2D 
CreateBoxPhysicsBody2D(physics_world2D* World, vec2 Position, real32 Width, real32 Height, real32 Density, real32 Restitution = 0.0f, bool32 IsStatic = false)
{   
    physics_body2D Body = {0};
    real32 Area = Width * Height;
    real32 Mass = Density * Area;
    
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
    
    Assert(IsPhysicsBodyValid(World, &Body));
    
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

bool32
CheckCollisionBounds(real32 MinA, real32 MaxA, real32 MinB, real32 MaxB)
{
    if(MinA >= MaxB || MinB >= MaxA)
    {
        return false;
    }
    
    return true;
}

void
CalculateAxisDepth(real32 MinA, real32 MaxA, real32 MinB, real32 MaxB, vec2 Axis, vec2* OutNormal, real32* OutDepth)
{
    real32 AxisDepth = MIN(MaxB - MinA, MaxA - MinB);
    if(AxisDepth < *OutDepth)
    {
        *OutDepth = AxisDepth;
        *OutNormal = Axis;
    }
}

void
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


void
ProjectCircle(vec2 Center, real32 Radius, vec2 Axis, real32* Min, real32* Max)
{
    vec2 Direction = Axis;
    vec2 DirectionAndRadius = Direction * Radius;
    
    vec2 P1 = Center - DirectionAndRadius;
    vec2 P2 = Center + DirectionAndRadius;
    
    *Min = Dot(P1, Axis);
    *Max = Dot(P2, Axis);
    
    if(*Min > *Max)
    {
        SWAP(real32, *Min, *Max);
    }
}

int
FindClosestPointOnPolygon(vec2 CircleCenter, vec2* PolygonVertices, int PolygonVerticesCount)
{
    int Result = -1;
    real32 MinDistance = FLT_MAX;
    
    for(int i=0; i<PolygonVerticesCount; i++)
    {
        vec2 V = PolygonVertices[i];
        real32 D = Distance(V, CircleCenter);
        
        if(D < MinDistance)
        {
            MinDistance = D;
            Result = i;
        }
    }
    
    return Result;
}


bool32
IntersectCircles(vec2 CenterA, real32 RadiusA, 
                 vec2 CenterB, real32 RadiusB, 
                 vec2* OutNormal, real32* OutDepth)
{
    *OutNormal = vec(0.0f, 0.0f);
    *OutDepth = 0.0f;
    
    real32 distance = Distance(CenterA, CenterB);
    real32 Radii = RadiusA + RadiusB;
    
    if(distance >= Radii)
    {
        return false;
    }
    
    *OutNormal = Normalize(CenterB - CenterA);
    *OutDepth = Radii - distance;
    
    return true;
}

bool32
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
        
        if(!CheckCollisionBounds(MinA, MaxA, MinB, MaxB))
            return false;
        
        CalculateAxisDepth(MinA, MaxA, MinB, MaxB, Axis, OutNormal, OutDepth);
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
        
        if(!CheckCollisionBounds(MinA, MaxA, MinB, MaxB))
            return false;
        
        CalculateAxisDepth(MinA, MaxA, MinB, MaxB, Axis, OutNormal, OutDepth);
    }
    
    vec2 Direction = BodyB->Position - BodyA->Position;
    if(Dot(Direction, *OutNormal) < 0.0f)
    {
        *OutNormal *= -1.0f;
    }
    
    return true;
}

bool32
IntersectCircleAndPolygon(physics_body2D* Circle, physics_body2D* Polygon, 
                          vec2* OutNormal, real32* OutDepth)
{
    Assert(Circle->Shape == CIRCLE);
    Assert(Polygon->Shape != CIRCLE);
    
    *OutNormal = vec(0.0f, 0.0f);
    *OutDepth = FLT_MAX;
    
    
    for(int i=0; i<ARRAY_COUNT(Polygon->Vertices); i++)
    {
        int Next = (i + 1) % ARRAY_COUNT(Polygon->Vertices);
        vec2 Va = Polygon->Vertices[i];
        vec2 Vb = Polygon->Vertices[Next];
        
        vec2 Edge = Vb - Va;
        // NOTE: Axis is the normal vector of Edge
        vec2 Axis = vec(-Edge.y, Edge.x);
        Axis = Normalize(Axis);
        
        real32 MinA, MaxA, MinB, MaxB;
        
        ProjectVertices(Polygon->Vertices, ARRAY_COUNT(Polygon->Vertices), Axis, &MinA, &MaxA);
        ProjectCircle(Circle->Position, Circle->Radius, Axis, &MinB, &MaxB);
        
        
        if(!CheckCollisionBounds(MinA, MaxA, MinB, MaxB))
            return false;
        
        CalculateAxisDepth(MinA, MaxA, MinB, MaxB, Axis, OutNormal, OutDepth);
    }
    
    {
        int ClosestPointIndex = FindClosestPointOnPolygon(Circle->Position, 
                                                          Polygon->Vertices, ARRAY_COUNT(Polygon->Vertices));
        vec2 ClosestPoint = Polygon->Vertices[ClosestPointIndex];
        vec2 Axis = ClosestPoint - Circle->Position;
        
        real32 MinA, MaxA, MinB, MaxB;
        
        ProjectVertices(Polygon->Vertices, ARRAY_COUNT(Polygon->Vertices), Axis, &MinA, &MaxA);
        ProjectCircle(Circle->Position, Circle->Radius, Axis, &MinB, &MaxB);
        
        if(!CheckCollisionBounds(MinA, MaxA, MinB, MaxB))
            return false;
        
        CalculateAxisDepth(MinA, MaxA, MinB, MaxB, Axis, OutNormal, OutDepth);
    }
    
    vec2 Direction = Polygon->Position - Circle->Position;
    if(Dot(Direction, *OutNormal) < 0.0f)
    {
        *OutNormal *= -1.0f;
    }
    
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
        // NOTE: Always pass the circle body in the first parameter
        
        if(A->Shape == CIRCLE)
        {
            Result = IntersectCircleAndPolygon(A, B,
                                               OutNormal, OutDepth);
        }else
        {
            Result = IntersectCircleAndPolygon(B, A,
                                               OutNormal, OutDepth);
            *OutNormal = -(*OutNormal);
        }
    }
    
    return Result;
}

void 
Transform2DPhysicsBody(physics_body2D* Body)
{
    RotatePhysicsBody(Body);
    MovePhysicsBody(Body);
    
    if(Body->Shape == BOX)
    {
        vec2* TransformedVertices = GetPhysicsBodyTransformedVertices(Body);
        for(int j = 0; j < ARRAY_COUNT(Body->Vertices); j++)
        {
            Body->Vertices[j] = TransformedVertices[j];
        }
    }
}

void
Collide2DPhysicsBodies(physics_body2D* Bodies, int BodyCount)
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
                BodyA->LinearVelocity = BodyA->LinearVelocity - OutNormal * (OutDepth / 2.0f);
                BodyB->LinearVelocity = BodyB->LinearVelocity + OutNormal * (OutDepth / 2.0f);
                
                vec2 RelativeVelocity = BodyB->LinearVelocity - BodyA->LinearVelocity;
                if(Dot(RelativeVelocity, OutNormal) > 0.0f)
                {
                    return;
                }
                
                real32 E = MIN(BodyA->Restitution, BodyB->Restitution);
                real32 J = -(1.0f + E) * Dot(RelativeVelocity, OutNormal);
                
                if(BodyA->Mass + BodyB->Mass > 0)
                {
                    J *= (BodyA->Mass * BodyB->Mass) / (BodyA->Mass + BodyB->Mass);
                }
                vec2 Impulse = J * OutNormal;
                
                BodyA->LinearVelocity = BodyA->LinearVelocity - Impulse * (1.0f / BodyA->Mass);
                BodyB->LinearVelocity = BodyB->LinearVelocity + Impulse * (1.0f / BodyB->Mass);
                
                Transform2DPhysicsBody(BodyA);
                Transform2DPhysicsBody(BodyB);
            }
        }
    }
}

#endif //PHYSICS2_D_H
