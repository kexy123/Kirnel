#pragma once

#include "page_alloc.h"

#define NUM_ENTRIES (1024) // The number of entries in both the PageTable and PageDirectory.

/// @brief Specification of the [page table entry](https://wiki.osdev.org/index.php?title=X86_Paging#Page_Table).
typedef struct __attribute__((packed))
{
    /// @brief The entry is present.
    unsigned Present : 1;

    /// @brief When set, the page can be written to; otherwise it is read-only.
    unsigned ReadOrWrite : 1;

    /// @brief When set, the page may be accessible to the user or the supervisor; otherwise it may only be accessed by the supervisor.
    unsigned UserOrSuper : 1;

    /// @brief When set, the page should have write-through behavior instead of write-back.
    unsigned PageWriteThrough : 1;

    /// @brief When set, the page it points to should not be cached.
    unsigned PageCacheDisable : 1;

    /// @brief Whether this page table entry was accessed by virtual memory translation. It does not get cleared automatically.
    unsigned Accessed : 1;

    /// @brief Whether the page was written to; usually ignored as it is reserved on some CPU models.
    unsigned Dirty : 1;

    /// @brief Determines if the [page attribute table](https://en.wikipedia.org/wiki/Page_attribute_table) is supported, where the PWT and PCD can determine caching behavior.
    unsigned PageAttributeTableSupported : 1;

    /// @brief This page's [translation lookaside buffer](https://en.wikipedia.org/wiki/Translation_lookaside_buffer) entry should not be invalidated when switching between page directories.
    unsigned Global : 1;

    /// @brief Usable bits for the kernel.
    unsigned Available : 3;

    /// @brief The physical page number.
    unsigned Page : 20;
} PageTableEntry;

/// @brief Specification of the [page directory entry](https://wiki.osdev.org/index.php?title=X86_Paging#Page_Directory) for 4 KiB page sizes.
typedef struct __attribute__((packed))
{
    /// @brief The entry is present.
    unsigned Present : 1;

    /// @brief When set, the page table it points to can be written to; otherwise it is read-only.
    unsigned ReadOrWrite : 1;

    /// @brief When set, the page table it points to may be accessible to the user or the supervisor; otherwise it may only be accessed by the supervisor.
    unsigned UserOrSuper : 1;

    /// @brief When set, the page table it points to should have write-through behavior instead of write-back.
    unsigned PageWriteThrough : 1;

    /// @brief When set, the page table it points to should not be cached.
    unsigned PageCacheDisable : 1;

    /// @brief Whether this directory table entry was accessed by virtual memory translation. It does not get cleared automatically.
    unsigned Accessed : 1;

    /// @brief Whether the page table it points to was written to; usually ignored as it is reserved on some CPU models.
    unsigned Dirty : 1;

    /// @brief The page size; should be 0.
    unsigned PageSize : 1;

    /// @brief Usable bits for the kernel.
    unsigned Available : 4;

    /// @brief The physical page number of the page table.
    unsigned Page : 20;
} PageDirectoryEntry;

/// @brief Page freeing behavior types for when unmapping pages and/or page tables in a page directory.
typedef enum
{
    /// @brief No pages are freed.
    NoFree = 0x00,

    /// @brief Frees the pages when unmapping them.
    FreePages = 0x01,

    /// @brief Frees the page tables if possible.
    FreePageTables = 0x02
} PageFreeType;

/// @brief The structure of a virtual address.
typedef union
{
    struct __attribute__((packed))
    {
        /// @brief The byte offset in the page.
        unsigned Offset : 12;

        /// @brief The page index.
        unsigned Page : 10;

        /// @brief The directory index.
        unsigned Directory : 10;
    };

    /// @brief The location pointer.
    void *Address;

    /// @brief The address as a number.
    unsigned long Raw;
} Address;

/// @brief A fixed array of page table entries that point to physical pages in memory.
typedef PageTableEntry PageTable[NUM_ENTRIES];

/// @brief A fixed array of physical addresses to PageTables.
typedef PageDirectoryEntry PageDirectory[NUM_ENTRIES];

/// @brief A physical page.
typedef unsigned char Page[PAGE_SIZE];

/// @brief Is set if paging is enabled.
extern _Bool pagingEnabled;

/// @brief The physical address of the page directory that the kernel is currently under.
extern PageDirectory *selfMemorySpace;

/// @brief Configures the virtual memory operations buffer for this CPU.
/// @param bufferIndex The buffer index to give. It must be unique against other CPUs.
void configure_mem_buffer(unsigned short bufferIndex);

/// @brief Creates an empty PageDirectory.
/// @return The physical address of where the PageDirectory was created.
PageDirectory *create_directory();

/// @brief Enables paging and global paging and switches to the given PageDirectory.
/// @param pageDirectory The PageDirectory to use.
__attribute__((__cdecl__)) extern void enable_paging(PageDirectory *pageDirectory);

/// @brief Checks if paging is enabled and sets pagingEnabled and the selfMemorySpace.
__attribute__((__cdecl__)) extern void check_paging();

/// @brief Invalidates a page by a given virtual address. Is used for setting or clearing the presence of pages from within the selfMemorySpace page directory.
/// @param virtualAddress The page-aligned virtual address whose page to invalidate.
__attribute__((__cdecl__)) extern void invalidate_page(void *virtualAddress);

/// @brief Switches to a given PageDirectory without changing the current paging state.
/// @param PageDirectory The PageDirectory by its physical address to switch to.
__attribute__((__cdecl__)) extern void switch_page(PageDirectory *PageDirectory);
