#ifndef SIMPLE_MATH_H
#define SIMPLE_MATH_H

#include "defines.h"

#define PI 3.141592653f

inline real32 absf(real32 x)
{
    if (x < 0)
        x = -x;
    return x;
}

inline uint32 RoundReal32ToUint32(real32 f)
{
    uint32 Result = static_cast<unsigned int>(f + 0.5);
    return Result;
}

inline real32 Sin(real32 x)
{
    real32 sign = 1;
    if (x < 0)
    {
        sign = -1.0;
        x = -x;
    }
    if (x > 360)
        x -= int32(x / 360.0f) * 360.0f;
    x *= PI / 180.0f;
    real32 res = 0;
    real32 term = x;
    int32 k = 1;
    while (res + term != res)
    {
        res += term;
        k += 2;
        term *= -x * x / k / (k - 1);
    }
    
    return sign * res;
}

inline real32 Cos(real32 x)
{
    if (x < 0)
        x = -x;
    if (x > 360)
        x -= int32(x / 360.0f) * 360.0f;
    x *= (real32) PI / 180.0f;
    real32 res = 0;
    real32 term = 1;
    int32 k = 0;
    while (res + term != res)
    {
        res += term;
        k += 2;
        term *= -x * x / k / (k - 1);
    }
    return res;
}

int
Clamp(int Value, int Min, int Max)
{
    if(Min == Max)
        return Min;
    
    if(Min > Max)
    {
        LOG_ERROR("Min Value can't be greater than Max Value!!!, Value provided %d, Min - %d, Max - %d", Value, Min, Max);
        return 0;
    }
    
    if(Value < Min)
        return Min;
    
    if(Value > Max)
        return Max;
    
    return Value;
}

real32
Clamp(real32 Value, real32 Min, real32 Max)
{
    if(Min == Max)
        return Min;
    
    if(Min > Max)
    {
        LOG_ERROR("Min Value can't be greater than Max Value!!!, Value provided %0.2f, Min - %0.2f, Max - %0.2f", Value, Min, Max);
        return 0;
    }
    
    if(Value < Min)
        return Min;
    
    if(Value > Max)
        return Max;
    
    return Value;
}

typedef union vec2
{
    real32 E[2];
    struct
    {
        real32 x, y;
    };
} vec2;

typedef struct
{
    real32 Sine;
    real32 Cosine;
    vec2 Position;
}transformVec2;

// TODO: Need to implement swizziling like glsl
typedef union vec3
{
    real32 E[3];
    struct
    {
        real32 x, y, z;
    };
} vec3;

// TODO: Need to implement swizziling like glsl
typedef union vec4
{
    real32 E[4];
    struct
    {
        real32 x, y, z, w;
    };
    // NOTE: For using it as a color struct
    struct
    {
        real32 r, g, b, a;
    };
} vec4;

inline vec2 vec(real32 x, real32 y)
{
    vec2 V = {};
    V.x = x;
    V.y = y;
    return V;
}

inline vec3 vec(real32 x, real32 y, real32 z)
{
    vec3 V = {};
    V.x = x;
    V.y = y;
    V.z = z;
    return V;
}

inline vec4 vec(real32 x, real32 y, real32 z, real32 w)
{
    vec4 V = {};
    V.x = x;
    V.y = y;
    V.z = z;
    V.w = w;
    return V;
}

inline vec4 vec(vec3 A, real32 w)
{
    vec4 V = {};
    V.x = A.x;
    V.y = A.y;
    V.z = A.z;
    V.w = w;
    return V;
}

inline real32 Dot(vec2 A, vec2 B)
{
    real32 result = 0.0f;
    for (int i = 0; i < 2; i++)
    {
        result += A.E[i] * B.E[i];
    }
    return result;
}

inline real32 Magnitude(vec2 A)
{
    return sqrtf(Dot(A, A));
}

inline vec2 Normalize(vec2 A)
{
    real32 mag = Magnitude(A);
    A.x /= mag;
    A.y /= mag;
    return A;
}

inline real32 Distance(vec2 A, vec2 B)
{
    real32 Result;
    Result = (A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y);
    return (real32) sqrt(Result);
}

inline real32 Angle(vec2 A, vec2 B)
{
    vec2 normA = Normalize(A);
    vec2 normB = Normalize(B);
    real32 cosValue = Dot(normA, normB);
    return acosf(cosValue);
}

