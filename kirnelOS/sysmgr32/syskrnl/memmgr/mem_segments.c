#include "mem_segments.h"
#include "utils/flow.h"

#define MEMORY_SEGMENT_START ((MemorySegmentEntry *const)(0x00007E00))

/// @brief The starting memory segment entry.
MemorySegmentEntry *startSegment = MEMORY_SEGMENT_START;

unsigned long memoryStart;
unsigned long memoryEnd;

/// @brief Gets the range of memory that was specified by the BIOS.
void get_memory_range()
{
    unsigned long low = 0xFFFFFFFF;
    unsigned long high = 0x00000000;

    MemorySegmentEntry *memorySegment = startSegment;
    while (1)
    {
        if (!memorySegment->SegmentLength)
        {
            // Reached the end of the memory segment table.
            break;
        }

        // Update low if surpassed.
        if ((unsigned long)memorySegment->BaseAddress < low)
        {
            low = (unsigned long)memorySegment->BaseAddress;
        }

        // Update high if surpassed.
        if ((unsigned long)memorySegment->BaseAddress + (unsigned long)memorySegment->SegmentLength > high)
        {
            high = (unsigned long)memorySegment->BaseAddress + (unsigned long)memorySegment->SegmentLength;
        }

        memorySegment++;
    }

    if (high < low)
    {
        panic();
    }

    memoryStart = low;
    memoryEnd = high;
}

void analyse_mem_segments()
{
    get_memory_range();
}