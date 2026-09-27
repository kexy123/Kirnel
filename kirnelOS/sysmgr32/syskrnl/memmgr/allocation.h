#pragma once

/// @brief Allocates and returns the starting address to a free memory of the given number of pages.
/// @param numPages The number of pages to allocate.
/// @return The starting memory address of the free pages.
void *allocate_strict(unsigned long numPages);

/// @brief Initialises the memory allocation table.
void init_allocator();