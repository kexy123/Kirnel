#include "page_alloc.h"
#include "paging.h"
#include "utils/flow.h"
#include "vga/out.h"

#define SELF_REFERENCING_POINTER ((PageDirectory *)(0xFF7FF000))                    // The pointer where a self-referencing page directory references itself.
#define VIRT_REFERENCING_POINTER ((PhysicalToVirtualTranslationPage *)(0xFF800000)) // The starting page table of translating physical addresses to virtual addresses.
#define PAGE_REFERENCING_POINTER (0xFFC00000)                                       // The starting pointer of the page table metadata.

_Bool pagingEnabled = 0;

PageDirectory *self = SELF_REFERENCING_POINTER;

/// @brief Converts a given address into its three parts: the directory entry, the page entry, and the byte offset.
/// @param address The address to convert.
/// @param directoryEntry The directory entry of the address.
/// @param pageEntry The page entry of the address.
/// @param offset The byte offset of the address.
static inline void split_address(void *address, unsigned short *directoryEntry, unsigned short *pageEntry, unsigned short *offset)
{
    unsigned long number = (unsigned long)address;

    *directoryEntry = number >> 22;           // Extract first 10 bits.
    *pageEntry = (number >> 12) & 0x000003FF; // Extract next 10 bits.
    *offset = number & 0x00000FFF;            // Extract last 12 bits.
}

void *virtual_to_physical(void *virtual)
{
    unsigned short dirEntry, pageEntry, offset;
    split_address(virtual, &dirEntry, &pageEntry, &offset);

    PageTableEntry entry = (*(PageTable *)(PAGE_REFERENCING_POINTER + (dirEntry << PAGE_SIZE_EXP)))[pageEntry];
    if (!entry.Present)
    {
        return (void *)0;
    }

    unsigned long physicalPage = entry.Page << PAGE_SIZE_EXP;
    return (void *)(physicalPage + offset);
}

void *physical_to_virtual(void *physical)
{
    if (!pagingEnabled)
    {
        return physical;
    }

    unsigned short dirEntry, pageEntry, offset;
    split_address(physical, &dirEntry, &pageEntry, &offset);

    unsigned long virtualAddress = (unsigned long)(*(VIRT_REFERENCING_POINTER + dirEntry))[pageEntry];
    if (virtualAddress == 0)
    {
        return (void *)0;
    }

    return (void *)(virtualAddress + offset);
}

/// @brief Adds the translation mapping from the physical address to the virtual address in the given page directory.
/// @param root The PageDirectory to modify.
/// @param physicalAddress The physical address to map. Must be page-aligned.
/// @param virtualAddress The virtual address to map to. Must be page-aligned.
void add_translation(PageDirectory *root, void *physicalAddress, void *virtualAddress)
{
    unsigned short dirEntry, pageEntry, _;
    split_address(physicalAddress, &dirEntry, &pageEntry, &_);

    if (pagingEnabled && root == self)
    {
        (*(VIRT_REFERENCING_POINTER + dirEntry))[pageEntry] = virtualAddress;
    }

    PhysicalToVirtualTranslationPage *virtualReferencingPage = (PhysicalToVirtualTranslationPage *)get_page(root, VIRT_REFERENCING_POINTER + dirEntry, NullCoalesce, 0, 1, 0, 0, 1);
    (*virtualReferencingPage)[pageEntry] = virtualAddress;
}

PageTable *get_page_table(PageDirectory *root, void *virtualAddress, CreationType create, _Bool canWrite, _Bool userAccessible, _Bool translating)
{
    unsigned short dirEntry, _, __;
    split_address(virtualAddress, &dirEntry, &_, &__);

    PageDirectoryEntry *entry = &(*root)[dirEntry];
    if (create == Create || !entry->Present && create == NullCoalesce)
    {
        Page *newPage = allocate_strict(1);
        *entry = (PageDirectoryEntry){
            .Accessed = 0,
            .Dirty = 0,
            .Page = (unsigned long)newPage >> PAGE_SIZE_EXP,
            .Present = 1,
            .ReadOrWrite = canWrite,
            .UserOrSuper = userAccessible,
        };

        if (translating)
        {
            get_page(root, (PageTable *)(PAGE_REFERENCING_POINTER + (dirEntry << PAGE_SIZE_EXP)), Create, newPage, 1, 0, 0, 1);
        }
    }

    if (!entry->Present)
    {
        return (PageTable *)0;
    }

    if (pagingEnabled && root == self)
    {
        return (PageTable *)(PAGE_REFERENCING_POINTER + (dirEntry << PAGE_SIZE_EXP));
    }

    // Note that we are using a different page directory, so we refer to the physical address of the entry and translate it to the current directory being used for virtual translation.
    // With paging disabled, this is just a regular physical address.
    return physical_to_virtual((void *)(entry->Page << PAGE_SIZE_EXP));
}

Page *get_page(PageDirectory *root, void *virtualAddress, CreationType create, void *physicalAddress, _Bool canWrite, _Bool userAccessible, _Bool global, _Bool translating)
{
    PageTable *table = get_page_table(root, virtualAddress, create != NoCreate ? NullCoalesce : NoCreate, canWrite, userAccessible, translating);
    if (table == 0)
    {
        return (Page *)0;
    }

    unsigned short _, pageEntry, __;
    split_address(virtualAddress, &_, &pageEntry, &__);

    PageTableEntry *entry = &(*table)[pageEntry];
    if (create == Create || !entry->Present && create == NullCoalesce)
    {
        if (physicalAddress == 0)
        {
            physicalAddress = allocate_strict(1);
        }

        *entry = (PageTableEntry){
            .Accessed = 0,
            .Dirty = 0,
            .Global = global,
            .Page = (unsigned long)physicalAddress >> PAGE_SIZE_EXP,
            .Present = 1,
            .ReadOrWrite = canWrite,
            .UserOrSuper = userAccessible,
        };

        if (translating)
        {
            add_translation(root, physicalAddress, virtualAddress);
        }
    }

    if (!entry->Present)
    {
        return (Page *)0;
    }

    if (pagingEnabled && root == self)
    {
        return virtualAddress;
    }

    // Ditto to get_page_table.
    return physical_to_virtual(physicalAddress);
}

void map(PageDirectory *root, Page *virtualPage, Page *physicalPage, unsigned long contiguous, _Bool canWrite, _Bool userAccessible, _Bool global, _Bool translating)
{
    while (contiguous > 0)
    {
        get_page(root, virtualPage, Create, physicalPage, canWrite, userAccessible, global, translating);

        virtualPage++;
        physicalPage++;
        contiguous--;
    }
}

void unmap(PageDirectory *root, void *virtualPage, unsigned long pages, PageFreeType free, _Bool translating);

PageDirectory *create_directory(_Bool translating)
{
    PageDirectory *directory = allocate_strict(1);
    PageDirectory *directoryVirtual = physical_to_virtual(directory);

    if (translating)
    {
        // Reserve page 1023 of page table 1021.
        // Consequently this also generates page tables 1022 (physical to virtual) and 1023 (virtual to physical) for translation.
        get_page(directoryVirtual, SELF_REFERENCING_POINTER, Create, directory, 1, 0, 0, 1);
    }

    return directory;
}