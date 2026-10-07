#include "alloc.hpp"
#include "size.hpp"
#include "memory.h"

void *operator new(SizeType size)
{
    return malloc(size);
}

void *operator new[](SizeType size)
{
    return malloc(size);
}

void operator delete(void *object) noexcept
{
    free(object);
}

void operator delete[](void *object) noexcept
{
    free(object);
}