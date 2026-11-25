#include "memory/memcopy.h"

void copy_to(void *source, void *destination, unsigned long byteCount)
{
    char *sourcePtr = (char *)source;
    char *destinationPtr = (char *)destination;

    for (unsigned long i = 0; i < byteCount; i++)
    {
        destinationPtr[i] = sourcePtr[i];
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

void byte_fill(void *source, unsigned long bytes, char character)
{
    char *sourcePtr = (char *)source;

    for (unsigned long i = 0; i < bytes; i++)
    {
        sourcePtr[i] = character;
    }
}