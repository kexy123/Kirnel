#include "page_alloc.h"
#include "paging.h"
#include "memmgr/kernel_virtual_layout.h"
#include "memory/memcopy.h"

/// @brief The structure of the virtual memory operations buffer page table.
typedef struct
{
    /// @brief The buffer that points to its own page table.
    PageTable self;

    /// @brief All the usable buffers, extensible for more than one logical CPU processor.
    Page buffers[1023];
} VirtualMemoryOperations;

#define VIRT_MEMOP_BUFFER ((VirtualMemoryOperations *)(0xFFC00000)) // The starting pointer of the virtual memory operations buffer.

/// @brief The behavior for when traversing through a page directory.
typedef enum
{
    /// @brief Gets the page table.
    ForPageTable,

    /// @brief Gets the page.
    ForPage
} TraversalType;

_Bool pagingEnabled = 0;
PageDirectory *selfMemorySpace = (PageDirectory *)KERNEL_NULL_PTR;

// TODO: Change this if we are using more than one CPU processor. Note that it cannot be 0.
unsigned short cpuBufferIndex;
Page *cpuBuffer;

void configure_mem_buffer(unsigned short bufferIndex)
{
    cpuBufferIndex = bufferIndex + 1;
    cpuBuffer = &(VIRT_MEMOP_BUFFER->buffers[bufferIndex]);
}

/// @brief Loads the given physicalPage onto the assigned virtual memory operations buffer for this CPU. Paging must be enabled.
/// @param physicalPage The physical page to load.
void load(void *physicalPage)
{
    if (!pagingEnabled)
    {
        // Paging is not enabled; simply set the buffer to the location.
        cpuBuffer = physicalPage;
        return;
    }

    PageTable *dir = &VIRT_MEMOP_BUFFER->self;
    (*dir)[cpuBufferIndex] = (PageTableEntry){
        .Page = (unsigned long)physicalPage >> PAGE_SIZE_EXP,
        .Present = 1,
        .ReadOrWrite = 1,
        .UserOrSuper = 0,
    };

    invalidate_page(dir + cpuBufferIndex);
}

/// @brief Sets up the given page directory to be able to perform virtual memory operations on itself.
/// @param directory The physical address of the page directory to bootstrap.
void bootstrap(PageDirectory *directory)
{
    // Bootstrap the page table into the directory.
        load(directory);
    PageDirectory *dirLocation = (PageDirectory *)cpuBuffer;

    unsigned long _;
    PageTable *virtMemOpTable = allocate_strict(1, &_);

    (*dirLocation)[1023] = (PageDirectoryEntry){
        .Page = (unsigned long)virtMemOpTable >> PAGE_SIZE_EXP,
        .Present = 1,
        .ReadOrWrite = 1,
        .UserOrSuper = 0,
    };

    // Bootstrap the page table into itself.
        load(virtMemOpTable);
    PageTable *memOpLocation = (PageTable *)cpuBuffer;

    (*memOpLocation)[0] = (PageTableEntry){
        .Page = (unsigned long)virtMemOpTable >> PAGE_SIZE_EXP,
        .Present = 1,
        .ReadOrWrite = 1,
        .UserOrSuper = 0,
    };
}

PageDirectory *create_directory()
{
    unsigned long _;
    PageDirectory *directory = allocate_strict(1, &_);

    bootstrap(directory);

    return directory;
}