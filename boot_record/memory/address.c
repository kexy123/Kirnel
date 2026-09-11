#include "address.h"

void ptr_to_seg_off(const void *objPtr, unsigned short *segment, unsigned short *offset)
{
    unsigned int ptr = (unsigned int)objPtr;

    *segment = (unsigned short)(ptr >> 4);
    *offset = (unsigned short)(ptr & 0xF);
}