inline vec2 Transform(vec2 A, transformVec2 TransformVec2)
{
    vec2 Result = {};
    
    Result.x = TransformVec2.Cosine * A.x - TransformVec2.Sine * A.y + TransformVec2.Position.y;
    Result.x = TransformVec2.Sine * A.x + TransformVec2.Cosine * A.y + TransformVec2.Position.x;
    
    return Result;
}

inline vec2 operator-(vec2 A)
{
    vec2 Result;
    Result.x = -A.x;
    Result.y = -A.y;
    return Result;
}

inline vec2 operator+(vec2 A, vec2 B)
{
    vec2 Result;
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    return Result;
}

inline vec2 operator-(vec2 A, vec2 B)
{
    vec2 Result;
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    return Result;
}

inline vec2 operator*(vec2 B, real32 A)
{
    vec2 Result;
    Result.x = A * B.x;
    Result.y = A * B.y;
    return Result;
}

inline vec2 operator*(real32 A, vec2 B)
{
    vec2 Result;
    Result.x = A * B.x;
    Result.y = A * B.y;
    return Result;
}

inline vec2 &operator*=(vec2 &B, real32 A)
{
    B = A * B;
    return B;
}

inline vec3 operator-(vec3 A)
{
    vec3 Result;
    Result.x = -A.x;
    Result.y = -A.y;
    Result.z = -A.z;
    return Result;
}

inline vec3 operator+(vec3 A, vec3 B)
{
    vec3 Result;
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    return Result;
}

inline vec3 operator-(vec3 A, vec3 B)
{
    vec3 Result;
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    return Result;
}

inline vec3 operator*(vec3 A, real32 B)
{
    vec3 Result;
    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;
    return Result;
}

inline vec3 operator/(vec3 A, real32 B) 
{
    vec3 Result;
    B = 1 / B;
    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;
    return Result;
}

inline vec3 Cross(vec3 A, vec3 B)
{
    vec3 Result;
    
    Result.x = A.y * B.z - A.z * B.y;
    Result.y = A.z * B.x - A.x * B.z;
    Result.z = A.x * B.y - A.y * B.x;
    
    return Result;
}

inline real32 Inner(vec3 A, vec3 B)
{
    real32 Result = A.x * B.x + A.y * B.y + A.z * B.z;
    return Result;
}

inline vec3 Hadamard(vec3 A, vec3 B)
{
    vec3 Result;
    
    Result.x = A.x * B.x;
    Result.y = A.y * B.y;
    Result.z = A.z * B.z;
    
    return Result;
}

inline real32 LengthSq(vec3 A)
{
    real32 Result = Inner(A, A);
    return Result;
}

inline real32 Length(vec3 A)
{
    real32 Result = (real32) sqrt(LengthSq(A));
    return Result;
}

inline vec3 Normalize(vec3 A)
{
    vec3 Result = A * (1.0f / Length(A));
    return Result;
}

inline vec3 NormalizeOrZero(vec3 A)
{
    vec3 Result = {};
    
    real32 LenSq = LengthSq(A);
    if (LenSq > powf(0.0001f, 2))
    {
        Result = Normalize(A);
    }
    return Result;
}

inline vec3 Lerp(vec3 A, real32 t, vec3 B)
{
    return (A * (1 - t)) + (B * t);
}

inline vec4 operator*(vec4 A, real32 B)
{
    vec4 Result;
    
    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;
    Result.w = A.w * B;
    
    return Result;
}

inline vec4 operator/(vec4 A, real32 B)
{
    vec4 Result;
    B = 1 / B;
    
    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;
    Result.w = A.w * B;
    
    return Result;
}

inline real32 Inner(vec4 A, vec4 B)
{
    real32 Result = A.x * B.x + A.y * B.y + A.z * B.z + A.w * B.w;
    return Result;
}

inline real32 LengthSq(vec4 A)
{
    real32 Result = Inner(A, A);
    return Result;
}

inline real32 Length(vec4 A)
{
    real32 Result = sqrtf(LengthSq(A));
    return Result;
}

inline vec4 Normalize(vec4 A)
{
    vec4 Result = A * (1.0f / Length(A));
    return Result;
}

typedef struct mat4x4
{
    real32 E[4][4];
} mat4x4;

inline mat4x4 Identity()
{
    mat4x4 R = {};
    
    for (int i = 0; i < 4; i++)
    {
        R.E[i][i] = 1;
    }
    return R;
}

inline mat4x4 ScalingMat4x4(vec3 S)
{
    mat4x4 I = Identity();
    
    for (int i = 0; i < 3; i++)
    {
        I.E[i][i] *= S.E[i];
    }
    return I;
}

