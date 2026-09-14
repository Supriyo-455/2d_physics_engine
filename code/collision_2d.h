/* date = September 14th 2026 8:45 pm */

#ifndef COLLISION_2D_H
#define COLLISION_2D_H


struct
AABB
{
	vec2 Max;
	vec2 Min;
};

struct
collision_manifold
{
	physics_body2D* BodyA;
	physics_body2D* BodyB;
	vec2 OutNormal;
	real32 OutDepth;
	vec2 Contact1;
	vec2 Contact2;
	uint32 ContactCount;
};


#endif //COLLISION_2D_H
