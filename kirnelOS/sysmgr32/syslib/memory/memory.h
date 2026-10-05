#pragma once

#define __CODE_SPACE (0x00000000UL) // The starting location of the process code space.

#define __HEAP_START (0x10000000UL) // The starting location of the process heap space.
#define __HEAP_END (0xBF800000UL)   // The ending location of the process heap space.

#define __HEAP_MAX_SIZE (__HEAP_END - __HEAP_START) // The maximum possible size of the heap.

#define __STACK_START (0xBF800000UL) // The starting (bottom) location of the stack.
#define __STACK_END (0xC0000000UL)   // The ending (top) location of the stack.

#define __ADDRESS_SIZE (4UL) // The size of a memory address in bytes.

#define __HEAP_ATOMIC (3)      // The smallest order whose block of free bytes can be allocated from the heap. It is 8 bytes.
#define __HEAP_SMALLEST (20)   // The smallest number of bytes for a block in the heap including the header.
#define __HEAP_NUM_ORDERS (28) // The number of orders in the heap free lists.

#define __HEAP_HEADER_SIZE (12) // The number of bytes in the header of a block in the heap disregarding the linked list.

/// @brief Allocates memory from the heap.
/// @param size The number of bytes to allocate. Note that you may not get the exact number of bytes.
/// @return The starting pointer of the free memory.
void *malloc(unsigned long size);

/// @brief Frees memory from the heap.
/// @param object The object to free.
void free(void *object);