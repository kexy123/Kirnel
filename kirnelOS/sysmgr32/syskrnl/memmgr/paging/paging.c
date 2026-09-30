#include "page_alloc.h"
#include "paging.h"
#include "utils/flow.h"

#define SELF_REFERENCING_POINTER ((PageDirectory *)(0xFF7FF000)) // The pointer where a self-referencing page directory references itself.
#define VIRT_REFERENCING_POINTER ((PageTable *)(0xFF800000))     // The starting page table of translating physical addresses to virtual addresses.
#define PAGE_REFERENCING_POINTER ((PageTable *)(0xFFC00000))     // The starting pointer of the page table metadata.

/// @brief Creation types for when retrieving pages or page tables.
typedef enum
{
    /// @brief The page/page table should be created and override the previously existing one.
    Create,

    /// @brief The page/page table should only be created if it doesn't exist.
    NullCoalesce,

    /// @brief A page/page table should not be created even if it doesn't exist.
    NoCreate
} CreationType;

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

    PageTableEntry entry = (*(PAGE_REFERENCING_POINTER + dirEntry))[pageEntry];
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

    unsigned long virtualAddress = (*(PhysicalToVirtualTranslationPage *)(VIRT_REFERENCING_POINTER + dirEntry))[pageEntry];
    if (virtualAddress == 0)
    {
        return (void *)0;
    }

    return (void *)(virtualAddress + offset);
}

/// @brief Adds the translation mapping from the physical address to the virtual address.
/// @param physicalAddress The physical address to map. Must be page-aligned.
/// @param virtualAddress The virtual address to map to. Must be page-aligned.
void add_translation(void *physicalAddress, void *virtualAddress)
{
    unsigned short dirEntry, pageEntry, _;
    split_address(physicalAddress, &dirEntry, &pageEntry, &_);

    (*(PhysicalToVirtualTranslationPage *)(VIRT_REFERENCING_POINTER + dirEntry))[pageEntry] = virtualAddress;
}

/// @brief Gets/creates the page table from the given directory and virtual address in that directory.
/// @param root The PageDirectory to look in/modify.
/// @param virtualAddress The virtual address in the page directory.
/// @param create The creation type.
/// @param canWrite The page table can be written to; otherwise it is read-only.
/// @param userAccessible The page table can be read user code.
/// @param translating This PageDirectory translates its own virtual addresses to physical address and vice versa.
/// @return The PageTable; 0 if not found and not created.
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
            get_page(root, newPage, PAGE_REFERENCING_POINTER + dirEntry, Create, 1, 0, 0, 1);
        }
    }

    if (!entry->Present)
    {
        return (PageTable *)0;
    }

    if (pagingEnabled && root == self)
    {
        return PAGE_REFERENCING_POINTER + dirEntry;
    }

    // Note that we are using a different page directory, so we refer to the physical address of the entry and translate it to the current directory being used for virtual translation.
    // With paging disabled, this is just a regular physical address.
    return physical_to_virtual(entry->Page << PAGE_SIZE_EXP);
}

/// @brief Gets/creates the page from the given directory and virtual address.
/// @param root The PageDirectory to look in.
/// @param virtualAddress The virtual address in the page directory.
/// @param create The creation type. Note that if the page should be created, a page table may be created if the corresponding page table doesn't exist.
/// @param physicalAddress The physical page-aligned address to map to if the page wasn't found.
/// @param canWrite The page table can be written to; otherwise it is read-only.
/// @param userAccessible The page table can be read user code.
/// @param global The page table should not be discarded when switching page tables.
/// @param translating This PageDirectory translates its own virtual addresses to physical address and vice versa.
/// @return The Page; 0 if not found and not created.
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
            add_translation(physicalAddress, virtualAddress);
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

void map(PageDirectory *root, void *virtualPage, Page *physicalPage, unsigned long contiguous, _Bool canWrite, _Bool userAccessible, _Bool global, _Bool translating);

void unmap(PageDirectory *root, void *virtualPage, unsigned long pages, _Bool translating);

PageDirectory *create_directory(_Bool translating)
{
    PageDirectory *directory = allocate_strict(1);

    if (pagingEnabled)
    {
        // TODO: Convert physical directory address to virtual directory address.
    }

    if (translating)
    {
        get_page_table(directory, 1022, Create, 1, 0, 1); // Reserve page table 1022 for physical to virtual addressing.
        // TOOD: Comment this one out to test if 1022 implicitly creates 1023.
        get_page_table(directory, 1023, NullCoalesce, 1, 0, 1); // Reserve page table 1023 for virtual to physical addressing.

        get_page(directory, SELF_REFERENCING_POINTER, Create, directory, 1, 0, 0, 1); // Reserve page 1023 of page table 1021.
    }

    return directory;
}