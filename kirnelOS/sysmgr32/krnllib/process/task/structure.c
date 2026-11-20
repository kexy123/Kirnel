#include "structure.h"
#include "memmgr/kernel_virtual_layout.h"
#include "memmgr/gdt/master.h"
#include "memmgr/gdt/gdt.h"
#include "memory/memory.h"

TSSObject create_tss()
{
    TaskStateSegment *location = calloc(sizeof(TaskStateSegment));
    *location = (TaskStateSegment){
        .ESP0 = kernelStack1End.Address,
        .SS0 = kernelDataSegment,
    };

    unsigned long segmentSelector = append_segment((unsigned long)location, sizeof(TaskStateSegment) - 1, (SegmentAttributes){.Raw = 0b10001001}, 0, 0);
    return (TSSObject){
        .Selector = segmentSelector,
        .TSS = location,
    };
}
