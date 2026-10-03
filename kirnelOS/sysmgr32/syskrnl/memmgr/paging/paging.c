#include "page_alloc.h"
#include "paging.h"
#include "utils/flow.h"
#include "utils/memcopy.h"

#define SELF_REFERENCING_POINTER (0xFF7FF000)                                       // The pointer where a self-referencing page directory references itself.
#define VIRT_REFERENCING_POINTER ((PhysicalToVirtualTranslationPage *)(0xFF800000)) // The starting page table of translating physical addresses to virtual addresses.
#define PAGE_REFERENCING_POINTER (0xFFC00000)                                       // The starting pointer of the page table metadata.

/// @brief The behavior for when traversing through a page directory.
typedef enum
{
    /// @brief Gets the page table.
    ForPageTable,

    /// @brief Gets the page.
    ForPage
} TraversalType;

_Bool pagingEnabled = 0;

PageDirectory *self = (PageDirectory *)SELF_REFERENCING_POINTER;

/// @brief Traverses through the given directory using the given virtual address and returns the page associated with it.
/// @param directory The PageDirectory to traverse in.
/// @param virtualAddress The virtual address to use.
/// @param type The type of object to traverse for.
/// @param result The location to load the page location onto.
/// @return True if the mapping exists; otherwise false.
_Bool traverse(PageDirectory *directory, Address virtualAddress, TraversalType type, Page **result)
{
    // Page directory.
    PageDirectoryEntry *_;
    PageTable *table;
    if (!get_page_table(directory, virtualAddress, &_, &table))
    {
        return 0;
    }

    if (type == ForPageTable)
    {
        *result = (Page *)table;
        return 1;
    }

    // Page table.
    PageTableEntry *__;
    if (!get_page(directory, table, virtualAddress, &__, result))
    {
        return 0;
    }

    if (type == ForPage)
    {
        return 1;
    }

    return 0;
}

/// @brief Translates a physical address to the current paging's virtual address if paging is enabled; simply translates the physical address as the virtual address if disabled.
/// @param physical The physical address.
/// @param result The location to translate the virtual address onto.
/// @return True if the physical-to-virtual mapping exists or paging is disabled; otherwise false.
_Bool phys_to_virt(Address physical, void **result)
{
    if (!pagingEnabled)
    {
        *result = physical.Address;
        return 1;
    }

    unsigned short offset = physical.Offset;
    physical.Offset = physical.Page;    // Shift the directory as the page offset.
    physical.Page = physical.Directory; // Shift the page as the address index.
    physical.Directory = 1022;          // Physical to virtual translation page table.

    Page *page;
    if (!traverse(self, physical, ForPage, &page))
    {
        return 0;
    }

    // Virtual page location.
    Address virtualAddress = (*(PhysicalToVirtualTranslationPage *)page)[physical.Offset];
    if (virtualAddress.Offset != 0)
    {
        // Invalid address as it is not page-aligned.
        return 0;
    }

    virtualAddress.Offset = offset;

    *result = virtualAddress.Address;
    return 1;
}

/// @brief Translates a virtual address to its physical address.
/// @param directory The PageDirectory for the virtual address.
/// @param virtual The virtual address.
/// @param result The location to translate the physical address onto.
/// @return True if the virtual-to-physical mapping exists; otherwise false.
_Bool virt_to_phys(PageDirectory *directory, Address virtual, void **result)
{
    unsigned short offset = virtual.Offset;
    virtual.Offset = virtual.Page;    // Shift the directory as the page offset.
    virtual.Page = virtual.Directory; // Shift the page as the address index.
    virtual.Directory = 1023;         // Self-referencing page tables.

    Page *page;
    if (!traverse(directory, virtual, ForPage, &page))
    {
        return 0;
    }

    PageTable *table = (PageTable *)page;
    *result = (void *)(((*table)[virtual.Offset].Page << PAGE_SIZE_EXP) + offset);
    return 1;
}

/// @brief Adds a physical-to-virtual translation map to the given page directory.
/// @param directory The PageDirectory to modify.
/// @param physical The page-aligned physical address.
/// @param virtual The page-aligned virtual address.
void add_translation(PageDirectory *directory, Address virtual, Address physical)
{
    physical.Offset = physical.Page;
    physical.Page = physical.Directory;
    physical.Directory = 1022;

    PageDirectoryEntry *_;
    PageTable *table;
    if (!get_page_table(directory, physical, &_, &table))
    {
        void *location;
        phys_to_virt((Address){.Address = map_page_table(directory, physical, (void *)0xFFFFFFFF, 1, 0, 1)}, &location);

        table = (PageTable *)location;
    }

    Page *page;
    if (!traverse(directory, physical, ForPage, &page))
    {
        void *location;
        phys_to_virt((Address){.Address = map_page(directory, table, physical, (void *)0xFFFFFFFF, 1, 0, 0, 1)}, &location);

        page = (Page *)location;
        one_fill(page, sizeof(Page));
    }

    PhysicalToVirtualTranslationPage *translationPage = (PhysicalToVirtualTranslationPage *)page;
    (*translationPage)[physical.Offset] = virtual;
}

