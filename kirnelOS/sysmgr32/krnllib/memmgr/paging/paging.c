#include "page_alloc.h"
#include "paging.h"
#include "memmgr/kernel_virtual_layout.h"
#include "memory/memcopy.h"

/// @brief The structure of the virtual memory operations buffer page table.
typedef struct
{
    /// @brief The buffer that points to its own page directory.
    PageDirectory selfDirectory;

    /// @brief The buffer that points to its own page table.
    PageTable selfTable;

    /// @brief All the usable buffers, extensible for more than one logical CPU processor.
    Page buffers[1022];
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

// TODO: Change this if we are using more than one CPU processor.
unsigned short cpuBufferIndex;
Page *cpuBuffer;

void configure_mem_buffer(unsigned short bufferIndex)
{
    cpuBufferIndex = bufferIndex;
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

    PageTable *dir = &VIRT_MEMOP_BUFFER->selfTable;
    (*dir)[cpuBufferIndex + 2] = (PageTableEntry){
        .Page = (unsigned long)physicalPage >> PAGE_SIZE_EXP,
        .Present = 1,
        .ReadOrWrite = 1,
        .UserOrSuper = 0,
    };

    invalidate_page((void *)cpuBuffer);
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

    // Bootstrap the page directory and page table into itself.
    load(virtMemOpTable);
    PageTable *memOpLocation = (PageTable *)cpuBuffer;

    // Page directory.
    (*memOpLocation)[0] = (PageTableEntry){
        .Page = (unsigned long)directory >> PAGE_SIZE_EXP,
        .Present = 1,
        .ReadOrWrite = 1,
        .UserOrSuper = 0,
    };

    // Page table of index 1023.
    (*memOpLocation)[1] = (PageTableEntry){
        .Page = (unsigned long)virtMemOpTable >> PAGE_SIZE_EXP,
        .Present = 1,
        .ReadOrWrite = 1,
        .UserOrSuper = 0,
    };
}

/// @brief Gets the physical page-aligned address of the given page from the loaded page table.
/// @param index The index from the page table to find.
/// @return The physical page-aligned address. If it doesn't exist, it is a null pointer.
void *get_page(unsigned long index)
{
    // Note: this assumes that the necessary page table is loaded into the correct CPU buffer; otherwise it is undefined behavior.
    PageTableEntry entry = (*(PageTable *)cpuBuffer)[index];
    if (!entry.Present)
    {
        return (void *)KERNEL_NULL_PTR;
    }

    return (void *)(entry.Page << PAGE_SIZE_EXP);
}

/// @brief Gets the physical page-aligned address of the given page table from the loaded page directory.
/// @param index The index from the page directory to find.
/// @return The physical page-aligned address. If it doesn't exist, it is a null pointer.
void *get_page_table(unsigned long index)
{
    // Note: this assumes that the necessary page directory is loaded into the correct CPU buffer; otherwise it is undefined behavior.
    PageDirectoryEntry entry = (*(PageDirectory *)cpuBuffer)[index];
    if (!entry.Present)
    {
        return (void *)KERNEL_NULL_PTR;
    }

    return (void *)(entry.Page << PAGE_SIZE_EXP);
}

/// @brief Maps a page by the index to a physicalPage.
/// @param index The index in the loaded page table to map.
/// @param physicalPage The physical page-aligned address to map to.
/// @param canWrite The page can be written to; otherwise it is read-only.
/// @param userAccessible The page can be accessible by the user; otherwise it is only accessible by the supervisor.
/// @param global The page is global regardless of page directory changes.
/// @return The physical page that was mapped to.
Page *map_page(unsigned long index, void *physicalPage, _Bool canWrite, _Bool userAccessible, _Bool global)
{
    // Note: this assumes that the necessary page table is loaded into the correct CPU buffer; otherwise it is undefined behavior.
    PageTable *table = (PageTable *)cpuBuffer;

    (*table)[index] = (PageTableEntry){
        .Page = (unsigned long)physicalPage >> PAGE_SIZE_EXP,
        .Present = 1,
        .ReadOrWrite = canWrite,
        .UserOrSuper = userAccessible,
        .Global = global,
    };

    return (Page *)physicalPage;
}

/// @brief Maps a page table by the index to a physicalPage.
/// @param index The index in the loaded page directory to map.
/// @param physicalPage The physical page-aligned address to map to. Null pointer to allocate a new page.
/// @param canWrite The page can be written to; otherwise it is read-only.
/// @param userAccessible The page can be accessible by the user; otherwise it is only accessible by the supervisor.
/// @return The physical page that was mapped to.
PageTable *map_page_table(unsigned long index, void *physicalPage, _Bool canWrite, _Bool userAccessible)
{
    if (physicalPage == (void *)KERNEL_NULL_PTR)
    {
        unsigned long _;
        physicalPage = allocate_strict(1, &_);
    }

    // Note: this assumes that the necessary page directory is loaded into the correct CPU buffer; otherwise it is undefined behavior.
    PageDirectory *directory = (PageDirectory *)cpuBuffer;

    (*directory)[index] = (PageDirectoryEntry){
        .Page = (unsigned long)physicalPage >> PAGE_SIZE_EXP,
        .Present = 1,
        .ReadOrWrite = canWrite,
        .UserOrSuper = userAccessible,
    };

    return (PageTable *)physicalPage;
}

/// @brief Unmaps a page from a given index.
/// @param index The index in the loaded page table to unmap.
/// @param type Additional instructions for unmapping pages.
void unmap_page(unsigned long index, PageFreeType type)
{
    // Note: this assumes that the necessary page table is loaded into the correct CPU buffer; otherwise it is undefined behavior.
    PageTable *table = (PageTable *)cpuBuffer;
    PageTableEntry *entry = &(*table)[index];

    if (type | FreePages && entry->Present)
    {
        deallocate(1, (void *)(entry->Page << PAGE_SIZE_EXP));
    }

    entry->Present = 0;
}

/// @brief Unmaps a page table from a given index.
/// @param index The index in the loaded page directory to unmap.
/// @param type Additional instructions for unmapping page tables.
void unmap_page_table(unsigned long index, PageFreeType type)
{
    // Note: this assumes that the necessary page directory is loaded into the correct CPU buffer; otherwise it is undefined behavior.
    PageDirectory *table = (PageDirectory *)cpuBuffer;
    PageDirectoryEntry *entry = &(*table)[index];

    if (type | FreePageTables && entry->Present)
    {
        deallocate(1, (void *)(entry->Page << PAGE_SIZE_EXP));
    }

    entry->Present = 0;
}

void *virt_to_phys(PageDirectory *directory, Address virtualAddress)
{
    load(directory);

    // Get the address of the page table.
    PageTable *table = (PageTable *)get_page_table(virtualAddress.Directory);
    load(table);

    // Get the address of the page.
    Page *page = (Page *)get_page(virtualAddress.Page);
    return (void *)((char *)page + virtualAddress.Offset);
}

void map(PageDirectory *directory, Address virtualPageStart, void *physicalPageStart, unsigned long numPages, _Bool canWrite, _Bool userAccessible, _Bool global)
{
    Address physical = (Address){.Address = physicalPageStart};
    physical.Offset = 0;
    virtualPageStart.Offset = 0;

    _Bool updatePageTable = 1;
    while (numPages > 0)
    {
        if (updatePageTable)
        {
            // Go to the next page table.
            load(directory);
            PageTable *pageTable = get_page_table(virtualPageStart.Directory);

            if (pageTable == (PageTable *)KERNEL_NULL_PTR)
            {
                // Create a new page table.
                pageTable = map_page_table(virtualPageStart.Directory, (void *)KERNEL_NULL_PTR, canWrite, userAccessible);
                load(pageTable);
                zero_fill((void *)cpuBuffer, sizeof(PageTable));
            }
            else
            {
                load(pageTable);
            }

            updatePageTable = 0;
        }

        // Map the page.
        map_page(virtualPageStart.Page, physical.Address, canWrite, userAccessible, global);
        if (directory == selfMemorySpace)
        {
            invalidate_page(virtualPageStart.Address);
        }

        virtualPageStart.Raw += PAGE_SIZE;
        physical.Raw += PAGE_SIZE;
        numPages--;
        if (virtualPageStart.Page == 0)
        {
            updatePageTable = 1;
        }
    }
}

void map_to_free(PageDirectory *directory, Address virtualPageStart, unsigned long numPages, _Bool canWrite, _Bool userAccessible, _Bool global)
{
    virtualPageStart.Offset = 0;

    while (numPages > 0)
    {
        unsigned long remainder;
        void *physicalPage = try_allocate(numPages, &remainder);

        map(directory, virtualPageStart, physicalPage, numPages - remainder, canWrite, userAccessible, global);

        virtualPageStart.Raw += (numPages - remainder) << PAGE_SIZE_EXP;
        numPages = remainder;
    }
}

void unmap(PageDirectory *directory, Address virtualPageStart, unsigned long numPages, PageFreeType type)
{
    virtualPageStart.Offset = 0;

    _Bool updatePageTable = 1, canUnmapPageTable = virtualPageStart.Page == 0;
    while (numPages > 0)
    {
        if (updatePageTable)
        {
            // Go to the next page table.
            load(directory);

            PageTable *pageTable = get_page_table(virtualPageStart.Directory);
            if (pageTable == (PageTable *)KERNEL_NULL_PTR)
            {
                if (numPages > NUM_ENTRIES)
                {
                    numPages -= NUM_ENTRIES;
                    virtualPageStart.Directory++;
                    continue;
                }
                return;
            }

            if (canUnmapPageTable)
            {
                // TODO: Invalidate all pages in the page table.
                // Unmap page table.
                unmap_page_table(virtualPageStart.Directory, type);
            }

            load(pageTable);
            updatePageTable = 0;
        }

        // Unmap page.
        unmap_page(virtualPageStart.Page, type);
        if (directory == selfMemorySpace)
        {
            invalidate_page(virtualPageStart.Address);
        }

        virtualPageStart.Raw += PAGE_SIZE;
        numPages--;
        if (virtualPageStart.Page == 0)
        {
            updatePageTable = 1;
            canUnmapPageTable = 1;
        }
    }
}

/// @brief Maps the kernel space, assuming paging is already enabled and the current page directory contains the kernel space.
/// @param directory The physical page directory to map the kernel space onto.
void map_kernel_space(PageDirectory *directory)
{
    load(directory);

    // TODO: Move these magic numbers into constexpr functions.
    // Essentially computes the starting pointers of page table index 768 in the directories (which is 0xC0000000).
    void *kernelSpaceSource = (void *)&(VIRT_MEMOP_BUFFER->selfDirectory[768]);
    void *kernelSpaceDestination = (void *)&((*(PageTable *)cpuBuffer)[768]);

    copy_to(kernelSpaceSource, kernelSpaceDestination, (1023 - 768) * sizeof(PageDirectoryEntry));
}

PageDirectory *create_directory()
{
    unsigned long _;
    PageDirectory *directory = allocate_strict(1, &_);

    bootstrap(directory);
    if (pagingEnabled)
    {
        map_kernel_space(directory);
    }

    return directory;
}