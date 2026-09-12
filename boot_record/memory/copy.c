#include "copy.h"

void copy_to(void *source, void *destination, short byteCount)
{
    char *sourcePtr = (char *)source;
    char *destinationPtr = (char *)destination;
    
    for (short i = 0; i < byteCount; i++)
    {
        sourcePtr[i] = destinationPtr[i];
    }
}