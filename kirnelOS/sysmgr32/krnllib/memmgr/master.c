#include "master.h"
#include "gdt/master.h"
#include "kernel_virtual_layout.h"
#include "kernel_jump.h"
#include "paging/mem_segments.h"
#include "paging/page_alloc.h"
#include "paging/paging.h"
#include "utils/bit.h"

void init_mem()
{
    // create_os_gdt();
    analyse_mem_segments();
    init_allocator();

    PageDirectory *root = create_directory(1);

    map(root, old_kernel_space, (void *)0x00000000, 0x1000, 1, 0, 0, 1); // Kernel lower half. This is to ensure the integrity of the code pointer.

    // The page allocation system.
    map(root, kernel_page_alloc_tree, (void *)treeLocation, 1 << (lowest_exp2(allocationTreeLength) - PAGE_SIZE_EXP), 1, 0, 0, 1);
    treeLocation = (const char *)kernel_page_alloc_tree.Address;

    enable_paging(root);

    // unmap(self, (Address){.Raw = 0x00000000}, 0x1000, NoFree, 1);
}