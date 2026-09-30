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

/// @brief Creation behavior types for when retrieving pages or page tables.
typedef enum
{
    /// @brief The page/page table should be created and override the previously existing one.
    Create,

    /// @brief The page/page table should only be created if it doesn't exist.
    NullCoalesce,

    /// @brief A page/page table should not be created even if it doesn't exist.
    NoCreate
} CreationType;

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

/// @brief A fixed array of page table entries that point to physical pages in memory.
typedef PageTableEntry PageTable[NUM_ENTRIES];

/// @brief A fixed array of physical addresses to PageTables.
typedef PageDirectoryEntry PageDirectory[NUM_ENTRIES];

/// @brief A physical page.
typedef unsigned char Page[PAGE_SIZE];

/// @brief A special page that is used to translate a physical address to the most-recently mapped virtual address.
typedef Page* PhysicalToVirtualTranslationPage[NUM_ENTRIES];

/// @brief Is set if paging is enabled.
extern _Bool pagingEnabled;

/// @brief The location of the self-referencing page directory in virtual memory if this page directory is being used.
extern PageDirectory *self;

/// @brief Converts a virtual address into its physical address; 0 if not found.
/// @param virtual The virtual address to convert.
/// @return The physical address.
void *virtual_to_physical(void *virtual);

/// @brief Converts a physical address into its virtual address; 0 if not found. Note that physical to virtual addresses are a one-to-many relationship, so only the most-recently mapped virtual address to this physical address is returned. If paging is not enabled, simply returns the physical address.
/// @param physical The physical address to convert.
/// @return The virtual address.
void *physical_to_virtual(void *physical);

/// @brief Gets/creates the page table from the given directory and virtual address in that directory.
/// @param root The PageDirectory to look in/modify.
/// @param virtualAddress The virtual address in the page directory.
/// @param create The creation type.
/// @param canWrite The page table can be written to; otherwise it is read-only.
/// @param userAccessible The page table can be read user code.
/// @param translating This PageDirectory translates its own virtual addresses to physical address and vice versa.
/// @return The PageTable; 0 if not found and not created.
PageTable *get_page_table(PageDirectory *root, void *virtualAddress, CreationType create, _Bool canWrite, _Bool userAccessible, _Bool translating);

/// @brief Gets/creates the page from the given directory and virtual address.
/// @param root The PageDirectory to look in.
/// @param virtualAddress The virtual address in the page directory.
/// @param create The creation type. Note that if the page can be created, a page table may be created if the corresponding page table doesn't exist.
/// @param physicalAddress The physical page-aligned address to map to if the page wasn't found. If 0, allocates a page instead.
/// @param canWrite The page table can be written to; otherwise it is read-only.
/// @param userAccessible The page table can be read user code.
/// @param global The page table should not be discarded when switching page tables.
/// @param translating This PageDirectory translates its own virtual addresses to physical address and vice versa.
/// @return The Page; 0 if not found and not created.
Page *get_page(PageDirectory *root, void *virtualAddress, CreationType create, void *physicalAddress, _Bool canWrite, _Bool userAccessible, _Bool global, _Bool translating);

/// @brief Maps one-to-one the physical page to the virtual page by the given number of contiguous pages.
/// @param root The PageDirectory to modify.
/// @param virtualPage The starting virtual page-aligned address.
/// @param physicalPage The starting physical page-aligned address.
/// @param contiguous The number of physical contiguous pages to map.
/// @param canWrite The pages can be written to; otherwise it is read-only.
/// @param userAccessible The pages can be accessed by the user; otherwise it is only accessible to the supervisor.
/// @param global The pages are global regardless of when switching between page directories.
/// @param translating This PageDirectory translates its own virtual addresses to physical address and vice versa.
void map(PageDirectory *root, Page *virtualPage, Page *physicalPage, unsigned long contiguous, _Bool canWrite, _Bool userAccessible, _Bool global, _Bool translating);

/// @brief Unmaps and frees the given number at pages by a starting virtual page address.
/// @param root The PageDirectory to modify.
/// @param virtualPage The starting virtual page-aligned address.
/// @param pages The number of pages to unmap.
/// @param free The page freeing behavior.
/// @param translating This PageDirectory translates its own virtual addresses to physical address and vice versa.
void unmap(PageDirectory *root, Page *virtualPage, unsigned long pages, PageFreeType free, _Bool translating);

/// @brief Creates an empty PageDirectory.
/// @param selfReferential The page directory should be self-referential.
/// @return The created PageDirectory.
PageDirectory *create_directory(_Bool selfReferential);

/// @brief Enables paging to the given PageDirectory.
/// @param pageDirectory The PageDirectory to use.
void enable_paging(PageDirectory *pageDirectory);