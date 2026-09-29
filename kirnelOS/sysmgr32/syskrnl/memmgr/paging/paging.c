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

/// @brief Attempts to get the page table at the given directory and index; 0 if not mapped.
/// @param directory The PageDirectory to look in.
/// @param index The index of the page directory.
/// @return The pointer to the PageTable; 0 if not found.
PageTable *get_page_table(PageDirectory *directory, unsigned short index)
{
    PageDirectoryEntry *entry = &(*directory)[index];
    if (entry->Present)
    {
        return (PageTable *)(entry->Page << PAGE_SIZE_EXP);
    }

    return (PageTable *)0;
}

/// @brief Attempts to unmap and freethe given page; does nothing if already unmapped.
/// @param table The PageTable to look in.
/// @param index The index of the page table to unmap.
void unmap_page(PageTable *table, unsigned short index)
{
    PageTableEntry *entry = &(*table)[index];
    if (!entry->Present)
    {
        return;
    }

    entry->Present = 0;

    deallocate(1, (Page *)(entry->Page << PAGE_SIZE_EXP));
}

/// @brief Attempts to unmap and free the given page table; does nothing if already unmapped.
/// @param directory The PageDirectory to look in.
/// @param index The index of the page directory to unmap.
void unmap_page_table(PageDirectory *directory, unsigned short index)
{
    PageDirectoryEntry *entry = &(*directory)[index];
    if (!entry->Present)
    {
        return;
    }

    PageTable *page = (PageTable *)(entry->Page << PAGE_SIZE_EXP);

    // Unmap all of the pages.
    for (int i = 0; i < MAX_ENTRIES; i++)
    {
        unmap_page(page, i);
    }

    entry->Present = 0;

    deallocate(1, page);
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

void unmap(PageDirectory *root, void *virtualPage, unsigned long pages)
{
    unsigned long virtualAddress = (unsigned long)virtualPage;

    unsigned short directoryEntry = virtualAddress >> 22;            // Extract first 10 bits.
    unsigned short tableEntry = (virtualAddress >> 12) & 0x000003FF; // Extract next 10 bits.

    PageTable *table;

    // Unmap the right hand side of the first page table.
    table = get_page_table(root, directoryEntry);
    if (pages != 0 && table)
    {
        for (unsigned long i = tableEntry; i < MAX_ENTRIES; i++)
        {
            unmap_page(table, i);

            pages--;
            if (pages == 0)
            {
                // We have unmapped all the necessary pages.
                return;
            }
        }
    }

    // Unmap entire page tables if applicable.
    while (pages >= MAX_ENTRIES)
    {
        directoryEntry++;
        unmap_page_table(root, directoryEntry);

        pages -= MAX_ENTRIES;
    }

    // Unmap the left hand side of the last page table.
    directoryEntry++;
    table = get_page_table(root, directoryEntry);
    if (pages != 0 && table)
    {
        for (unsigned long i = 0; i < pages; i++)
        {
            unmap_page(table, i);
        }
    }
}

PageDirectory *create_directory()
{
    return (PageDirectory *)allocate_strict(1);
}