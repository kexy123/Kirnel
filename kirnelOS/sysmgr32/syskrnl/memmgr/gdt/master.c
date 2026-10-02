#include "master.h"
#include "gdt.h"

void create_os_gdt()
{
    generate_descriptor();

    add_segment(1, 0x00000000, 0x00FFFFFF, (SegmentAttributes){.Raw = 0b10011010}, 1, Bits32); // Kernel code segment.
    add_segment(2, 0x00000000, 0x00FFFFFF, (SegmentAttributes){.Raw = 0b10010010}, 1, Bits32); // Kernel data segment.

    init_gdt();
}