/* date = March 24th 2026 1:45 pm */

#ifndef DEFINES_H
#define DEFINES_H

#include <stdio.h>
#include <string.h>
#include <string>
#include <stdarg.h>
#include <math.h>
#include <stdint.h>
#include <float.h>
#include <stdlib.h>

#ifndef _MSC_VER
#define sprintf_s(buf, ...) snprintf(buf, sizeof(buf), __VA_ARGS__)
#endif

// TODO: Need to use Opengl in future
#define SDL_MAIN_HANDLED
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

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

#define Assert(Expression) if(!(Expression)) { *(int*)0 = 0; }

#define ARRAY_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

#define Kilobytes(Value) (Value * 1024LL)
#define Megabytes(Value) (Kilobytes(Value) * 1024LL)
#define Gigabytes(Value) (Megabytes(Value) * 1024LL)
#define Terabytes(Value) (Gigabytes(Value) * 1024LL)


#define MIN(A, B) ((A < B) ? A : B)

#define MAX(A, B) ((A > B) ? A : B)

#define SWAP(TYPE, A, B) { TYPE _MACRO_TEMP_; _MACRO_TEMP_ = A; A = B; B = _MACRO_TEMP_; } 

void 
__LogMessage(FILE* Stream, const char* File, int Line, const char* Prefix, const char* Format, ...) {
    char Buffer[2048];
    int HeaderLength = snprintf(Buffer, sizeof(Buffer), "%s:%d - %s - ", File, Line, Prefix);
    
    va_list Args;
    va_start(Args, Format);
    
    // NOTE: only write into the remaining space in the buffer
    vsnprintf(Buffer + HeaderLength, sizeof(Buffer) - HeaderLength, Format, Args);
    va_end(Args);
    
    fputs(Buffer, Stream);
}

#define LOG_ERROR(MSG, ...) \
__LogMessage(stderr, __FILE__, __LINE__, "ERROR", MSG, ##__VA_ARGS__)

#define LOG_WARNING(MSG, ...) \
__LogMessage(stdout, __FILE__, __LINE__, "WARNING", MSG, ##__VA_ARGS__)

#define LOG_INFO(MSG, ...) \
__LogMessage(stdout, __FILE__, __LINE__, "INFO", MSG, ##__VA_ARGS__)


#endif //DEFINES_H
