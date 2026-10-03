#include "page_alloc.h"
#include "mem_segments.h"
#include "memmgr/kernel_virtual_layout.h"
#include "utils/bit.h"
#include "utils/flow.h"
#include "utils/memcopy.h"

#define MAXIMUM_ORDERS (21) // The maximum number of orders in the buddy allocation tree for 32-bit memory.

/// @brief The starting pointers of the allocation tree in terms of order.
static AllocationNode *allocationTree[MAXIMUM_ORDERS];

/// @brief The highest order of the allocation tree. This means that there are orders between 0..highestOrder, and the number of total orders is highestOrder + 1.
unsigned long highestOrder;

unsigned long allocationTreeLength;

const char *treeLocation;

/// @brief Gets an AllocationNode by pointer at the given order and index.
/// @param order The order to go in.
/// @param index The index in the order free list. Note that it is 1-indexed, and the first element is not a usable AllocationNode.
/// @return The pointer to the AllocationNode.
static inline AllocationNode *get_allocation_node(int order, unsigned long index)
{
    return allocationTree[order] + index - 1;
}

/// @brief Gets the 0-based index of the given AllocationNode in the given order.
/// @param order The order the AllocationNode is in.
/// @param node The AllocationNode.
/// @return The index of the AllocationNode.
static inline unsigned long get_index_of_node(int order, AllocationNode *node)
{
    return node - allocationTree[order] - 1;
}

/// @brief Returns the starting address of the page that the given AllocationNode points to.
/// @param order The order the AllocationNode is in.
/// @param node The AllocationNode.
/// @return The starting address of the page.
static inline void *get_page_location(int order, AllocationNode *node)
{
    return (void *)(get_index_of_node(order, node) << order << PAGE_SIZE_EXP);
}

/// @brief Clears and connects the adjacent nodes of the AllocationNode together.
/// @param order The order the AllocationNode is in.
/// @param node The AllocationNode to dissolve.
void dissolve(int order, AllocationNode *node)
{
    AllocationNode *previous = get_allocation_node(order, node->Previous);
    AllocationNode *next = get_allocation_node(order, node->Next);

    // Turn previous <-> node <-> next to previous <-> next.
    previous->Next = node->Next;
    next->Previous = node->Previous;

    // Invalidate the AllocationNode.
    node->Previous = 0;
    node->Next = 0;
}

/// @brief Inserts an AllocationNode at the given index, without performing any cascading merge operations, to the end of the linked list of its order.
/// @param order The order to insert the AllocationNode in.
/// @param index The 0-indexed page that the AllocationNode points to.
/// @return The new AllocationNode.
AllocationNode *append(int order, unsigned long index)
{
    AllocationNode *node = get_allocation_node(order, index + 2);

    AllocationNode *central = get_allocation_node(order, 1);
    AllocationNode *end = get_allocation_node(order, central->Previous);

    // Turn end <-> central to end <-> node <-> central.
    end->Next = index + 2;
    node->Previous = central->Previous;

    central->Previous = index + 2;
    node->Next = 1;

    return node;
}

/// @brief Appends an AllocationNode and performs a cascading merge with its buddies if possible.
/// @param order The order to insert the AllocationNode in.
/// @param index The 0-indexed page that the AllocationNode points to.
void cascade_add(int order, unsigned long index)
{
    while (order < highestOrder)
    {
        // We cascade first before adding the merged block.
        AllocationNode *buddy = get_allocation_node(order, (index ^ 1) + 2);
        if (buddy->Next == 0 && buddy->Previous == 0)
        {
            // There is no buddy at this point so add the new block.
            break;
        }

        dissolve(order, buddy);
        order++;
        index /= 2;
    }

    append(order, index);
}

/// @brief Bisects the AllocationNode and puts its split parts into the lower order.
/// @param order The order the AllocationNode is in.
/// @param node The AllocationNode to split.
/// @return The left AllocationNode.
AllocationNode *split(int order, AllocationNode *node)
{
    unsigned long index = get_index_of_node(order, node);

    AllocationNode *left = append(order - 1, index * 2); // Left child.
    append(order - 1, index * 2 + 1);                    // Right child.

    dissolve(order, node);

    return left;
}

/// @brief Finds and splits a higher-order block down to the block of the desired target.
/// @param orderTarget The order to split down to.
/// @return The AllocationNode that has been created from the split.
AllocationNode *cascade_split(int orderTarget)
{
    AllocationNode *start, *candidate;
    int order = orderTarget;
    while (1)
    {
        // Find a higher AllocationNode.
        if (order > highestOrder)
        {
            panic();
        }

        start = get_allocation_node(order, 1);
        candidate = get_allocation_node(order, start->Next);
        if (start != candidate)
        {
            break;
        }

        order++;
    }

    // Split down to orderTarget.
    while (order > orderTarget)
    {
        candidate = split(order, candidate);
        order--;
    }

    return candidate;
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

        if (current == 0) // 0 is aligned to the highest possible order.
        {
            maxOrder = rangeMaxOrder;
        }

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
        allocationTree[highestOrder - i] = (AllocationNode *)allocationTreeLength;
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

void *allocate_strict(unsigned long numPages)
{
    unsigned long order = lowest_exp2(numPages);

    AllocationNode *candidate = cascade_split(order);
    void *location = get_page_location(order, candidate);

    dissolve(order, candidate);
    zero_fill(location, PAGE_SIZE << order);

    return location;
}

void deallocate(int order, void *address)
{
    if ((unsigned long)address & ((PAGE_SIZE << order) - 1))
    {
        // The address is not aligned to pages.
        return;
    }

    unsigned long page = (unsigned long)address >> PAGE_SIZE_EXP;
    cascade_add(order, page >> order);
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
        AllocationNode *startingNode = (allocationTree[i] += (unsigned long)start);

        // Link to itself to mark it as an empty list.
        startingNode->Next = 1;
        startingNode->Previous = 1;
    }

    // TODO: Functions that allow allocation at specific locations.
    add_range(segment->BaseAddress + allocationTreeLength, segment->BaseAddress + segment->SegmentLength);

    treeLocation = (char *)segment;
}

void locate_allocator()
{
    // Essentially reinstantiates the necessary variables to preserve page allocation.
    highestOrder = lowest_exp2(memoryEnd) - PAGE_SIZE_EXP;

    compute_allocation_tree_length();
    for (int i = 0; i <= highestOrder; i++)
    {
        // Add the dedicated address of the allocation tree offset.
        allocationTree[i] = (unsigned long)allocationTree[i] + kernel_page_alloc_tree.Address;
    }

    treeLocation = (char *)kernel_page_alloc_tree.Address;
}