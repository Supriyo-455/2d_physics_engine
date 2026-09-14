collision_manifold
CreateCollisionManifold(physics_body2D* A, 
						physics_body2D* B, 
						vec2 OutNormal, 
						real32 OutDepth, 
						vec2 Contact1, 
						vec2 Contact2, 
						uint32 ContactCount)
{
	collision_manifold CollisionManifold = {};
	
	CollisionManifold.BodyA = A;
	CollisionManifold.BodyB = B;
	CollisionManifold.OutNormal = OutNormal;
	CollisionManifold.OutDepth = OutDepth;
	CollisionManifold.Contact1 = Contact1;
	CollisionManifold.Contact2 = Contact2;
	CollisionManifold.ContactCount = ContactCount;
	
	return CollisionManifold;
}

inline AABB
GetAABBFromPhysicsBody(physics_body2D* Body)
{
	real32 MinX = FLT_MAX;
	real32 MaxX = FLT_MIN;
	real32 MinY = FLT_MAX;
	real32 MaxY = FLT_MIN;
	
	AABB Result = {};
	
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
		
		for(int i=0; i<PolygonVertsCount; i++)
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
	
	Result.Min = vec(MinX, MinY);
	Result.Max = vec(MaxX, MaxY);
    
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

bool32
IntersectAABBs(AABB BodyA_AABB, AABB BodyB_AABB)
{
	bool32 Collided = true;
	
	if(BodyB_AABB.Max.x <= BodyA_AABB.Min.x || 
	   BodyA_AABB.Max.x <= BodyB_AABB.Min.x ||
	   BodyB_AABB.Max.y <= BodyA_AABB.Min.y ||
	   BodyA_AABB.Max.y <= BodyB_AABB.Min.y)
	{
		Collided = false;
	}
	
	return Collided;
}

bool32
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
ResolveCollision(collision_manifold* CollisionManifold)
{
	physics_body2D* A = CollisionManifold->BodyA;
	physics_body2D* B = CollisionManifold->BodyB;
	vec2 OutNormal = CollisionManifold->OutNormal;
	
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
	
	A->TransformUpdateRequired = true;
	B->TransformUpdateRequired = true;
}


void
UpdatePhysicsWorld2d(physics_world2D* World, real32 ElapsedTime, int Iterations)
{
	Iterations = Clamp(Iterations, World->MinIteration, World->MaxIteration);
	
	World->ContactPoints.clear();
	
	real32 SubStepTime = ElapsedTime / Iterations;
	
	for(int it=1; it<Iterations; it++)
	{
		// NOTE: Movement step
		for(int i=0; i<World->Bodies.size(); i++)
		{
			ApplyGravity(&World->Bodies[i],
						 World->Gravity, 
						 World->ForceMultiplier, 
						 ElapsedTime);
			
			ApplyForce(&World->Bodies[i], SubStepTime);
			RotatePhysicsBody(&World->Bodies[i], SubStepTime);
			MovePhysicsBody(&World->Bodies[i], SubStepTime);
			
			// NOTE: Reset the force to zero
			World->Bodies[i].Force = vec(0.0f, 0.0f);
		}
		
		// NOTE: Reset collision state
		for(int i=0; i<World->Bodies.size(); i++)
		{
			World->Bodies[i].IsCollided = false;
		}
		
		World->CollisionManifolds.clear();
		
		// NOTE: Collide step
		for(int i=0; i<World->Bodies.size()-1; i++)
		{
			physics_body2D* BodyA = &World->Bodies[i];
			AABB BodyA_AABB = GetAABBFromPhysicsBody(BodyA);
			
			for(int j=i+1; j<World->Bodies.size(); j++)
			{
				physics_body2D* BodyB = &World->Bodies[j];
				AABB BodyB_AABB = GetAABBFromPhysicsBody(BodyB);
				
				if(!IntersectAABBs(BodyA_AABB, BodyB_AABB))
				{
					continue;
				}
				
				if(BodyA->IsStatic && BodyB->IsStatic)
				{
					continue;
				}
				
				vec2 OutNormal = vec(0.0f, 0.0f);
				real32 OutDepth = 0.0f;
				
				vec2 Contact1 = vec(0.0f, 0.0f);
				vec2 Contact2 = vec(0.0f, 0.0f);
				uint32 ContactPoints;
				
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
						BodyA->Position = BodyA->Position - (OutNormal * (OutDepth * 0.5f));
						BodyB->Position = BodyB->Position + (OutNormal * (OutDepth * 0.5f));
					}
					
					FindContactPoints(BodyA, BodyB, &Contact1, &Contact2, &ContactPoints);
					
					collision_manifold CollisionManifold = 
						CreateCollisionManifold(BodyA, 
												BodyB, 
												OutNormal, 
												OutDepth, 
												Contact1, 
												Contact2,
												ContactPoints);
					
					World->CollisionManifolds.push_back(CollisionManifold);
				}
			}
			
			for(int i=0; i<World->CollisionManifolds.size(); i++)
			{
				collision_manifold CollisionManifold = World->CollisionManifolds.data()[i];
				ResolveCollision(&CollisionManifold);
				
				if(CollisionManifold.ContactCount > 0)
				{
					World->ContactPoints.push_back(CollisionManifold.Contact1);
					
					if(CollisionManifold.ContactCount > 1)
						World->ContactPoints.push_back(CollisionManifold.Contact2);
				}
			}
		}
	}
}
