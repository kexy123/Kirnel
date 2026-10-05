#include "master.h"
#include "gdt/master.h"
#include "kernel_virtual_layout.h"
#include "kernel_jump.h"
#include "paging/mem_segments.h"
#include "paging/page_alloc.h"
#include "paging/paging.h"
#include "utils/bit.h"
#include "vga/out.h"

void init_mem()
{
    analyse_mem_segments();
    init_allocator();

    PageDirectory *root = create_directory();

    map(root, oldKernelSpace, (void *)0x00000000, 0x1000, 1, 0, 0); // Old kernel space.

    map_to_free(root, kernelPagingDirectory, 1, 1, 0, 1); // Global location of the kernel page directory.

    // The page allocation system.
    map(root, kernelPageAllocTree, (void *)treeLocation, 1UL << (lowest_exp2(allocationTreeLength) - PAGE_SIZE_EXP), 1, 0, 0);
    treeLocation = (const char *)kernelPageAllocTree.Address;

    // The kernel stack.
    unsigned long stack_length = (kernelStackEnd.Raw - kernelStackStart.Raw) >> PAGE_SIZE_EXP; // The number of pages in the kernel stack.
    map_to_free(root, kernelStackStart, stack_length, 1, 0, 0);

    enable_paging(root);

    *(PageDirectory **)kernelPagingDirectory.Address = root; // Add the kernel page directory.

    create_os_gdt();
}

void finalize_mem()
{
    analyse_mem_segments();
    check_paging();
    locate_allocator();

    map(self, kernelVGA, (void *)0x000B8000, 1, 1, 0, 0); // Remapping the VGA buffer.
    change_vga_output((VGACharacter *)kernelVGA.Address);

    unmap(self, oldKernelSpace, 0x1000, FreePageTables);
}