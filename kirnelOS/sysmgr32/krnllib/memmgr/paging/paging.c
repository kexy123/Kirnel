#include "page_alloc.h"
#include "paging.h"
#include "memmgr/kernel_virtual_layout.h"
#include "memory/memcopy.h"

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

_Bool virt_to_phys(Address virtual, void **result)
{
    if (!pagingEnabled)
    {
        *result = virtual.Address;
        return 1;
    }

    unsigned short offset = virtual.Offset;
    virtual.Offset = virtual.Page;    // Shift the directory as the page offset.
    virtual.Page = virtual.Directory; // Shift the page as the address index.
    virtual.Directory = 1023;         // Self-referencing page tables.

    Page *page;
    if (!traverse(self, virtual, ForPage, &page))
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
        void *location = (void *)map_page_table(directory, physical, (void *)0xFFFFFFFF, 1, 0);
        if (pagingEnabled)
        {
            map(self, kernelPagingBuffer, (void *)location, 1, 1, 0, 0);
            location = kernelPagingBuffer.Address;
        }

        table = (PageTable *)location;
    }

    Page *page;
    if (!traverse(directory, physical, ForPage, &page))
    {
        void *location = (void *)map_page(directory, table, physical, (void *)0xFFFFFFFF, 1, 0, 0);
        if (pagingEnabled)
        {
            map(self, kernelPagingBuffer, (void *)location, 1, 1, 0, 0);
            location = kernelPagingBuffer.Address;
        }

        page = (Page *)location;
        one_fill(page, sizeof(Page));
    }

    PhysicalToVirtualTranslationPage *translationPage = (PhysicalToVirtualTranslationPage *)page;
    (*translationPage)[physical.Offset] = virtual;
}

_Bool get_page(PageDirectory *directory, PageTable *table, Address virtualAddress, PageTableEntry **entryResult, Page **result)
{
    PageTable *virtual = table;
    if (pagingEnabled && directory != self)
    {
        map(self, kernelPagingBuffer, (void *)table, 1, 1, 0, 0);
        virtual = (PageTable *)kernelPagingBuffer.Address;
    }

    PageTableEntry *entry = &((*virtual)[virtualAddress.Page]);
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

    *result = (Page *)(entry->Page << PAGE_SIZE_EXP);
    return 1;
}

_Bool get_page_table(PageDirectory *directory, Address virtualAddress, PageDirectoryEntry **entryResult, PageTable **result)
{
    PageDirectory *virtual = directory;
    if (pagingEnabled && directory != self)
    {
        map(self, kernelPagingBuffer, (void *)directory, 1, 1, 0, 0);
        virtual = (PageDirectory *)kernelPagingBuffer.Address;
    }

    PageDirectoryEntry *entry = &((*virtual)[virtualAddress.Directory]);
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

    *result = (PageTable *)(entry->Page << PAGE_SIZE_EXP);
    return 1;
}

void *map_page(PageDirectory *directory, PageTable *table, Address virtualAddress, void *physicalAddress, _Bool canWrite, _Bool userAccessible, _Bool global)
{
    if (physicalAddress == (void *)0xFFFFFFFF)
    {
        unsigned long _;
        physicalAddress = allocate_strict(1, &_);
    }

    Address physical = {.Address = physicalAddress};

    PageTable *virtual = table;
    if (pagingEnabled && directory != self)
    {
        map(self, kernelPagingBuffer, (void *)table, 1, 1, 0, 0);
        virtual = (PageTable *)kernelPagingBuffer.Address;
    }

    (*virtual)[virtualAddress.Page] = (PageTableEntry){
        .Accessed = 0,
        .Dirty = 0,
        .ReadOrWrite = canWrite,
        .UserOrSuper = userAccessible,
        .Global = global,
        .Page = (unsigned long)physicalAddress >> PAGE_SIZE_EXP,
        .Present = 1,
    };

    if (pagingEnabled && directory == self)
    {
        invalidate_page(virtualAddress.Address);
    }

    // We added a page translation from a virtual to a physical address via map_page_table. Add the other way around.
    physical.Offset = 0x000;
    add_translation(directory, virtualAddress, physical);

    return physicalAddress;
}

PageTable *map_page_table(PageDirectory *directory, Address virtualAddress, PageTable *physicalPageTable, _Bool canWrite, _Bool userAccessible)
{
    if (physicalPageTable == (void *)0xFFFFFFFF)
    {
        unsigned long _;
        physicalPageTable = allocate_strict(1, &_);
    }

    PageDirectory *virtual = directory;
    if (pagingEnabled && directory != self)
    {
        map(self, kernelPagingBuffer, (void *)directory, 1, 1, 0, 0);
        virtual = (PageDirectory *)kernelPagingBuffer.Address;
    }

    (*virtual)[virtualAddress.Directory] = (PageDirectoryEntry){
        .Accessed = 0,
        .Dirty = 0,
        .ReadOrWrite = canWrite,
        .UserOrSuper = userAccessible,
        .Page = (unsigned long)physicalPageTable >> PAGE_SIZE_EXP,
        .Present = 1,
    };

    // We need to add this page table as part of the virtual to physical translation.
    virtualAddress.Offset = 0x000;
    virtualAddress.Page = virtualAddress.Directory;
    virtualAddress.Directory = 1023; // Self-referencing page tables.

    Page *page;
    void *location;
    if (!traverse(directory, virtualAddress, ForPageTable, &page))
    {
        // Create the page table if it doesn't exist.
        location = map_page_table(directory, virtualAddress, (PageTable *)0xFFFFFFFF, 1, 0);
    }
    else
    {
        location = (void *)page;
    }

    if (pagingEnabled && directory == self)
    {
        phys_to_virt((Address){.Address = location}, &location);
    }

    map_page(directory, (PageTable *)location, virtualAddress, physicalPageTable, 1, 0, 0);

    return physicalPageTable;
}

