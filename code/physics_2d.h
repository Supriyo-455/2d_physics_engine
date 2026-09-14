/* date = March 22nd 2026 8:57 pm */

#ifndef PHYSICS2_D_H
#define PHYSICS2_D_H

#include "simple_math.h"
#include <vector>

#include "transform_2d.h"
#include "physics_body_2d.h"
#include "collision_2d.h"

struct
physics_world2D
{
	std::vector<physics_body2D> Bodies;
	std::vector<collision_manifold> CollisionManifolds;
	std::vector<vec2> ContactPoints;
	
	inline local_persist const real32 ForceMultiplier = 1000.0f;
    
	inline local_persist const int MaxIteration = 128;
	inline local_persist const int MinIteration = 1;
	
    // NOTE: Gravity unit - meter/sec^2
    inline local_persist const vec2 Gravity = vec(0.0f, 9.81f);
    
    // NOTE: Size unit - meter^2
    inline local_persist const real32 MinBodySize = 0.01f * 0.01f;
    inline local_persist const real32 MaxBodySize = 1000.0f * 1000.0f;
    
    // NOTE: Density unit - gm/cm^3
    inline local_persist const real32 MinDensity = 0.5f;
    inline local_persist const real32 MaxDensity = 21.4f;
};

// NOTE: Unity Build dont re-arrange these files
#include "transform_2d.cpp"
#include "physics_body_2d.cpp"
#include "collision_2d.cpp"


#endif //PHYSICS2_D_H
