#pragma once

#include "page_alloc.h"

#define MAX_ENTRIES (1024) // The maximum number of entries in both the PageTable and PageDirectory.

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

    /// @brief The page number.
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

    /// @brief The page number of the page table.
    unsigned Page : 20;
} PageDirectoryEntry;

/// @brief A fixed array of page table entries that point to physical pages in memory.
typedef PageTableEntry PageTable[MAX_ENTRIES];

/// @brief A fixed array of physical addresses to PageTables.
typedef PageDirectoryEntry PageDirectory[MAX_ENTRIES];

/// @brief A physical page.
typedef unsigned char Page[PAGE_SIZE];

/// @brief Is set if paging is enabled.
extern _Bool pagingEnabled;

/// @brief Maps one-to-one the physical page to the virtual page by the given number of contiguous pages.
/// @param root The PageDirectory to modify.
/// @param virtualPage The starting virtual page-aligned address.
/// @param physicalPage The starting physical page-aligned address.
/// @param contiguous The number of physical contiguous pages to map.
/// @param canWrite The entry can be written to; otherwise it is read-only.
/// @param userAccessible The entry can be accessed by the user; otherwise it is only accessible to the supervisor.
/// @param global The page entry is global regardless of when switching between page directories.
/// @param selfReferential The page directory is self-referential and its integrity should be maintained.
void map(PageDirectory *root, void *virtualPage, Page *physicalPage, unsigned long contiguous, _Bool canWrite, _Bool userAccessible, _Bool global, _Bool selfReferential);

/// @brief Unmaps and frees the given number at pages by a starting virtual page address.
/// @param root The PageDirectory to modify.
/// @param virtualPage The the starting virtual page-aligned address.
/// @param pages The number of pages to unmap.
/// @param selfReferential The page directory is self-referential and its integrity should be maintained.
void unmap(PageDirectory *root, void *virtualPage, unsigned long pages, _Bool selfReferential);

/// @brief Creates an empty PageDirectory.
/// @param selfReferential The page directory should be self-referential.
/// @return The created PageDirectory.
PageDirectory *create_directory(_Bool selfReferential);

/// @brief Enables paging to the given PageDirectory.
/// @param pageDirectory The PageDirectory to use.
void enable_paging(PageDirectory *pageDirectory);