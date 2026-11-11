#ifndef PERFORMANCE_H
#define PERFORMANCE_H

struct debug_record
{
    const char* BlockName;
    const char* FileName;
    uint32 LineNumber;
    uint64 CycleCount;
    uint32 HitCount;
};

extern struct debug_record DebugRecordArray[];

#if ENGINE_DEBUG

// TODO: Tree structure for related timed_blocks and timed_functions?
struct timed_block
{
    debug_record* Record;
    timed_block(uint32 Counter, const char* BlockName, const char* FileName, uint32 LineNumber)
    {
        Record = DebugRecordArray + Counter;
        Record->BlockName  = BlockName;
        Record->FileName   = FileName;
        Record->LineNumber = LineNumber;
        Record->CycleCount -= __rdtsc();
        Record->HitCount++;
    }
    ~timed_block()
    {
        Record->CycleCount += __rdtsc();
    }
};


#define TIMED_BLOCK__(BlockString, Line) timed_block TimedBlock##Line(__COUNTER__, BlockString, __FILE__, Line)
#define TIMED_BLOCK_(BlockString, Line)  TIMED_BLOCK__(BlockString, Line)

#define TIMED_BLOCK(BlockName) TIMED_BLOCK_(#BlockName, __LINE__)

#define TIMED_FUNCTION TIMED_BLOCK_(__FUNCTION__, __LINE__)

#else

#define TIMED_BLOCK(BlockName)
#define TIMED_FUNCTION

#endif // ENGINE_DEBUG
#endif // PERFORMANCE_H