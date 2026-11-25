#include "memory/alloc.hpp"
#include "memory/memory.h"
#include "size.hpp"

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