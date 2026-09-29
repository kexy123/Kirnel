#include "gdt.h"
#include "memmgr/paging/page_alloc.h"

/// @brief The starting entry of the global descriptor table.
SegmentEntry *table;

/// @brief The descriptor defining metadata of the global descriptor table.
TableDescriptorRegister memDescriptor;

/// @brief Loads the given global descriptor table to the CPU.
/// @param descriptor The descriptor of the global descriptor table.
extern void load_gdt(TableDescriptorRegister *descriptor);

void generate_descriptor()
{
    memDescriptor.SegmentStart = table = (SegmentEntry *)allocate_strict(1);
    memDescriptor.Length = 1;
}

void append_segment(unsigned long base, unsigned long limit, SegmentAttributes *attributes, SegmentFlags *flags)
{
    table[memDescriptor.Length] = (SegmentEntry){
        .LimitLow = (unsigned short)limit, // Extract low 16 bits of the limit.
        .BaseLow = base & 0x00FFFFFF,      // Extract low 24 bits of the base.
        .Attributes = *attributes,
        .LimitHigh = (limit >> 16) & 0x000F, // Extract next 4 bits of the limit.
        .Flags = *flags,
        .BaseHigh = base >> 24, // Extract last 8 bits of the base.
    };

    memDescriptor.Length++;
}

void init_gdt()
{
    load_gdt(&memDescriptor);
}