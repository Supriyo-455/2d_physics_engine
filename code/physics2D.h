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
transform2D
{
    real32 PosX;
    real32 PosY;
    real32 Sin;
    real32 Cos;
};

struct
physics_body2D
{
    vec2 LinearVelocity;
    vec2 Position;
    
    real32 Rotation;
    real32 RotationalVelocity;
    
    real32 Mass;
    real32 InvMass;
    real32 Density;
    real32 Restitution;
    real32 Area;
    
    vec2 Force;
    
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

transform2D
CreateTransform2D(vec2 Pos, real32 Angle)
{
    transform2D Result = {};
    
    Result.Cos = cosf(Angle);
    Result.Sin = sinf(Angle);
    Result.PosX = Pos.x;
    Result.PosY = Pos.y;
    
    return Result;
}

transform2D
CreateTransform2D(real32 PosX, real32 PosY, real32 Angle)
{
    transform2D Result = {};
    
    Result.Cos = cosf(Angle);
    Result.Sin = sinf(Angle);
    Result.PosX = PosX;
    Result.PosY = PosY;
    
    return Result;
}

vec2
Transform(vec2 V, transform2D Transform)
{
    vec2 Result = {};
    
    Result.x = Transform.Cos * V.x - Transform.Sin * V.y + Transform.PosX;
    Result.y = Transform.Sin * V.x + Transform.Cos * V.y + Transform.PosY;
    
    return Result;
}

vec2
Transform(vec2 V, vec2 Pos, real32 Angle)
{
    transform2D T = CreateTransform2D(Pos, Angle);
    return Transform(V, T);
}

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
    Body.Force = vec(0.0f, 0.0f);
    
    if(!Body.IsStatic)
    {
        Body.InvMass = 1.0f / Body.Mass;
    }
    else
    {
        Body.InvMass = 0.0f;
    }
    
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
    Body.Force = vec(0.0f, 0.0f);
    
    Body.IsCollided = false;
    
    real32 HalfWidth = Width / 2.0f;
    real32 HalfHeight = Height / 2.0f;
    
    // NOTE: Relative to (0,0)
    Body.Vertices[0] = vec(-HalfWidth, -HalfHeight); // Top-Left
    Body.Vertices[1] = vec(-HalfWidth,  HalfHeight); // Bottom-Left
    Body.Vertices[2] = vec( HalfWidth,  HalfHeight); // Bottom-Right
    Body.Vertices[3] = vec( HalfWidth, -HalfHeight); // Top-Right
    
    memset(Body.TransformedVertices, 0, sizeof(Body.TransformedVertices));
    Body.TransformUpdateRequired = true;
    
    Body.Triangles[0] = 0;
    Body.Triangles[1] = 1;
    Body.Triangles[2] = 2;
    Body.Triangles[3] = 2;
    Body.Triangles[4] = 3;
    Body.Triangles[5] = 0;
    
    if(!Body.IsStatic)
    {
        Body.InvMass = 1.0f / Body.Mass;
    }
    else
    {
        Body.InvMass = 0.0f;
    }
    
    Assert(IsPhysicsBodyValid(World, &Body));
    
    return Body;
}

vec2 FindPolygonCenter(vec2* Vertices, int VerticesCount)
{
    real32 SumX = 0.0f;
    real32 SumY = 0.0f;
    
    for(int i=0; i<VerticesCount; i++)
    {
        vec2 V = Vertices[i];
        SumX += V.x;
        SumY += V.y;
    }
    
    return vec(SumX / (real32)VerticesCount, SumY / (real32)VerticesCount);
}

inline void
MovePhysicsBody(physics_body2D* Body, real32 ElapsedTime)
{
    Body->Position = Body->Position + Body->LinearVelocity * ElapsedTime;
    Body->TransformUpdateRequired = true;
}

inline void
RotatePhysicsBody(physics_body2D* Body, real32 ElapsedTime)
{
    Body->Rotation = Body->Rotation + Body->RotationalVelocity * ElapsedTime;
    Body->TransformUpdateRequired = true;
}

inline void
ApplyForce(physics_body2D* Body, real32 ElapsedTime)
{
    if (Body->Mass > 0.0f)
    {
        vec2 Acceleration = Body->Force * Body->InvMass;
        Body->LinearVelocity = Body->LinearVelocity + Acceleration * ElapsedTime;
        Body->TransformUpdateRequired = true;
    }
}

