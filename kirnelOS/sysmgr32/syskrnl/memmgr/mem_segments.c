#include "mem_segments.h"
#include "utils/flow.h"

#define MEMORY_SEGMENT_START ((const MemorySegmentEntry *const)(0x00007E00))

const MemorySegmentEntry *const memorySegments = MEMORY_SEGMENT_START;

unsigned long numSegments;

unsigned long memoryStart;
unsigned long memoryEnd;

/// @brief Counts the number of memory segments.
void count_memory_segments()
{
    numSegments = 0;

    const MemorySegmentEntry *memorySegment = memorySegments;
    while (1)
    {
        if (!memorySegment->SegmentLength)
        {
            return;
        }

        memorySegment++;
        numSegments++;
    }
}

/// @brief Gets the range of memory that was specified by the BIOS.
void get_memory_range()
{
    unsigned long low = 0xFFFFFFFF;
    unsigned long high = 0x00000000;

    for (int i = 0; i < numSegments; i++)
    {
        // Update low if surpassed.
        if ((unsigned long)memorySegments[i].BaseAddress < low)
        {
            low = (unsigned long)memorySegments[i].BaseAddress;
        }

        // Update high if surpassed.
        if ((unsigned long)memorySegments[i].BaseAddress + (unsigned long)memorySegments[i].SegmentLength > high)
        {
            high = (unsigned long)memorySegments[i].BaseAddress + (unsigned long)memorySegments[i].SegmentLength;
        }
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
    count_memory_segments();

    get_memory_range();
}