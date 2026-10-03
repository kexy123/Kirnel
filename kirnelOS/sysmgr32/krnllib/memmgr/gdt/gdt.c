#include "gdt.h"
#include "memmgr/paging/page_alloc.h"
#include "utils/memcopy.h"

/// @brief The location of the global descriptor table.
DescriptorTable *table;

/// @brief The descriptor defining metadata of the global descriptor table.
TableDescriptorRegister memDescriptor;

/// @brief The number of entries in the current descriptor table.
unsigned short maxEntries;

/// @brief Loads the given global descriptor table to the CPU.
/// @param descriptor The descriptor of the global descriptor table.
extern void load_gdt(TableDescriptorRegister *descriptor);

void generate_descriptor()
{
    memDescriptor.SegmentStart = table = (DescriptorTable *)allocate_strict(1);

    // Zero-fill the first entry.
    zero_fill(table, sizeof(SegmentEntry));

    maxEntries = 1;
    memDescriptor.Length = sizeof(SegmentEntry) - 1;
}

void add_segment(unsigned short index, unsigned long base, unsigned long limit, SegmentAttributes attributes, _Bool granular, SegmentSize size)
{
    (*table)[index] = (SegmentEntry){
        .LimitLow = (unsigned short)limit, // Extract low 16 bits of the limit.
        .BaseLow = base & 0x00FFFFFF,      // Extract low 24 bits of the base.
        .Attributes = attributes,
        .LimitHigh = (limit >> 16) & 0x000F, // Extract next 4 bits of the limit.

        .Reserved = 0,
        .LongMode = size == Bits64,
        .DescriptorSize = size == Bits32,
        .Granularity = granular,

        .BaseHigh = base >> 24, // Extract last 8 bits of the base.
    };

    index++;
    if (index > maxEntries)
    {
        maxEntries = index;
        memDescriptor.Length = maxEntries * sizeof(SegmentEntry) - 1;
    }
}

void init_gdt()
{
    load_gdt(&memDescriptor);
}