/// @brief Unmaps a page in the given page table at the given address.
/// @param directory The PageDirectory this table is in.
/// @param table The PageTable to modify.
/// @param virtualAddress The address whose page to unmap.
/// @param free Determines if the page should be freed.
void unmap_page(PageDirectory *directory, PageTable *table, Address virtualAddress, PageFreeType free)
{
    PageTable *virtual = table;
    if (pagingEnabled && directory != self)
    {
        map(self, kernelPagingBuffer, (void *)table, 1, 1, 0, 0);
        virtual = (PageTable *)kernelPagingBuffer.Address;
    }

    PageTableEntry *entry = &(*virtual)[virtualAddress.Page];
    entry->Present = 0;

    if (pagingEnabled && directory == self)
    {
        invalidate_page(virtualAddress.Address);
    }

    if (free & FreePages)
    {
        deallocate(1, (void *)(entry->Page << PAGE_SIZE_EXP));
    }
}

/// @brief Unmaps a page table in the given page directory at the given address.
/// @param directory The PageDirectory to modify.
/// @param virtualAddress The address whose page table to unmap.
/// @param free Determines if the page table should be freed.
void unmap_page_table(PageDirectory *directory, Address virtualAddress, PageFreeType free)
{
    PageDirectory *virtual = directory;
    if (pagingEnabled && directory != self)
    {
        map(self, kernelPagingBuffer, (void *)directory, 1, 1, 0, 0);
        virtual = (PageDirectory *)kernelPagingBuffer.Address;
    }

    PageDirectoryEntry *entry = &(*virtual)[virtualAddress.Directory];
    entry->Present = 0;

    // Ditto to map_page_table.
    virtualAddress.Offset = 0x000;
    virtualAddress.Page = virtualAddress.Directory;
    virtualAddress.Directory = 1023;

    PageTable *selfTable;
    Page *page;
    if (traverse(directory, virtualAddress, ForPageTable, &page))
    {
        selfTable = (PageTable *)page;
        unmap_page(directory, selfTable, virtualAddress, free);
    }

    if (free & FreePageTables)
    {
        deallocate(1, (void *)(entry->Page << PAGE_SIZE_EXP));
    }
}

void map(PageDirectory *root, Address virtualAddress, void *physicalAddress, unsigned long contiguous, _Bool canWrite, _Bool userAccessible, _Bool global)
{
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
                void *location = (void *)map_page_table(root, virtualAddress, (PageTable *)0xFFFFFFFF, canWrite, userAccessible);
                if (pagingEnabled && root == self)
                {
                    phys_to_virt((Address){.Address = location}, &location);
                }

                table = (PageTable *)location;
            }
        }

        map_page(root, table, virtualAddress, (void *)physicalPage, canWrite, userAccessible, global);

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

void map_to_free(PageDirectory *root, Address virtualAddress, unsigned long pages, _Bool canWrite, _Bool userAccessible, _Bool global)
{
    while (pages > 0)
    {
        unsigned long oldPages = pages;
        void *location = try_allocate(pages, &pages);

        map(root, virtualAddress, location, oldPages - pages, canWrite, userAccessible, global);

        virtualAddress.Address += (oldPages - pages) << PAGE_SIZE_EXP;
    }
}

void unmap(PageDirectory *root, Address virtualAddress, unsigned long pages, PageFreeType free)
{
    PageTable *table;
    _Bool removePageTable = virtualAddress.Page == 0;
    _Bool newPageTable = 1;

    while (pages > 0)
    {
        PageDirectoryEntry *_;
        if (newPageTable && !get_page_table(root, virtualAddress, &_, &table))
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
                continue;
            }
        }

        newPageTable = 0;
        unmap_page(root, table, virtualAddress, free);

        virtualAddress.Page++;
        if (virtualAddress.Page == 0)
        {
            if (removePageTable)
            {
                unmap_page_table(root, virtualAddress, free);
            }

            virtualAddress.Directory++;
            removePageTable = 1;
            newPageTable = 1;
        }

        pages--;
    }
}

/// @brief Adds the kernel space as a protected region in the given directory.
/// @param directory The directory to link the kernel space onto.
void add_kernel_to_directory(PageDirectory *directory)
{
    Address address = {.Directory = 768};
    for (unsigned long pageNum = 768; pageNum < 1022; pageNum++)
    {
        PageDirectoryEntry *entry;
        PageTable *_;

        if (get_page_table(self, address, &entry, &_))
        {
            PageTable *table = (PageTable *)(entry->Page << PAGE_SIZE_EXP);
            map_page_table(directory, address, table, 0, 1);
        }

        address.Directory++;
    }
}

PageDirectory *create_directory()
{
    unsigned long _;
    PageDirectory *directory = allocate_strict(1, &_);

    PageTable *page = map_page_table(directory, (Address){.Raw = SELF_REFERENCING_POINTER}, (PageTable *)0xFFFFFFFF, 1, 0);
    map_page(directory, page, (Address){.Raw = SELF_REFERENCING_POINTER}, directory, 1, 0, 0);

    if (pagingEnabled)
    {
        add_kernel_to_directory(directory);
    }

    return directory;
}