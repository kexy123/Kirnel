#include "master.h"
#include "gdt.h"

// The segments are binary data structures: https://wiki.osdev.org/Segment_Selector

unsigned short kernelCodeSegment;
unsigned short kernelDataSegment;

unsigned short userCodeSegment;
unsigned short userDataSegment;

void create_os_gdt()
{
    generate_descriptor();

    kernelCodeSegment = add_segment(1, 0x00000000, 0xFFFFF, (SegmentAttributes){.Raw = 0b10011010}, 1, Bits32); // 0x08
    kernelDataSegment = add_segment(2, 0x00000000, 0xFFFFF, (SegmentAttributes){.Raw = 0b10010010}, 1, Bits32); // 0x10

    userCodeSegment = add_segment(3, 0x00000000, 0xBFFFF, (SegmentAttributes){.Raw = 0b11111010}, 1, Bits32); // 0x1B
    userDataSegment = add_segment(4, 0x00000000, 0xBFFFF, (SegmentAttributes){.Raw = 0b11110010}, 1, Bits32); // 0x23

    init_gdt();
}

void check_os_gdt()
{
    kernelCodeSegment = 0x08;
    kernelDataSegment = 0x10;

    userCodeSegment = 0x1B;
    userDataSegment = 0x23;
}