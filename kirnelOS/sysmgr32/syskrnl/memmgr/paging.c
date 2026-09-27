#include "page_alloc.h"
#include "paging.h"
#include "utils/flow.h"

/// @brief Creates a page entry at the given index that points to a physical page-aligned address.
/// @param table The PageTable to modify.
/// @param index The index of the page table. It must be empty.
/// @param pageAddress The page-aligned address to point to.
/// @param canWrite The entry can be written to; otherwise it is read-only.
/// @param userAccessible The entry can be accessed by the user; otherwise it is only accessible to the supervisor.
/// @param global The page entry is global regardless of when switching between page directories.
void create_page_entry(PageTable *table, unsigned short index, Page *pageAddress, _Bool canWrite, _Bool userAccessible, _Bool global)
{
    unsigned long address = ((unsigned long)pageAddress) >> PAGE_SIZE_EXP;

    PageTableEntry *entry = &(*table)[index];
    if (entry->Present && entry->Page != address)
    {
        // The page entry must be empty or it must be pointing to the same pageAddress.
        panic();
        return;
    }

    *entry = (PageTableEntry){
        .Present = 1,
        .ReadOrWrite = canWrite,
        .UserOrSuper = userAccessible,
        .Dirty = 0,
        .Accessed = 0,
        .Global = global,
        .Page = address,
    };
}

/// @brief Tries to create an active directory entry at the given index that points to a new page table.
/// @param directory The PageDirectory to modify.
/// @param index The index of the page directory.
/// @param canWrite The entry can be written to; otherwise it is read-only.
/// @param userAccessible The entry can be accessed by the user; otherwise it is only accessible to the supervisor.
/// @return The created/already existing page table.
PageTable *try_create_directory_entry(PageDirectory *directory, unsigned short index, _Bool canWrite, _Bool userAccessible)
{
    PageDirectoryEntry *entry = &(*directory)[index];
    if (entry->Present)
    {
        // There's already a directory entry here.
        return (PageTable *)(entry->Page << PAGE_SIZE_EXP);
    }

    PageTable *address = allocate_strict(1);
    *entry = (PageDirectoryEntry){
        .Present = 1,
        .ReadOrWrite = canWrite,
        .UserOrSuper = userAccessible,
        .Dirty = 0,
        .Accessed = 0,
        .PageSize = 0,
        .Page = ((unsigned long)address) >> PAGE_SIZE_EXP,
    };

    return (PageTable *)address;
}

void map(PageDirectory *root, void *virtualPage, Page *physicalPage, unsigned long contiguous, _Bool canWrite, _Bool userAccessible, _Bool global)
{
    unsigned long virtualAddress = (unsigned long)virtualPage;

    unsigned short directoryEntry = virtualAddress >> 22;            // Extract first 10 bits.
    unsigned short tableEntry = (virtualAddress >> 12) & 0x000003FF; // Extract next 10 bits.

    // Map the physical page onto the virtual page.
    PageTable *table = try_create_directory_entry(root, directoryEntry, canWrite, userAccessible);
    for (unsigned long current = 0; current < contiguous; current++)
    {
        create_page_entry(table, tableEntry, physicalPage, canWrite, userAccessible, global);

        physicalPage++;
        tableEntry++;
        if (tableEntry >= MAX_ENTRIES)
        {
            // Go to the next page table.
            tableEntry = 0;

            directoryEntry++;
            table = try_create_directory_entry(root, directoryEntry, canWrite, userAccessible);
        }
    }
}

PageDirectory *create_directory()
{
    return (PageDirectory *)allocate_strict(1);
}