#include "memcopy.h"

void copy_to(void *source, void *destination, short byteCount)
{
    char *sourcePtr = (char *)source;
    char *destinationPtr = (char *)destination;

    for (short i = 0; i < byteCount; i++)
    {
        sourcePtr[i] = destinationPtr[i];
    }
}

void zero_fill(void *source, unsigned long bytes)
{
    char *sourcePtr = (char *)source;

    for (unsigned long i = 0; i < bytes; i++)
    {
        sourcePtr[i] = 0x00;
    }
}

void one_fill(void *source, unsigned long bytes)
{
    char *sourcePtr = (char *)source;

    for (unsigned long i = 0; i < bytes; i++)
    {
        sourcePtr[i]= 0xFF;
    }
}