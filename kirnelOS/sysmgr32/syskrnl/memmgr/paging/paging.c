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

void map(PageDirectory *root, Page *virtualPage, Page *physicalPage, unsigned long contiguous, _Bool canWrite, _Bool userAccessible, _Bool global, _Bool translating);

void unmap(PageDirectory *root, Page *virtualPage, unsigned long pages, PageFreeType free, _Bool translating);

PageDirectory *create_directory(_Bool translating);