#include "page_alloc.h"
#include "paging.h"
#include "utils/flow.h"

#define SELF_REFERENCING_POINTER ((PageDirectory *)(0xFF7FF000))                    // The pointer where a self-referencing page directory references itself.
#define VIRT_REFERENCING_POINTER ((PhysicalToVirtualTranslationPage *)(0xFF800000)) // The starting page table of translating physical addresses to virtual addresses.
#define PAGE_REFERENCING_POINTER ((PageTable *)(0xFFC00000))                        // The starting pointer of the page table metadata.

_Bool pagingEnabled = 0;

PageDirectory *self = SELF_REFERENCING_POINTER;

/// @brief Converts a given address into its three parts: the directory entry, the page entry, and the byte offset.
/// @param address The address to convert.
/// @param directoryEntry The directory entry of the address.
/// @param pageEntry The page entry of the address.
/// @param offset The byte offset of the address.
void split_address(void *address, unsigned short *directoryEntry, unsigned short *pageEntry, unsigned short *offset)
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

    unsigned long virtualAddress = (*(VIRT_REFERENCING_POINTER + dirEntry))[pageEntry];
    if (virtualAddress == 0)
    {
        return (void *)0;
    }

    return (void *)(virtualAddress + offset);
}

PageTable *get_page_table(PageDirectory *root, unsigned short index, _Bool createIfEmpty, _Bool canWrite, _Bool userAccessible, _Bool global, _Bool translating);

void map(PageDirectory *root, void *virtualPage, Page *physicalPage, unsigned long contiguous, _Bool canWrite, _Bool userAccessible, _Bool global, _Bool translating);

void unmap(PageDirectory *root, void *virtualPage, unsigned long pages, _Bool translating);

PageDirectory *create_directory(_Bool translating);