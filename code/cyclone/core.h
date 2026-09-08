/* date = August 29th 2026 5:43 pm */
#ifndef CORE_H
#define CORE_H

#include "precision.h"

struct vec3
{
    real x;
    real y;
    real z;
    real pad;
};

inline vec3
Vec3(real x, real y, real z)
{
    vec3 V = {};
    V.x = x;
    V.y = y;
    V.z = z;
    return V;
}

inline vec3
Vec3Zero(void)
{
    vec3 V = {};
    return V;
}

inline void
Invert(vec3 *V)
{
    V->x = -V->x;
    V->y = -V->y;
    V->z = -V->z;
}

inline void
Normalize(vec3 *V)
{
    real l = sqrtf(V->x * V->x + V->y * V->y + V->z * V->z);
    if (l > 0)
    {
        real inv = ((real)1) / l;
        V->x *= inv;
        V->y *= inv;
        V->z *= inv;
    }
}

inline void
Scale(vec3 *V, real Value)
{
    V->x *= Value;
    V->y *= Value;
    V->z *= Value;
}

inline void
Add(vec3 *A, vec3 B)
{
    A->x += B.x;
    A->y += B.y;
    A->z += B.z;
}

inline void
Subtract(vec3 *A, vec3 B)
{
    A->x -= B.x;
    A->y -= B.y;
    A->z -= B.z;
}

inline void
AddScaledVectorUpdate(vec3 *A, vec3 B, real Scale)
{
    A->x += B.x * Scale;
    A->y += B.y * Scale;
    A->z += B.z * Scale;
}

inline void
ComponentProductUpdate(vec3 *A, vec3 B)
{
    A->x *= B.x;
    A->y *= B.y;
    A->z *= B.z;
}

inline real
Magnitude(vec3 V)
{
    return real_sqrt(V.x * V.x + V.y * V.y + V.z * V.z);
}

inline real
SquareMagnitude(vec3 V)
{
    return (V.x * V.x + V.y * V.y + V.z * V.z);
}

inline vec3
Scaled(vec3 V, real Value)
{
    vec3 R;
    R.x = V.x * Value;
    R.y = V.y * Value;
    R.z = V.z * Value;
    R.pad = 0;
    return R;
}

inline vec3
AddVec3(vec3 A, vec3 B)
{
    vec3 R;
    R.x = A.x + B.x;
    R.y = A.y + B.y;
    R.z = A.z + B.z;
    R.pad = 0;
    return R;
}

inline vec3
SubtractVec3(vec3 A, vec3 B)
{
    vec3 R;
    R.x = A.x - B.x;
    R.y = A.y - B.y;
    R.z = A.z - B.z;
    R.pad = 0;
    return R;
}

inline vec3
ComponentProduct(vec3 A, vec3 B)
{
    vec3 R;
    R.x = A.x * B.x;
    R.y = A.y * B.y;
    R.z = A.z * B.z;
    R.pad = 0;
    return R;
}

inline real
Dot(vec3 A, vec3 B)
{
    return (A.x * B.x + A.y * B.y + A.z * B.z);
}

inline vec3
Cross(vec3 A, vec3 B)
{
    vec3 R;
    R.x = A.y * B.z - A.z * B.y;
    R.y = A.z * B.x - A.x * B.z;
    R.z = A.x * B.y - A.y * B.x;
    R.pad = 0;
    return R;
}

#endif // CORE_H