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

void
FindContactPointsCircles(vec2 CenterA, 
						 real32 RadiusA, 
						 vec2 CenterB, 
						 real32 RadiusB, 
						 vec2* ContactPoint)
{
	vec2 A2B = CenterB - CenterA;
	vec2 Direction = Normalize(A2B);
	*ContactPoint = CenterA + Direction * RadiusA;
}

void
FindContactPoints(physics_body2D* BodyA, physics_body2D* BodyB, 
				  vec2* Contact1, vec2* Contact2, uint32* ContactPoints)
{
	*Contact1 = vec(0.0f, 0.0f);
	*Contact2 = vec(0.0f, 0.0f);
	*ContactPoints = 0;
	
	if(BodyA->Shape == CIRCLE && BodyB->Shape == CIRCLE)
    {
		FindContactPointsCircles(BodyA->Position, BodyA->Radius, 
								 BodyB->Position, BodyB->Radius, 
								 Contact1);
		*ContactPoints = 1;
	}
    else if(BodyA->Shape == BOX && BodyB->Shape == BOX)
    {}
    else
    {
        // NOTE: Either one of them is box and circle
        // NOTE: Always pass the circle body in the first parameter
        
        if(BodyA->Shape == CIRCLE)
        {}
        else
        {}
    }
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

inline void
ApplyGravity(physics_body2D* Body, vec2 Gravity, real32 ForceMultiplier, real32 ElapsedTime)
{
    if (Body->Mass > 0.0f)
    {
        vec2 Acceleration = Gravity * Body->InvMass * ForceMultiplier;
        Body->LinearVelocity = Body->LinearVelocity + Acceleration * ElapsedTime;
        Body->TransformUpdateRequired = true;
    }
}

