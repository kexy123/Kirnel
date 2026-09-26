#include "allocation.h"
#include "mem_segments.h"
#include "utils/bit.h"

#define ALLOC_START ((AllocationNode *)(0x00020000)) // Starting location of the memory allocation tree.
#define PAGE_SIZE_EXP (12)                           // The exponent of the size of a page in bytes in memory.
#define PAGE_SIZE (1 << PAGE_SIZE_EXP)               // The number of bytes in a page in memory.
#define MAXIMUM_ORDERS (21)                          // The maximum number of orders in the buddy allocation tree for 32-bit memory.

/// @brief An allocation node. The position of the allocation node and in its order determines the size of the starting page it's pointing to.
typedef struct __attribute__((packed))
{
    /// @brief The previous allocation node in its order list; 0 to mark the start.
    unsigned long Previous;

    /// @brief The next allocation node in its order list; 0 to mark the end.
    unsigned long Next;
} AllocationNode;

/// @brief The starting pointers of the allocation tree in terms of order.
AllocationNode *allocation_tree[MAXIMUM_ORDERS];

/// @brief The highest order of the allocation tree.
unsigned long highestOrder;

/// @brief Gets an AllocationNode by pointer at the given order and index.
/// @param order The order to go in.
/// @param index The index in the order free list.
/// @return The pointer to the AllocationNode.
inline AllocationNode *get_allocation_node(int order, unsigned long index)
{
    return allocation_tree[order] + index;
}

/// @brief Gets the starting AllocationNode index at the given order.
/// @param order The order to go in.
/// @return The index of the starting AllocationNode.
inline unsigned long get_start(int order)
{
    return get_allocation_node(order, 0)->Next;
}

/// @brief Gets the ending AllocationNode index at the given order.
/// @param order The order to go in.
/// @return The index of the last AllocationNode.
inline unsigned long get_end(int order)
{
    return get_allocation_node(order, 0)->Previous;
}

/// @brief Zero out and connect the adjacent nodes of the AllocationNode together.
/// @param order The order that the AllocationNode is in.
/// @param node The AllocationNode to dissolve.
void dissolve(int order, AllocationNode *node)
{
    AllocationNode *previous = get_allocation_node(order, node->Previous);
    AllocationNode *next = get_allocation_node(order, node->Next);

    previous->Next = node->Next;
    next->Previous = node->Previous;

    node->Previous = 0;
    node->Next = 0;
}

void init_allocator()
{
    highestOrder = lowest_exp2(memoryEnd) - PAGE_SIZE_EXP;
}