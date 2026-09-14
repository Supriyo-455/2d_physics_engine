/* date = September 14th 2026 2:20 pm */

#ifndef PHYSICS_BODY_H
#define PHYSICS_BODY_H

enum
shape
{
    CIRCLE = 0,
    BOX = 1
};

// TODO: Cache AABB information?
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

#endif //PHYSICS_BODY_H
