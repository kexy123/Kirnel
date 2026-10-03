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

/// @brief A special page that is used to translate a physical address to the most-recently mapped virtual address.
typedef Address PhysicalToVirtualTranslationPage[NUM_ENTRIES];

/// @brief Is set if paging is enabled.
extern _Bool pagingEnabled;

/// @brief The location of the self-referencing page directory in virtual memory if this page directory is being used.
extern PageDirectory *self;

/// @brief Geets a page table from the given page directory and index.
/// @param directory The PageDirectory to look in.
/// @param virtualAddress The virtual address corresponding to this traversal.
/// @param entryResult The location to load the directory entry onto.
/// @param result The location to load the page table location onto. This acknowledges paging.
/// @return True if the page table exists; otherwise false. In contradictory cases a kernel panic may occur.
_Bool get_page_table(PageDirectory *directory, Address virtualAddress, PageDirectoryEntry **entryResult, PageTable **result);

/// @brief Gets a page from the given page table and index.
/// @param directory The PageDirectory this page table is in.
/// @param table The PageTable to look in.
/// @param virtualAddress The virtual address corresponding to this traversal.
/// @param entryResult The location to load the table entry onto.
/// @param result The location to load the page location onto. This acknowledges paging.
/// @return True if the page exists; otherwise false. In contradictory cases a kernel panic may occur.
_Bool get_page(PageDirectory *directory, PageTable *table, Address virtualAddress, PageTableEntry **entryResult, Page **result);

/// @brief Maps a page from the page directory at the given page table and address to the given physical address or a free page.
/// @param directory The PageDirectory the page table is in.
/// @param table The PageTable to modify.
/// @param virtualAddress The virtual address to map from.
/// @param physicalAddress The physical address to map to. 0xFFFFFFFF to map to a free page.
/// @param canWrite The pages can be written to; otherwise it is read-only.
/// @param userAccessible The pages can be accessed by the user; otherwise it is only accessible to the supervisor.
/// @param global The pages are global regardless of when switching between page directories.
/// @param translating This PageDirectory translates its own virtual addresses to physical address and vice versa.
/// @return The physical address that was mapped to.
void *map_page(PageDirectory *directory, PageTable *table, Address virtualAddress, void *physicalAddress, _Bool canWrite, _Bool userAccessible, _Bool global, _Bool translating);

/// @brief Maps a page table from a page directory to the given page table address or a free page.
/// @param directory The PageDirectory to modify.
/// @param virtualAddress The virtual address to map from.
/// @param physicalPageTable The physical PageTable address to map to. 0xFFFFFFFF to map to a free page.
/// @param canWrite The pages can be written to; otherwise it is read-only.
/// @param userAccessible The pages can be accessed by the user; otherwise it is only accessible to the supervisor.
/// @param translating This PageDirectory translates its own virtual addresses to physical address and vice versa.
/// @return The physical address that was mapped to.
PageTable *map_page_table(PageDirectory *directory, Address virtualAddress, PageTable *physicalPageTable, _Bool canWrite, _Bool userAccessible, _Bool translating);

/// @brief Maps one-to-one the physical page to the virtual page by the given number of contiguous pages.
/// @param root The PageDirectory to modify.
/// @param virtualAddress The starting virtual page-aligned address.
/// @param physicalAddress The starting physical page-aligned address.
/// @param contiguous The number of physical contiguous pages to map.
/// @param canWrite The pages can be written to; otherwise it is read-only.
/// @param userAccessible The pages can be accessed by the user; otherwise it is only accessible to the supervisor.
/// @param global The pages are global regardless of when switching between page directories.
/// @param translating This PageDirectory translates its own virtual addresses to physical address and vice versa.
void map(PageDirectory *root, Address virtualAddress, void *physicalAddress, unsigned long contiguous, _Bool canWrite, _Bool userAccessible, _Bool global, _Bool translating);

/// @brief Unmaps and frees the given number at pages by a starting virtual page address.
/// @param root The PageDirectory to modify.
/// @param virtualPage The starting virtual page-aligned address.
/// @param pages The number of pages to unmap.
/// @param free The page freeing behavior.
/// @param translating This PageDirectory translates its own virtual addresses to physical address and vice versa.
void unmap(PageDirectory *root, Address virtualAddress, unsigned long pages, PageFreeType free, _Bool translating);

/// @brief Creates an empty PageDirectory.
/// @param selfReferential The page directory should be self-referential.
/// @return The created PageDirectory.
PageDirectory *create_directory(_Bool selfReferential);

/// @brief Enables paging to the given PageDirectory.
/// @param pageDirectory The PageDirectory to use.
extern void enable_paging(PageDirectory *pageDirectory);

/// @brief Checks if paging is enabled and sets pagingEnabled.
extern void check_paging();

/// @brief Invalidates a page by a given virtual address. Is used for setting or clearing the presence of pages from within the self page directory.
/// @param virtualAddress The page-aligned virtual address whose page to invalidate.
extern void invalidate_page(void *virtualAddress);