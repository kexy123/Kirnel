#include "address.h"

void nearptr_to_seg_off(const void *objPtr, unsigned short *segment, unsigned short *offset)
{
    *segment = 0x0000;
    *offset = (unsigned short)(objPtr);
}