_Bool get_page(PageDirectory *directory, PageTable *table, Address virtualAddress, PageTableEntry **entryResult, Page **result)
{
    PageTableEntry *entry = &((*table)[virtualAddress.Page]);
    if (!entry->Present)
    {
        return 0;
    }

    *entryResult = entry;

    // Handle case for self-querying.
    if (directory == self && pagingEnabled)
    {
        *result = (Page *)(virtualAddress.Raw & 0xFFFFF000);
        return 1;
    }

    Address physicalLocation = {.Raw = entry->Page << PAGE_SIZE_EXP};
    if (!phys_to_virt(physicalLocation, (void **)result))
    {
        // The page is present but there's no virtual location mapped to it.
        panic();
        return 0;
    }

    return 1;
}

_Bool get_page_table(PageDirectory *directory, Address virtualAddress, PageDirectoryEntry **entryResult, PageTable **result)
{
    PageDirectoryEntry *entry = &((*directory)[virtualAddress.Directory]);
    if (!entry->Present)
    {
        return 0;
    }

    *entryResult = entry;

    // Handle case for self-querying.
    if (directory == self && pagingEnabled)
    {
        *result = (PageTable *)(PAGE_REFERENCING_POINTER + (virtualAddress.Directory << PAGE_SIZE_EXP));
        return 1;
    }

    Address physicalLocation = {.Raw = entry->Page << PAGE_SIZE_EXP};
    void *virtualLocation;
    if (!phys_to_virt(physicalLocation, &virtualLocation))
    {
        // The page table is present but there's no virtual location mapped to it.
        panic();
        return 0;
    }

    *result = virtualLocation;
    return 1;
}

void *map_page(PageDirectory *directory, PageTable *table, Address virtualAddress, void *physicalAddress, _Bool canWrite, _Bool userAccessible, _Bool global, _Bool translating)
{
    if (physicalAddress == (void *)0xFFFFFFFF)
    {
        physicalAddress = allocate_strict(1);
    }

    Address physical = {.Address = physicalAddress};

    (*table)[virtualAddress.Page] = (PageTableEntry){
        .Accessed = 0,
        .Dirty = 0,
        .ReadOrWrite = canWrite,
        .UserOrSuper = userAccessible,
        .Global = global,
        .Page = (unsigned long)physicalAddress >> PAGE_SIZE_EXP,
        .Present = 1,
    };

    if (translating)
    {
        // We added a page translation from a virtual to a physical address via map_page_table. Add the other way around.
        physical.Offset = 0x000;
        add_translation(directory, virtualAddress, physical);
    }

    return physicalAddress;
}

PageTable *map_page_table(PageDirectory *directory, Address virtualAddress, PageTable *physicalPageTable, _Bool canWrite, _Bool userAccessible, _Bool translating)
{
    if (physicalPageTable == (void *)0xFFFFFFFF)
    {
        physicalPageTable = allocate_strict(1);
    }

    (*directory)[virtualAddress.Directory] = (PageDirectoryEntry){
        .Accessed = 0,
        .Dirty = 0,
        .ReadOrWrite = canWrite,
        .UserOrSuper = userAccessible,
        .Page = (unsigned long)physicalPageTable >> PAGE_SIZE_EXP,
        .Present = 1,
    };

    if (translating)
    {
        // We need to add this page table as part of the virtual to physical translation.
        virtualAddress.Offset = 0x000;
        virtualAddress.Page = virtualAddress.Directory;
        virtualAddress.Directory = 1023; // Self-referencing page tables.

        PageTable *table;
        Page *page;
        if (!traverse(directory, virtualAddress, ForPageTable, &page))
        {
            // Create the page table if it doesn't exist.
            void *virtualLocation;
            phys_to_virt((Address){.Address = map_page_table(directory, virtualAddress, (PageTable *)0xFFFFFFFF, 1, 0, 1)}, &virtualLocation);

            table = virtualLocation;
        }
        else
        {
            table = (PageTable *)page;
        }

        map_page(directory, table, virtualAddress, physicalPageTable, 1, 0, 0, 1);
    }

    return physicalPageTable;
}

