#include "memory.h"

void 
ZeroSize(uint64 Size, void* Ptr)
{
	uint8 *Byte = (uint8 *)Ptr;
    
	while(Size--)
    {
        *Byte++ = 0;
    }
}

void 
MemoryCopy(uint64 Size, void *SourceInit, void *DestInit)
{
    uint8 *Source = (uint8 *)SourceInit;
    uint8 *Dest = (uint8 *)DestInit;
    
	while(Size--) 
	{ 
		*Dest++ = *Source++; 
	}
}

uint64 
StringLength(const char* Str)
{
    uint64 Count = 0;
    
	while(*Str++) 
	{ 
		Count++; 
	}
    
	return Count;
}

bool32
IsPowerOf2(uint64 X)
{
	Assert(X > 0);
	return (X & (X - 1)) == 0;
}


uint64 
MemoryAddressAlignForward(uint64 Ptr, uint64 Align)
{
	uint64 P;
	uint64 A;
	uint64 Modulo;
	
	Assert(IsPowerOf2(Align));
	
	P = Ptr;
	A = (uint64)Align;
	
	Modulo = P & (A - 1);
	if(Modulo != 0)
	{
		// NOTE: If P address is not alligned,
		// push the address to the next value which is alligned
		P += A - Modulo;
	}
	
	return P;
}

void*
MemoryArenaAllocate(memory_arena* Arena, uint64 Size, uint64 Align)
{
	uint64 CurrPtr = (uint64) Arena->Buffer + (uint64) Arena->CurrOffset;
	uint64 Offset = MemoryAddressAlignForward(CurrPtr, Align);
	// NOTE: Change to relative offset
	Offset -= (uint64) Arena->Buffer;
	
	if(Offset + Size <= Arena->Size)
	{
		void* Ptr = &Arena->Buffer[Offset];
		Arena->PrevOffset = Offset;
		Arena->CurrOffset = Offset + Size;
		
		// NOTE: Zero the new memory by default
		ZeroSize(Size, Ptr);
		return Ptr;
	}
	
	// TODO: Memory arena is out of memory, need to handle it intelligently
	return NULL;
}

void*
MemoryArenaAllocate(memory_arena* Arean, uint64 Size)
{
	return MemoryArenaAllocate(Arean, Size, DEFAULT_ALIGNMENT);
}

void* 
PushCopy_(memory_arena* Arena, uint64 Size, void* Source)
{
    void* Dest = PushSizeNoClear(Arena, Size);
    
	if(Dest) 
	{ 
		MemoryCopy(Size, Source, Dest); 
	}
    
	return Dest;
}

char* 
PushStringZ_(memory_arena* Arena, const char* Source)
{
	// NOTE: +1 for null terminator
    uint64 Size = StringLength(Source) + 1; 
    char* Dest = (char*)PushSizeNoClear(Arena, Size);
    
	if(Dest) 
	{ 
		MemoryCopy(Size, (void*)Source, Dest); 
	}
    
	return Dest;
}

void
MemoryArenaInit(memory_arena* Arena, void* BackingBuffer, uint64 BackingBufferLenght)
{
	Arena->Buffer = (uint8*)BackingBuffer;
	Arena->Size = BackingBufferLenght;
	Arena->CurrOffset = 0;
	Arena->PrevOffset = 0;
}

void* 
BootstrapPushSize_(uint64 MemoryArenaSize, uint64 StructSize)
{
    void* BackingBuffer = malloc(MemoryArenaSize); 
    memory_arena* Arena = (memory_arena*)BackingBuffer;
    MemoryArenaInit(Arena, BackingBuffer, MemoryArenaSize);
    
    // NOTE: Reserve space for the arena itself without zeroing it out
    // Since memory_arena is exactly 32 bytes, we just advance the offset.
    Arena->CurrOffset = sizeof(memory_arena);
	
    void* Struct = MemoryArenaAllocate(Arena, StructSize);
	
    // NOTE: Because the arena pointer is the very first variable in the struct, 
    // we can just set it directly.
    *(memory_arena**)Struct = Arena;
	
    return Struct;
}

void*
MemoryArenaResize(memory_arena* Arena, void* OldMemory, uint64 OldMemorySize, uint64 NewMemorySize, uint64 Align)
{
	uint8* Memory = (uint8*) OldMemory;
	
	Assert(IsPowerOf2(Align));
	
	if(Memory == NULL || OldMemorySize == 0)
	{
		return MemoryArenaAllocate(Arena, NewMemorySize, Align);
	}
	else if(Arena->Buffer <= OldMemory && OldMemory < Arena->Buffer + Arena->Size)
	{
		if(Arena->Buffer + Arena->PrevOffset == OldMemory)
		{
			Arena->CurrOffset = Arena->PrevOffset + NewMemorySize;
			if(NewMemorySize > OldMemorySize)
			{
				// NOTE: Zero the memory by default
				ZeroSize(NewMemorySize - OldMemorySize, &Arena->Buffer[Arena->PrevOffset + OldMemorySize]);
			}
			return OldMemory;
		}
		else
		{
			void* NewMemory = MemoryArenaAllocate(Arena, NewMemorySize, Align);
			uint64 CopySize = OldMemorySize < NewMemorySize ? OldMemorySize : NewMemorySize;
			// NOTE: copy across old memory to the new memory
			MemoryCopy(CopySize, OldMemory, NewMemory);
			return NewMemory;
		}
	}
	else
	{
		Assert(0 && "Memory is out of bounds of the buffer in this arena");
		return NULL;
	}
}

void 
MemoryArenaFreeAll(memory_arena* Arena)
{
	Arena->CurrOffset = 0;
	Arena->PrevOffset = 0;
}

temp_memory_arena
TempMemoryArenaBegin(memory_arena* Arena)
{
	temp_memory_arena TempArena;
	
	TempArena.Arena = Arena;
	TempArena.PrevOffset = Arena->PrevOffset;
	TempArena.CurrOffset = Arena->CurrOffset;
	
	return TempArena;
}

void
TempMemoryArenaEnd(temp_memory_arena TempArena)
{
	TempArena.Arena->PrevOffset = TempArena.PrevOffset;
	TempArena.Arena->CurrOffset = TempArena.CurrOffset;
}
