#pragma once

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

/// @brief The address of the alloocation tree.
extern const char *treeLocation;

/// @brief Allocates and returns the starting physical address to a free zeroed-out memory chunk of the given number of pages.
/// @param numPages The number of pages to allocate.
/// @return The starting physical memory address of the free pages.
void *allocate_strict(unsigned long numPages);

/// @brief Frees the given physical address with the given order.
/// @param order The order of the address.
/// @param address The starting address. Must be physical.
void deallocate(int order, void *address);

/// @brief Initialises the memory allocation table.
void init_allocator();