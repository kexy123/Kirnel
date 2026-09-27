#include "allocation.h"
#include "mem_segments.h"
#include "utils/bit.h"
#include "utils/flow.h"

#define ALLOC_START ((AllocationNode *)(0x00020000)) // Starting location of the memory allocation tree.
#define PAGE_SIZE_EXP (12)                           // The exponent of the size of a page in bytes in memory.
#define PAGE_SIZE (1 << PAGE_SIZE_EXP)               // The number of bytes in a page in memory.
#define MAXIMUM_ORDERS (21)                          // The maximum number of orders in the buddy allocation tree for 32-bit memory.

/// @brief An allocation node. The position of the allocation node and in its order determines the size of the starting page it's pointing to.
typedef struct __attribute__((packed)) AllocNode
{
    /// @brief The previous allocation node in its order list.
    struct AllocNode *Previous;

    /// @brief The next allocation node in its order list.
    struct AllocNode *Next;
} AllocationNode;

/// @brief The starting pointers of the allocation tree in terms of order.
AllocationNode *allocation_tree[MAXIMUM_ORDERS];

/// @brief The highest order of the allocation tree.
unsigned long highestOrder;

/// @brief The number of bytes of the allocation tree.
unsigned long allocationTreeLength;

/// @brief Gets an AllocationNode by pointer at the given order and index.
/// @param order The order to go in.
/// @param index The index in the order free list. Note that it is 1-indexed.
/// @return The pointer to the AllocationNode.
static inline AllocationNode *get_allocation_node(int order, unsigned long index)
{
    return allocation_tree[order] + index - 1;
}

/// @brief Gets the starting AllocationNode index at the given order.
/// @param order The order to go in.
/// @return The index of the starting AllocationNode.
static inline AllocationNode *get_start(int order)
{
    return get_allocation_node(order, 1)->Next;
}

/// @brief Gets the ending AllocationNode index at the given order.
/// @param order The order to go in.
/// @return The index of the last AllocationNode.
static inline AllocationNode *get_end(int order)
{
    return get_allocation_node(order, 1)->Previous;
}

/// @brief Returns the starting address of the page that the given AllocationNode points to.
/// @param order The order the AllocationNode is in.
/// @param node The AllocationNode.
/// @return The starting address of the page.
void *get_page_location(int order, AllocationNode *node)
{
    unsigned long index = node - allocation_tree[order] - 1;
    return (void *)(index << order << PAGE_SIZE_EXP);
}

/// @brief Clears and connects the adjacent nodes of the AllocationNode together.
/// @param node The AllocationNode to dissolve.
void dissolve(AllocationNode *node)
{
    AllocationNode *previous = node->Previous;
    AllocationNode *next = node->Next;

    // Turn previous <-> node <-> next to previous <-> next.
    previous->Next = node->Next;
    next->Previous = node->Previous;

    // Zero-out the AllocationNode.
    node->Previous = (AllocationNode *)0UL;
    node->Next = (AllocationNode *)0UL;
}

/// @brief Inserts an AllocationNode at the given index, without performing any cascading merge operations, to the end of the linked list of its order.
/// @param order The order to insert the AllocationNode in.
/// @param index The page that the AllocationNode points to.
void append(int order, unsigned long index)
{
    AllocationNode *node = get_allocation_node(order, index);

    AllocationNode *central = get_allocation_node(order, 1);
    AllocationNode *end = central->Previous;

    // Turn end <-> central to end <-> node <-> central.
    central->Previous = node;
    node->Next = central;

    end->Next = node;
    node->Previous = end;
}

/// @brief Adds the usable range to the memory allocation tree.
/// @param baseAddress The starting address of the usable range.
/// @param endAddress The ending address of the usable range.
void add_range(unsigned long baseAddress, unsigned long endAddress)
{
    // Align to pages.
    unsigned long pageStart = baseAddress >> PAGE_SIZE_EXP;
    unsigned long pageEnd = endAddress >> PAGE_SIZE_EXP;

    if ((baseAddress & 0x00000FFF) != 0)
    {
        // The pageStart extends out of the baseAddress, so keep it inside.
        // pageEnd does not need any checks because its right-shift is essentially integer division. Note that pageEnd - 1 is the last page in this range.
        pageStart++;
    }

    // Decumulate the range and add the optimal blocks.
    unsigned long range = pageEnd - pageStart;
    unsigned long current = pageStart;
    while (range > 0)
    {
        unsigned long maxOrder = __builtin_ctz(current);
        unsigned long rangeMaxOrder = 31 - __builtin_clz(range);

        if (maxOrder > rangeMaxOrder)
        {
            maxOrder = rangeMaxOrder;
        }

        append(maxOrder, (current >> maxOrder));

        range -= 1 << maxOrder;
        current += 1 << maxOrder;
    }
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
const MemorySegmentEntry *find_sufficient_tree()
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

        // Do not start the tree at the boot sector.
        if (memorySegments[i].BaseAddress == 0)
        {
            continue;
        }

        return &memorySegments[i];
    }

    // No sufficient location to store the allocation tree.
    panic();
    return (MemorySegmentEntry *)0;
}

void init_allocator()
{
    highestOrder = lowest_exp2(memoryEnd) - PAGE_SIZE_EXP;
    compute_allocation_tree_length();

    const MemorySegmentEntry *segment = find_sufficient_tree();
    AllocationNode *start = (AllocationNode *)(unsigned long)segment->BaseAddress;
    for (int i = 0; i <= highestOrder; i++)
    {
        // Add the offset to each order.
        AllocationNode *startingNode = (allocation_tree[i] += (unsigned long)start);

        // Link to itself to mark it as an empty list.
        startingNode->Next = startingNode;
        startingNode->Previous = startingNode;
    }

    // TODO: Functions that allow allocation at specific locations.
    add_range(segment->BaseAddress + allocationTreeLength, segment->BaseAddress + segment->SegmentLength);
}