/// @brief Unmaps a page in the given page table at the given address.
/// @param table The PageTable to modify.
/// @param virtualAddress The address whose page to unmap.
/// @param free Determines if the page should be freed.
void unmap_page(PageTable *table, Address virtualAddress, PageFreeType free)
{
    PageTableEntry *entry = &(*table)[virtualAddress.Page];
    entry->Present = 0;

    if (free & FreePages)
    {
        deallocate(1, (void *)(entry->Page << PAGE_SIZE_EXP));
    }
}

/// @brief Unmaps a page table in the given page directory at the given address.
/// @param directory The PageDirectory to modify.
/// @param virtualAddress The address whose page table to unmap.
/// @param free Determines if the page table should be freed.
/// @param translating This PageDirectory translates its own page tables in itself.
void unmap_page_table(PageDirectory *directory, Address virtualAddress, PageFreeType free, _Bool translating)
{
    PageDirectoryEntry *entry = &(*directory)[virtualAddress.Directory];
    entry->Present = 0;

    if (translating)
    {
        // Ditto to map_page_table.
        virtualAddress.Offset = 0x000;
        virtualAddress.Page = virtualAddress.Directory;
        virtualAddress.Directory = 1023;

        PageTable *selfTable;
        PageDirectoryEntry *_;
        if (get_page_table(directory, virtualAddress, &_, &selfTable))
        {
            unmap_page(selfTable, virtualAddress, free);
        }
    }

    if (free & FreePageTables)
    {
        deallocate(1, (void *)(entry->Page << PAGE_SIZE_EXP));
    }
}

void map(PageDirectory *root, Address virtualAddress, void *physicalAddress, unsigned long contiguous, _Bool canWrite, _Bool userAccessible, _Bool global, _Bool translating)
{
    Page *page;
    PageTable *table;
    _Bool newPageTable = 1;

    Page *physicalPage = (Page *)physicalAddress;
    while (contiguous > 0)
    {
        if (newPageTable)
        {
            newPageTable = 0;

            PageDirectoryEntry *_;
            if (!get_page_table(root, virtualAddress, &_, &table))
            {
                void *location;
                phys_to_virt((Address){.Address = (void *)map_page_table(root, virtualAddress, (PageTable *)0xFFFFFFFF, canWrite, userAccessible, translating)}, &location);

                table = (PageTable *)location;
            }
            else
            {
                table = (PageTable *)page;
            }
        }

        map_page(root, table, virtualAddress, (void *)physicalPage, canWrite, userAccessible, global, translating);

        virtualAddress.Page++;
        if (virtualAddress.Page == 0)
        {
            virtualAddress.Directory++;
            newPageTable = 1;
        }

        physicalPage++;
        contiguous--;
    }
}

void unmap(PageDirectory *root, Address virtualAddress, unsigned long pages, PageFreeType free, _Bool translating)
{
    PageTable *table;
    _Bool removePageTable = virtualAddress.Page == 0;
    _Bool newPageTable = 1;

    while (pages > 0)
    {
        if (newPageTable)
        {
            newPageTable = 0;
            PageDirectoryEntry *_;
            if (!get_page_table(root, virtualAddress, &_, &table))
            {
                // Skip this page table.
                if (pages <= NUM_ENTRIES)
                {
                    // Avoid integer overflow issues.
                    return;
                }
                else
                {
                    pages -= NUM_ENTRIES;
                }
            }
        }

        unmap_page(table, virtualAddress, free);

        virtualAddress.Page++;
        if (virtualAddress.Page == 0)
        {
            if (removePageTable)
            {
                unmap_page_table(root, virtualAddress, free, translating);
            }

            virtualAddress.Directory++;
            removePageTable = 1;
            newPageTable = 1;
        }

        pages--;
    }
}

PageDirectory *create_directory(_Bool translating)
{
    PageDirectory *directory = allocate_strict(1);

    void *virtualLocation;
    phys_to_virt((Address){.Address = directory}, &virtualLocation);
    PageDirectory *virtualDirectory = (PageDirectory *)virtualLocation;

    if (translating)
    {
        phys_to_virt((Address){.Address = map_page_table(virtualDirectory, (Address){.Raw = SELF_REFERENCING_POINTER}, (PageTable *)0xFFFFFFFF, 1, 0, 1)}, &virtualLocation);

        PageTable *page = (PageTable *)virtualLocation;
        map_page(virtualDirectory, page, (Address){.Raw = SELF_REFERENCING_POINTER}, directory, 1, 0, 0, 1);
    }

    return directory;
}