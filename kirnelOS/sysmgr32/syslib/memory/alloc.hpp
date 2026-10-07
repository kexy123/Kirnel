#pragma once

#include "size.hpp"

/// @brief Allocates a place in the heap for the given number of bytes. Note that the new object may have internal fragmentation.
/// @param size The number of bytes to allocate.
/// @return The starting location of the usable data.
void *operator new(SizeType size);

/// @brief Allocates a place in the heap for the given number of bytes. Note that the new object may have internal fragmentation.
/// @param size The number of bytes to allocate.
/// @return The starting location of the usable data.
void *operator new[](SizeType size);

/// @brief Frees the given object from the heap.
/// @param object The object to free.
void operator delete(void *object) noexcept;

/// @brief Frees the given object from the heap.
/// @param object The object to free.
void operator delete[](void *object) noexcept;