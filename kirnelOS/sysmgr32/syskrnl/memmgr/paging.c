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
    if ((*table)[index].Present)
    {
        // The page entry must be empty.
        panic();
        return;
    }

    (*table)[index] = (PageTableEntry){
        .Present = 1,
        .ReadOrWrite = canWrite,
        .UserOrSuper = userAccessible,
        .Dirty = 0,
        .Accessed = 0,
        .Global = global,
        .Page = ((unsigned long)pageAddress) >> PAGE_SIZE_EXP,
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
    if ((*directory)[index].Present)
    {
        // There's already a directory entry here.
        return (PageTable *)((*directory)[index].Page << PAGE_SIZE_EXP);
    }

    PageTable *address = allocate_strict(1);
    (*directory)[index] = (PageDirectoryEntry){
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