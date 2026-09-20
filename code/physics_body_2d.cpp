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
    physics_body2D Body = {};
    real32 Area = PI * Radius * Radius;
    real32 Mass = Density * Area;
    
    Body.AngularVelocity = 0.0f;
    Body.LinearVelocity = vec(0.0f, 0.0f);
    Body.Position = Position;
    Body.Angle = 0.0f;
    Body.Mass = Mass;
    Body.Density = Density;
    Body.Restitution = Clamp(Restitution, 0.0f, 1.0f);
    Body.Area = Area;
    Body.IsStatic = IsStatic;
    Body.Shape = CIRCLE;
    Body.Radius = Radius;
    Body.Force = vec(0.0f, 0.0f);
	Body.Inertia = (1.0f / 2.0f) * Body.Mass * Body.Radius * Body.Radius;
    
    if(!Body.IsStatic)
    {
        Body.InvMass = 1.0f / Body.Mass;
		Body.InvIntertia = 1.0f / Body.Inertia;
    }
    else
    {
        Body.InvMass = 0.0f;
		Body.InvIntertia = 0.0f;
    }
    
    Assert(IsPhysicsBodyValid(World, &Body));
    
    return Body;
}

physics_body2D 
CreateBoxPhysicsBody2D(physics_world2D* World, vec2 Position, real32 Width, real32 Height, real32 Density, real32 Restitution = 0.0f, bool32 IsStatic = false)
{   
    physics_body2D Body = {};
    real32 Area = Width * Height;
    real32 Mass = Density * Area;
    
    Body.AngularVelocity = 0.0f;
    Body.LinearVelocity = vec(0.0f, 0.0f);
    Body.Position = Position;
    Body.Angle = 0.0f;
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
    
    ZeroSize(sizeof(Body.TransformedVertices), Body.TransformedVertices);
	Body.TransformUpdateRequired = true;
    
    Body.Triangles[0] = 0;
    Body.Triangles[1] = 1;
    Body.Triangles[2] = 2;
    Body.Triangles[3] = 2;
    Body.Triangles[4] = 3;
    Body.Triangles[5] = 0;
    
	Body.Inertia = (1.0f / 12.0f) * Body.Mass 
		* (Body.Width * Body.Width + Body.Height * Body.Height);
	
    if(!Body.IsStatic)
    {
        Body.InvMass = 1.0f / Body.Mass;
		Body.InvIntertia = 1.0f / Body.InvIntertia;
    }
    else
    {
        Body.InvMass = 0.0f;
		Body.InvIntertia = 0;
    }
    
	Body.Aabb = {};
	Body.AabbUpdateRequired = false;
	
    Assert(IsPhysicsBodyValid(World, &Body));
    
    return Body;
}

vec2 FindPolygonCenter(vec2* Vertices, int VerticesCount)
{
    real32 SumX = 0.0f;
    real32 SumY = 0.0f;
    
    for(int i = 0; 
		i < VerticesCount; 
		i++)
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
        transform2D SavedTransform = CreateTransform2D(Body->Position, Body->Angle);
        
        for(int i = 0; 
			i < ARRAY_COUNT(Body->Vertices); 
			i++)
        {
            Body->TransformedVertices[i] = Transform(Body->Vertices[i], SavedTransform);
        }
        Body->TransformUpdateRequired = false;
    }
    
    return Body->TransformedVertices;
}

inline AABB
GetAABBFromPhysicsBody(physics_body2D* Body)
{
	if(Body->AabbUpdateRequired)
	{
		real32 MinX = FLT_MAX;
		real32 MaxX = FLT_MIN;
		real32 MinY = FLT_MAX;
		real32 MaxY = FLT_MIN;
		
		if(Body->Shape == CIRCLE)
		{
			MinX = Body->Position.x - Body->Radius;
			MinY = Body->Position.y - Body->Radius;
			
			MaxX = Body->Position.x + Body->Radius;
			MaxY = Body->Position.y + Body->Radius;
		}
		else if(Body->Shape == BOX)
		{
			vec2* PolygonVerts = GetPhysicsBodyTransformedVertices(Body);
			int PolygonVertsCount = ARRAY_COUNT(Body->Vertices);
			
			for(int i = 0; 
				i < PolygonVertsCount; 
				i++)
			{
				vec2 V = PolygonVerts[i];
				
				if(V.x < MinX) MinX = V.x;
				if(V.x > MaxX) MaxX = V.x;
				
				if(V.y < MinY) MinY = V.y;
				if(V.y > MaxY) MaxY = V.y;
			}
		}
		else
		{
			LOG_ERROR("unknown shaped physics body!");
		}
		
		Body->Aabb.Min = vec(MinX, MinY);
		Body->Aabb.Max = vec(MaxX, MaxY);
	}
	
	return Body->Aabb;
}

void
ProjectVertices(vec2* Vertices, int VerticesCount, vec2 Axis, real32* Min, real32* Max)
{
    *Min = FLT_MAX;
    *Max = FLT_MIN;
    
    for(int i = 0; 
		i < VerticesCount; 
		i++)
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
FindClosestVertexOnPolygon(vec2 CircleCenter, vec2* PolygonVertices, int PolygonVerticesCount)
{
    int Result = -1;
    real32 MinDistance = FLT_MAX;
    
    for(int i = 0; 
		i < PolygonVerticesCount; 
		i++)
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

inline void
MovePhysicsBody(physics_body2D* Body, real32 ElapsedTime)
{
    Body->Position = Body->Position + Body->LinearVelocity * ElapsedTime;
    Body->TransformUpdateRequired = true;
	Body->AabbUpdateRequired = true;
}

inline void
RotatePhysicsBody(physics_body2D* Body, real32 ElapsedTime)
{
    Body->Angle = Body->Angle + Body->AngularVelocity * ElapsedTime;
    Body->TransformUpdateRequired = true;
	Body->AabbUpdateRequired = true;
}

inline void
ApplyForce(physics_body2D* Body, real32 ElapsedTime)
{
    if (Body->Mass > 0.0f)
    {
        vec2 Acceleration = Body->Force * Body->InvMass;
        Body->LinearVelocity = Body->LinearVelocity + Acceleration * ElapsedTime;
        Body->TransformUpdateRequired = true;
		Body->AabbUpdateRequired = true;
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
		Body->AabbUpdateRequired = true;
    }
}
