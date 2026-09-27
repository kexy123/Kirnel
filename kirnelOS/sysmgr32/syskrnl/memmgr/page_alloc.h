#pragma once

#define PAGE_SIZE_EXP (12)             // The exponent of the size of a page in bytes in memory.
#define PAGE_SIZE (1 << PAGE_SIZE_EXP) // The number of bytes in a page in memory.

/// @brief Allocates and returns the starting address to a free memory of the given number of pages.
/// @param numPages The number of pages to allocate.
/// @return The starting memory address of the free pages.
void *allocate_strict(unsigned long numPages);

/// @brief Frees the given address with the given order.
/// @param order The order of the address.
/// @param address The starting address.
void deallocate(int order, void *address);

/// @brief Initialises the memory allocation table.
void init_allocator();