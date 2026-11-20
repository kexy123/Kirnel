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

PageDirectory *selfMemorySpace = (PageDirectory *)0xFFFFFFFF;

PageDirectory *create_directory()
{
    
}