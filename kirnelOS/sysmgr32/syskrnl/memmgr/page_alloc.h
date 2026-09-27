#pragma once

#define ALLOC_START ((AllocationNode *)(0x00020000)) // Starting location of the memory allocation tree.

#define PAGE_SIZE_EXP (12)             // The exponent of the size of a page in bytes in memory.
#define PAGE_SIZE (1 << PAGE_SIZE_EXP) // The number of bytes in a page in memory.

/// @brief An allocation node. The position of the allocation node and in its order determines the size of the starting page it's pointing to.
typedef struct __attribute__((packed)) AllocNode
{
    /// @brief The previous allocation node in its order list.
    struct AllocNode *Previous;

    /// @brief The next allocation node in its order list.
    struct AllocNode *Next;
} AllocationNode;

/// @brief The number of bytes of the allocation tree.
extern unsigned long allocationTreeLength;

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