/* date = October 14th 2026 7:24 am */

#ifndef CONTAINERS_H
#define CONTAINERS_H

// NOTE: LIFO: Last In First Out
struct stack
{
	memory_arena* MemoryArena;
	void* Top;
	void* Bottom;
	uint32 Size;
};

// NOTE: FIFO: First In First Out
struct queue
{
	memory_arena* MemoryArena;
	void* Top;
	void* Bottom;
	uint32 Size;
}

struct list
{
	memory_arena* MemoryArena;
	uint32 Size;
}

#endif //CONTAINERS_H