vec2*
GetPhysicsBodyTransformedVertices(physics_body2D* Body)
{
    if(Body->TransformUpdateRequired)
    {
        transform2D SavedTransform = CreateTransform2D(Body->Position, Body->Rotation);
        
        for(int i=0; i<ARRAY_COUNT(Body->Vertices); i++)
        {
            Body->TransformedVertices[i] = Transform(Body->Vertices[i], SavedTransform);
        }
        Body->TransformUpdateRequired = false;
    }
    
    return Body->TransformedVertices;
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
    
    vec2 P1 = Center + DirectionAndRadius;
    vec2 P2 = Center - DirectionAndRadius;
    
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
    
    vec2* VertsA = GetPhysicsBodyTransformedVertices(BodyA);
    vec2* VertsB = GetPhysicsBodyTransformedVertices(BodyB);
    
    int VertsCountA = ARRAY_COUNT(BodyA->Vertices);
    int VertsCountB = ARRAY_COUNT(BodyB->Vertices);
    
    for(int i=0; i<VertsCountA; i++)
    {
        int Next = (i + 1) % VertsCountA;
        vec2 Va = VertsA[i];
        vec2 Vb = VertsA[Next];
        
        vec2 Edge = Vb - Va;
        // NOTE: Axis is the normal vector of Edge
        vec2 Axis = vec(0.0f, 0.0f);
        real32 AxisDepth = 0.0f;
        real32 MinA, MaxA, MinB, MaxB;
        
        Axis = vec(-Edge.y, Edge.x);
        Axis = Normalize(Axis);
        
        ProjectVertices(VertsA, VertsCountA, Axis, &MinA, &MaxA);
        ProjectVertices(VertsB, VertsCountB, Axis, &MinB, &MaxB);
        
        if(MinA >= MaxB || MinB >= MaxA)
            return false;
        
        AxisDepth = MIN(MaxB - MinA, MaxA - MinB);
        if(AxisDepth < *OutDepth)
        {
            *OutDepth = AxisDepth;
            *OutNormal = Axis;
        }
    }
    
    for(int i=0; i<VertsCountB; i++)
    {
        int Next = (i + 1) % VertsCountB;
        vec2 Va = VertsB[i];
        vec2 Vb = VertsB[Next];
        
        vec2 Edge = Vb - Va;
        
        // NOTE: Axis is the normal vector of Edge
        vec2 Axis = vec(0.0f, 0.0f);
        real32 AxisDepth = 0.0f;
        real32 MinA, MaxA, MinB, MaxB;
        
        Axis = vec(-Edge.y, Edge.x);
        Axis = Normalize(Axis);
        
        ProjectVertices(VertsB, VertsCountB, Axis, &MinA, &MaxA);
        ProjectVertices(VertsA, VertsCountA, Axis, &MinB, &MaxB);
        
        
        if(MinA >= MaxB || MinB >= MaxA)
            return false;
        
        AxisDepth = MIN(MaxB - MinA, MaxA - MinB);
        if(AxisDepth < *OutDepth)
        {
            *OutDepth = AxisDepth;
            *OutNormal = Axis;
        }
    }
    
    *OutDepth /= Magnitude(*OutNormal);
    *OutNormal = Normalize(*OutNormal);
    
    vec2 Direction = BodyB->Position - BodyA->Position;
    if(Dot(Direction, *OutNormal) < 0.0f)
    {
        *OutNormal = -(*OutNormal);
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
    
    vec2* PolygonVerts = GetPhysicsBodyTransformedVertices(Polygon);
    int PolygonVertsCount = ARRAY_COUNT(Polygon->Vertices);
    
    for(int i=0; i<PolygonVertsCount; i++)
    {
        int Next = (i + 1) % PolygonVertsCount;
        vec2 Va = PolygonVerts[i];
        vec2 Vb = PolygonVerts[Next];
        
        vec2 Edge = Vb - Va;
        
        // NOTE: Axis is the normal vector of Edge
        vec2 Axis = vec(0.0f, 0.0f);
        real32 AxisDepth = 0.0f;
        real32 MinA, MaxA, MinB, MaxB;
        
        Axis = vec(-Edge.y, Edge.x);
        Axis = Normalize(Axis);
        
        ProjectVertices(PolygonVerts, PolygonVertsCount, Axis, &MinA, &MaxA);
        ProjectCircle(Circle->Position, Circle->Radius, Axis, &MinB, &MaxB);
        
        if(MinA >= MaxB || MinB >= MaxA)
            return false;
        
        AxisDepth = MIN(MaxB - MinA, MaxA - MinB);
        if(AxisDepth < *OutDepth)
        {
            *OutDepth = AxisDepth;
            *OutNormal = Axis;
        }
    }
    
    {
        int ClosestPointIndex = FindClosestPointOnPolygon(Circle->Position, 
                                                          PolygonVerts,
                                                          PolygonVertsCount);
        vec2 ClosestPoint = PolygonVerts[ClosestPointIndex];
        
        vec2 Axis = vec(0.0f, 0.0f);
        real32 AxisDepth = 0.0f;
        real32 MinA, MaxA, MinB, MaxB;
        
        Axis = ClosestPoint - Circle->Position;
        Axis = Normalize(Axis);
        
        ProjectVertices(PolygonVerts, PolygonVertsCount, Axis, &MinA, &MaxA);
        ProjectCircle(Circle->Position, Circle->Radius, Axis, &MinB, &MaxB);
        
        if(MinA >= MaxB || MinB >= MaxA)
            return false;
        
        AxisDepth = MIN(MaxB - MinA, MaxA - MinB);
        if(AxisDepth < *OutDepth)
        {
            *OutDepth = AxisDepth;
            *OutNormal = Axis;
        }
    }
    
    *OutDepth /= Magnitude(*OutNormal);
    *OutNormal = Normalize(*OutNormal);
    
    vec2 Direction = Polygon->Position - Circle->Position;
    if(Dot(Direction, *OutNormal) < 0.0f)
    {
        *OutNormal = -(*OutNormal);
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
        }
        else
        {
            Result = IntersectCircleAndPolygon(B, A,
                                               OutNormal, OutDepth);
            *OutNormal = -(*OutNormal);
        }
    }
    
    return Result;
}

