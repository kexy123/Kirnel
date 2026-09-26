#include "allocation.h"
#include "mem_segments.h"
#include "utils/bit.h"
#include "utils/flow.h"

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

/// @brief The number of bytes of the allocation tree.
unsigned long allocationTreeLength;

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

/// @brief Computes the length of the allocation tree in bytes and the number of AllocationNodes for each existing order.
void compute_allocation_tree_length()
{
    allocationTreeLength = 0;
    for (int i = 0; i <= highestOrder; i++)
    {
        // Each order has one extra element on a power of two. Note that order 0 is the deepest in the tree.
        allocation_tree[highestOrder - i] = (AllocationNode *)allocationTreeLength;
        allocationTreeLength += ((1 << i) + 1) * sizeof(AllocationNode);
    }
}

/// @brief Locates a sufficient memory segment that can store the allocation tree.
/// @return The starting pointer of the memory segment that can be used.
AllocationNode *find_sufficient_tree()
{
    for (int i = 0; i < numSegments; i++)
    {
        if (memorySegments[i].RegionType != Usable)
        {
            continue;
        }

        if (memorySegments[i].SegmentLength < allocationTreeLength)
        {
            continue;
        }

        return (AllocationNode *)(memorySegments[i].BaseAddress);
    }

    // No sufficient location to store the allocation tree.
    panic();
    return (AllocationNode *)0;
}

void init_allocator()
{
    highestOrder = lowest_exp2(memoryEnd) - PAGE_SIZE_EXP;
    compute_allocation_tree_length();

    AllocationNode *start = find_sufficient_tree();
    for (int i = 0; i <= highestOrder; i++)
    {
        // Add the offset to each order.
        allocation_tree[i] += (unsigned long)start;
    }
}