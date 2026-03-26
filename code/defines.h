/* date = March 24th 2026 1:45 pm */

#ifndef DEFINES_H
#define DEFINES_H

#include <stdint.h>
#include <float.h>
#include <stdlib.h>

// TODO: Can we make it sdl independent? So that we can use opengl in future??
#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include <stdio.h>
#include <string>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;

typedef float real32;
typedef double real64;

#define internal static
#define local_persist static
#define global static

#include <math.h>

#define Assert(Expression) if(!(Expression)) { *(int*)0 = 0; }

#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

#define Kilobytes(Value) (Value * 1024LL)
#define Megabytes(Value) (Kilobytes(Value) * 1024LL)
#define Gigabytes(Value) (Megabytes(Value) * 1024LL)
#define Terabytes(Value) (Gigabytes(Value) * 1024LL)

// TODO: SWAP min, max macros
#define SWAP(TYPE, A, B) {TYPE _MACRO_TEMP_; _MACRO_TEMP_ = A; A = B; B = _MACRO_TEMP_;} 


#endif //DEFINES_H
