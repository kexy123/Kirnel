#include "page_alloc.h"
#include "paging.h"
#include "utils/flow.h"
#include "vga/out.h"

#define SELF_REFERENCING_POINTER ((PageDirectory *)(0xFF7FF000))                    // The pointer where a self-referencing page directory references itself.
#define VIRT_REFERENCING_POINTER ((PhysicalToVirtualTranslationPage *)(0xFF800000)) // The starting page table of translating physical addresses to virtual addresses.
#define PAGE_REFERENCING_POINTER (0xFFC00000)                                       // The starting pointer of the page table metadata.

_Bool pagingEnabled = 0;

PageDirectory *self = SELF_REFERENCING_POINTER;

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

    // Page directory.
    PageTable *table;
    PageDirectoryEntry *_;
    if (!get_page_table(self, physical, &_, &table))
    {
        return 0;
    }

    // Page table.
    PhysicalToVirtualTranslationPage *page;
    PageTableEntry *__;
    if (!get_page(self, table, physical, &__, &page))
    {
        return 0;
    }

    // Virtual page location.
    Address virtualAddress = (*page)[physical.Offset];
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

    // Page directory.
    PageTable *table;
    PageDirectoryEntry *_;
    if (!get_page_table(directory, virtual, &_, &table))
    {
        return 0;
    }

    // Page table.
    PageTable *page;
    PageTableEntry *__;
    if (!get_page(directory, table, virtual, &page, &__))
    {
        return 0;
    }

    *result = (void *)(((*page)[offset].Page << PAGE_SIZE_EXP) + offset);
    return 1;
}

/// @brief Gets a page from the given page table and index.
/// @param directory The PageDirectory this page table is in.
/// @param table The PageTable to look in.
/// @param virtualAddress The virtual address corresponding to this traversal.
/// @param entryResult The location to load the table entry onto.
/// @param result The location to load the page location onto. This acknowledges paging.
/// @return True if the page exists; otherwise false. In contradictory cases a kernel panic may occur.
_Bool get_page(PageDirectory *directory, PageTable *table, Address virtualAddress, PageTableEntry **entryResult, Page **result)
{
    PageTableEntry *entry = table[virtualAddress.Page];
    if (!entry->Present)
    {
        return 0;
    }

    *entryResult = entry;

    // Handle case for self-querying.
    if (directory == self && pagingEnabled)
    {
        *result = virtualAddress.Address;
        return 1;
    }

    Address physicalLocation = {.Raw = entry->Page << PAGE_SIZE_EXP};
    if (!phys_to_virt(physicalLocation, result))
    {
        // The page is present but there's no virtual location mapped to it.
        panic();
        return 0;
    }

    return 1;
}

/// @brief Geets a page table from the given page directory and index.
/// @param directory The PageDirectory to look in.
/// @param virtualAddress The virtual address corresponding to this traversal.
/// @param entryResult The location to load the directory entry onto.
/// @param result The location to load the page table location onto. This acknowledges paging.
/// @return True if the page table exists; otherwise false. In contradictory cases a kernel panic may occur.
_Bool get_page_table(PageDirectory *directory, Address virtualAddress, PageDirectoryEntry **entryResult, PageTable **result)
{
    PageDirectoryEntry *entry = directory[virtualAddress.Directory];
    if (!entry->Present)
    {
        return 0;
    }

    *entryResult = entry;

    // Handle case for self-querying.
    if (directory == self && pagingEnabled)
    {
        *result = PAGE_REFERENCING_POINTER + (virtualAddress.Directory << PAGE_SIZE_EXP);
        return 1;
    }

    Address physicalLocation = {.Raw = entry->Page << PAGE_SIZE_EXP};
    if (!phys_to_virt(physicalLocation, result))
    {
        // The page table is present but there's no virtual location mapped to it.
        panic();
        return 0;
    }

    return 1;
}

void map(PageDirectory *root, Page *virtualPage, Page *physicalPage, unsigned long contiguous, _Bool canWrite, _Bool userAccessible, _Bool global, _Bool translating);

void unmap(PageDirectory *root, Page *virtualPage, unsigned long pages, PageFreeType free, _Bool translating);

PageDirectory *create_directory(_Bool translating);