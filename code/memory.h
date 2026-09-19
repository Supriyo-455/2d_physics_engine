/* date = September 18th 2026 8:33 pm */

#ifndef MEMORY_H
#define MEMORY_H

struct memory_arena
{
	uint8* Buffer;
	uint64 Size;
	uint64 PrevOffset;
	uint64 CurrOffset;
};

struct temp_memory_arena
{
	memory_arena* Arena;
	uint64 PrevOffset;
	uint64 CurrOffset;
};

#define Kilobytes(Value) (Value * 1024LL)
#define Megabytes(Value) (Kilobytes(Value) * 1024LL)
#define Gigabytes(Value) (Megabytes(Value) * 1024LL)
#define Terabytes(Value) (Gigabytes(Value) * 1024LL)

#define DEFAULT_ALIGNMENT (2*sizeof(void *))


#define PushStruct(Arena, type) (type*)MemoryArenaAllocate((Arena), sizeof(type))
#define PushArray(Arena, Count, type) (type*)MemoryArenaAllocate((Arena), (Count)*sizeof(type))
#define PushSize(Arena, Size) MemoryArenaAllocate((Arena), (Size))
#define PushSizeNoClear(Arena, Size) MemoryArenaAllocate((Arena), (Size))

#define ZeroStruct(Instance) ZeroSize(sizeof(Instance), &(Instance))
#define ZeroArray(Count, Pointer) ZeroSize((Count)*sizeof((Pointer)[0]), Pointer)

#define BootstrapPushStruct(type, ArenaSize) (type*)BootstrapPushSize_(ArenaSize, sizeof(type))

#endif //MEMORY_H