void
ResolveCollision(physics_body2D* A, physics_body2D* B, vec2 OutNormal)
{
    vec2 RelativeVelocity = B->LinearVelocity - A->LinearVelocity;
    real32 e = MIN(A->Restitution, B->Restitution);
    
    // NOTE: If objects are already separating, do not apply impulse
    real32 VelAlongNormal = Dot(RelativeVelocity, OutNormal);
    if (VelAlongNormal > 0.0f)
        return;
    
    real32 j = -(1.0f + e) * VelAlongNormal;
    j /= A->InvMass + B->InvMass;
    
    // Multiply OutNormal by scalar impulse/mass instead of dividing by a vector
    vec2 Impulse = OutNormal * j;
    A->LinearVelocity = A->LinearVelocity - Impulse * A->InvMass;
    B->LinearVelocity = B->LinearVelocity + Impulse * B->InvMass;
}

void
UpdatePhysicsWorld2d(physics_world2D* World, real32 ElapsedTime)
{
    // NOTE: Movement step
    for(int i=0; i<World->BodyCount; i++)
    {
        ApplyForce(&World->Bodies[i], ElapsedTime);
        RotatePhysicsBody(&World->Bodies[i], ElapsedTime);
        MovePhysicsBody(&World->Bodies[i], ElapsedTime);
        
        // NOTE: Reset the force to zero
        World->Bodies[i].Force = vec(0.0f, 0.0f);
    }
    
    // NOTE: Reset collision state
    for(int i=0; i<World->BodyCount; i++)
    {
        World->Bodies[i].IsCollided = false;
    }
    
    // NOTE: Collide step
    for(int i=0; i<World->BodyCount-1; i++)
    {
        physics_body2D* BodyA = &World->Bodies[i];
        
        for(int j=i+1; j<World->BodyCount; j++)
        {
            physics_body2D* BodyB = &World->Bodies[j];
            
            if(BodyA->IsStatic && BodyB->IsStatic)
            {
                continue;
            }
            
            vec2 OutNormal = vec(0.0f, 0.0f);
            real32 OutDepth = 0.0f;
            if(CheckCollision2D(BodyA, BodyB, &OutNormal, &OutDepth))
            {
                BodyA->IsCollided = true;
                BodyB->IsCollided = true;
                
                // NOTE: Compress this code
                if(BodyA->IsStatic)
                {
                    BodyB->Position = BodyB->Position + OutNormal * (OutDepth);
                }
                else if(BodyB->IsStatic)
                {
                    BodyA->Position = BodyA->Position - OutNormal * (OutDepth);
                }
                else
                {
                    BodyA->Position = BodyA->Position - (OutNormal * (OutDepth / 2.0f));
                    BodyB->Position = BodyB->Position + OutNormal * (OutDepth / 2.0f);
                }
                
                ResolveCollision(BodyA, BodyB, OutNormal);
                
                BodyA->TransformUpdateRequired = true;
                BodyB->TransformUpdateRequired = true;
            }
        }
    }
}

#endif //PHYSICS2_D_H
