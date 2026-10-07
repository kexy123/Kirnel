#include "bit.h"
#include "memcopy.h"
#include "memory.h"

/// @brief The length of a block in the heap in bytes.
typedef unsigned long HeapLength;

/// @brief The flags of a block in the heap.
typedef union
{
    struct __attribute__((packed))
    {
        /// @brief The heap block is free.
        unsigned Free : 1;
    };

    /// @brief The raw bits of the heap flags.
    unsigned long Raw;
} HeapFlags;

/// @brief The starting metadata of a free heap block.
typedef struct __attribute__((packed)) FreeHeapBlock
{
    /// @brief The length of the free heap block in bytes.
    HeapLength Length;

    /// @brief The previous heap block in its order free list.
    struct FreeHeapBlock *Previous;

    /// @brief The next heap block in its order free list.
    struct FreeHeapBlock *Next;
} FreeHeap;

/// @brief The ending metadata of a heap block.
typedef struct __attribute__((packed))
{
    /// @brief The flags of the heap block
    HeapFlags Flags;

    /// @brief The length of the heap block in bytes.
    HeapLength Length;
} EndHeap;

/// @brief The free list of heap blocks.
FreeHeap *heapTop[__HEAP_NUM_ORDERS];

/// @brief The byte after the last byte of the heap.
static char *heapEnd = (char *)__HEAP_START;

/// @brief Adds a free block to the start of the free list that it should be in.
/// @param location The location of the block. This starts at the metadata point.
void push(void *location)
{
    FreeHeap *freeBlock = (FreeHeap *)location;
    unsigned long index = highest_exp2(freeBlock->Length) - __HEAP_ATOMIC;

    // Connect from heapTop <-> nextBlock to heapTop <-> freeBlock <-> nextBlock.
    FreeHeap *nextBlock = heapTop[index];
    heapTop[index] = freeBlock;

    freeBlock->Next = nextBlock;
    if (nextBlock != 0)
    {
        // Shift the linked list.
        nextBlock->Previous = freeBlock;
    }
}

/// @brief Dissolves a heap block from its free list and connects its neighbors together.
/// @param location The location of the block. This starts at the metadata point.
void dissolve(void *location)
{
    FreeHeap *main = (FreeHeap *)location;
    FreeHeap *previous = main->Previous, *next = main->Next;

    // Disconnect from previous <-> main <-> next to previous <-> next.
    if (previous == 0)
    {
        // This heap block is connected to the heapTop pointer, so refer to that pointer.
        unsigned long index = highest_exp2(main->Length) - __HEAP_ATOMIC;
        heapTop[index] = next;
    }
    else
    {
        previous->Next = next;
    }

    if (next != 0)
    {
        next->Previous = previous;
    }
}

/// @brief Initializes a block of memory from the heap with metadata.
/// @param location The location of the block. This starts at the metadata point.
/// @param size The size of the entire block, including the metadata.
/// @param free The block should be in a free state and should be part of a free list; otherwise it is used. Note that if free, it will not merge with other free blocks.
/// @return The starting location of the usable data.
void *init_block(void *location, unsigned long size, _Bool free)
{
    // Add the length of the heap block at both ends.
    HeapLength *length = (HeapLength *)location;
    *length = size;

    length = (HeapLength *)((char *)location + size - sizeof(HeapLength));
    *length = size;

    HeapFlags *flags = (HeapFlags *)((char *)length - sizeof(HeapFlags));
    flags->Free = free;

    if (free)
    {
        push(location);
    }

    // Return the start of the usable data.
    return (void *)((char *)location + sizeof(HeapLength));
}

/// @brief Attempts to split a heap block into two parts to remove internal fragmentation. If the partition can cause a bad heap block, it will not be split. The left partition is the one that accomodates the given size.
/// @param location The block to partition. This starts at the metadata point.
/// @param size The number of useable bytes to keep when partitioning.
void try_partition(void *location, unsigned long size)
{
    HeapLength length = *(HeapLength *)location;

    HeapLength partitionLength = size + __HEAP_HEADER_SIZE;
    HeapLength remainder = length - partitionLength;
    if (remainder < __HEAP_SMALLEST)
    {
        // The remainder cannot be used to initialize a free block.
        return;
    }

    init_block(location, partitionLength, 0);
    init_block(location + partitionLength, remainder, 1);
}

/// @brief Expands the heap to accomodate a block of the given size and returns the block in a used state.
/// @param size The number of bytes to allocate.
/// @return The starting usable byte of the block.
void *expand(unsigned long size)
{
    char *freeBlock = heapEnd;
    unsigned long blockSize = __HEAP_HEADER_SIZE + size;

    heapEnd += blockSize;
    if (heapEnd > (char *)__HEAP_END)
    {
        // TODO: Abort process.
    }

    return init_block(freeBlock, blockSize, 0);
}

void *malloc(unsigned long size)
{
    for (unsigned long orderIndex = lowest_exp2(size) - __HEAP_ATOMIC; orderIndex < __HEAP_NUM_ORDERS; orderIndex++)
    {
        char *location = (char *)heapTop[orderIndex];
        if (location == 0)
        {
            continue;
        }

        try_partition((void *)location, size);
        return (void *)(location + sizeof(HeapLength));
    }

    return expand(size);
}

void free(void *object)
{
    // Pointer arithmetic :')

    char *meta = (char *)object - sizeof(HeapLength); // Refer to the metadata of the object.
    HeapLength *length = (HeapLength *)meta;

    HeapLength totalLength = *length;
    void *freeLocation = (void *)meta;

    // Try and merge the left neighbour.
    EndHeap *previousEnd = (EndHeap *)(meta - sizeof(EndHeap));
    if (previousEnd >= (EndHeap *)__HEAP_START && previousEnd->Flags.Free)
    {
        // Merge this block and move the ending location of where to free the entire merged block.
        freeLocation = (void *)(meta - previousEnd->Length);
        dissolve(freeLocation);

        totalLength += previousEnd->Length;
    }

    // Try and merge the right neighbour.
    HeapLength *nextLength = (HeapLength *)(meta + *length);
    if (nextLength < (HeapLength *)heapEnd)
    {
        EndHeap *nextEnd = (EndHeap *)((char *)nextLength + *nextLength - sizeof(EndHeap));
        if (nextEnd->Flags.Free)
        {
            // Merge this block.
            dissolve((void *)nextLength);
            totalLength += *nextLength;
        }
    }
    else
    {
        // Merge with the large heap space at the end of the heapEnd.
        heapEnd = (char *)freeLocation;
        return;
    }

    // Reinstantiate the new merged block.
    init_block(freeLocation, totalLength, 1);
}