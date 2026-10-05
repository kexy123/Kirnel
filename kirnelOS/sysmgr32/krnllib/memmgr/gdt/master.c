#include "master.h"
#include "gdt.h"

const unsigned short kernelCodeSegment = 0x0008;
const unsigned short kernelDataSegment = 0x0010;

const unsigned short userCodeSegment = 0x0018;
const unsigned short userDataSegment = 0x0020;

void create_os_gdt()
{
    generate_descriptor();

    add_segment(1, 0x00000000, 0xFFFFF, (SegmentAttributes){.Raw = 0b10011010}, 1, Bits32); // Kernel code segment.
    add_segment(2, 0x00000000, 0xFFFFF, (SegmentAttributes){.Raw = 0b10010010}, 1, Bits32); // Kernel data segment.

    add_segment(3, 0x00000000, 0xC0000, (SegmentAttributes){.Raw = 0b11111010}, 1, Bits32); // User code segment.
    add_segment(4, 0x00000000, 0xC0000, (SegmentAttributes){.Raw = 0b11110010}, 1, Bits32); // User data segment.

    init_gdt();
}