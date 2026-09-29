#include "master.h"
#include "gdt.h"

void create_os_gdt()
{
    generate_descriptor();

    append_segment(0x00000000, 0x00FFFFFF, &(SegmentAttributes){.Raw = 0b10011010}, &(SegmentFlags){.Raw = 0b1100}); // Kernel code segment.

    append_segment(0x00000000, 0x00FFFFFF, &(SegmentAttributes){.Raw = 0b10010010}, &(SegmentFlags){.Raw = 0b1000}); // Kernel data segment.

    init_gdt();
}