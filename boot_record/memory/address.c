#include "address.h"

void nearptr_to_seg_off(const void *objPtr, unsigned short *segment, unsigned short *offset)
{
    *segment = 0x0000; // TODO: Match the DS from boot.asm if needed.
    *offset = (unsigned short)(objPtr);
}