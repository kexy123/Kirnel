#include "mem_segments.h"

#define MEMORY_SEGMENT_START ((const MemorySegmentEntry *const)(0x00007E00))

const MemorySegmentEntry *memorySegments = MEMORY_SEGMENT_START;