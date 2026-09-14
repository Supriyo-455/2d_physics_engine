/* date = September 11th 2026 6:50 pm */

#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#if ENGINE_DEBUG
#define BEGIN_TIMED_BLOCK_(StartCycleCount) StartCycleCount = __rdtsc();
#define BEGIN_TIMED_BLOCK(ID) uint64 BEGIN_TIMED_BLOCK_(StartCycleCount##ID)
#define END_TIMED_BLOCK_(StartCycleCount, ID) DebugGlobalMemory->Counters[ID].CycleCount += __rdtsc() - StartCycleCount; ++DebugGlobalMemory->Counters[ID].HitCount;
#define END_TIMED_BLOCK(ID) END_TIMED_BLOCK_(StartCycleCount##ID, DebugCycleCounter_##ID)
// TODO(casey): Clamp END_TIMED_BLOCK_COUNTED so that if the calc is wrong, it won't overflow!
#define END_TIMED_BLOCK_COUNTED(ID, Count) DebugGlobalMemory->Counters[DebugCycleCounter_##ID].CycleCount += __rdtsc() - StartCycleCount##ID; DebugGlobalMemory->Counters[DebugCycleCounter_##ID].HitCount += (Count);
#else
#define BEGIN_TIMED_BLOCK(ID) 
#define END_TIMED_BLOCK(ID)
#define END_TIMED_BLOCK_COUNTED(ID, Count)
#endif

#if ENGINE_DEBUG
#define TIMED_BLOCK(ID) timed_block TimedBlock##ID(DebugCycleCounter_##ID);

struct timed_block
{
    uint64 StartCycleCount;
    u32 ID;
    
    timed_block(uint32 IDInit)
    {
        ID = IDInit;
        BEGIN_TIMED_BLOCK_(StartCycleCount);
    }
    
    ~timed_block()
    {
        END_TIMED_BLOCK_(StartCycleCount, ID);
    }
};

#else
#define TIMED_BLOCK(ID)
#endif

#endif //PERFORMANCE_H