inline mat4x4 transMat4x4(mat4x4 A, vec3 T)
{
    for (int i = 0; i < 3; i++)
    {
        A.E[i][3] = T.E[i];
    }
    return A;
}

inline mat4x4 operator*(mat4x4 A, real32 B)
{
    mat4x4 R = {};
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            R.E[r][c] = B * A.E[r][c];
        }
    }
    return R;
}

inline mat4x4 operator*(mat4x4 A, mat4x4 B)
{
    mat4x4 R = {};
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            for (int i = 0; i < 4; i++)
            {
                R.E[r][c] += A.E[r][i] + B.E[i][c];
            }
        }
    }
    return R;
}

inline vec4 operator*(mat4x4 A, vec4 B)
{
    vec4 R = {};
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            R.E[r] += A.E[r][c] * B.E[c];
        }
    }
    return R;
}

inline mat4x4 operator+(mat4x4 A, mat4x4 B)
{
    mat4x4 R = {};
    for (int r = 0; r < 4; r++)
    {
        for (int c = 0; c < 4; c++)
        {
            R.E[r][c] += A.E[r][c] + B.E[r][c];
        }
    }
    return R;
}

inline mat4x4 rotateZ(real32 deg)
{
    real32 sinval = (real32) sin(deg);
    real32 cosval = (real32) cos(deg);
    mat4x4 rMat =
    {
        {{cosval, -sinval, 0, 0},
            {sinval, cosval, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1}},
    };
    return rMat;
}

inline mat4x4 rotateX(real32 deg)
{
    real32 sinval = (real32) sin(deg);
    real32 cosval = (real32) cos(deg);
    mat4x4 rMat =
    {
        {{1, 0, 0, 0},
            {0, cosval, -sinval, 0},
            {0, sinval, cosval, 0},
            {0, 0, 0, 1}},
    };
    return rMat;
}

inline mat4x4 transpose(mat4x4 A)
{
    mat4x4 R;
    for (int j = 0; j < 4; j++)
    {
        for (int i = 0; i < 4; i++)
        {
            R.E[j][i] = A.E[i][j];
        }
    }
    return R;
}

inline mat4x4 projection(real32 AspectWithOverHeight, real32 FocalLength)
{
    real32 a = 1.0f;
    real32 b = AspectWithOverHeight;
    real32 c = FocalLength;
    mat4x4 R =
    {
        {{a, 0, 0, 0},
            {0, b, 0, 0},
            {0, 0, 1, 0},
            {0, 0, c, 0}}};
    
    return R;
}

inline uint32 RGBAPack4x8(vec4 Unpacked)
{
    uint32 Result = ((RoundReal32ToUint32(Unpacked.w) << 24) |
                     (RoundReal32ToUint32(Unpacked.z) << 16) |
                     (RoundReal32ToUint32(Unpacked.y) << 8) |
                     (RoundReal32ToUint32(Unpacked.x) << 0));
    
    return Result;
}

inline uint32 BGRAPack4x8(vec4 Unpacked)
{
    uint32 Result = ((RoundReal32ToUint32(Unpacked.x) << 24) |
                     (RoundReal32ToUint32(Unpacked.y) << 16) |
                     (RoundReal32ToUint32(Unpacked.z) << 8) |
                     (RoundReal32ToUint32(Unpacked.w) << 0));
    
    return Result;
}

inline vec4 RGBAUnpack4x8(uint32 Packed)
{
    vec4 Result = {};
    Result.x = (real32)((Packed >> 0) & 0xFF);
    Result.y = (real32)((Packed >> 8) & 0xFF);
    Result.z = (real32)((Packed >> 16) & 0xFF);
    Result.w = (real32)((Packed >> 24) & 0xFF);
    return Result;
}

inline real32 LinearTosRGB(real32 L)
{
    if (L < 0.0001f)
        L = 0.0f;
    if (L > 1.0f)
        L = 1.0f;
    
    real32 S = L * 12.92f;
    ;
    if (L > 0.0031308f)
    {
        S = (1.055f * powf(L, (1.0f / 2.4f))) - 0.055f;
    }
    return S;
}

// TODO: Better random number generator than this one current
inline real32 RandomUnilateral()
{
    return rand() / (float)RAND_MAX;
}

inline real32 RandomBilateral()
{
    return 2.0f * RandomUnilateral() - 1.0f;
}

#endif //SIMPLE_MATH_H
