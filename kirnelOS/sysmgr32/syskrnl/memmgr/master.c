#include "master.h"
#include "memmgr/gdt/master.h"
#include "memmgr/kernel_virtual_layout.h"
#include "memmgr/krnl_jump.h"
#include "memmgr/mem_segments.h"
#include "memmgr/page_alloc.h"
#include "memmgr/paging.h"

void init_mem()
{
    create_os_gdt();
    analyse_mem_segments();
    init_allocator();

    PageDirectory *root = create_directory();

    map(root, (void *)0x00000000, (void *)0x00000000, 32768, 1, 0, 0);   // Kernel lower half. This is to ensure the integrity of the code pointer.
    map(root, (void *)kernel_space, (void *)0x00000000, 32768, 1, 0, 0); // Kernel will be moved to the upper half.

    map(root, (void *)ALLOC_START, (void *)ALLOC_START, allocationTreeLength >> PAGE_SIZE_EXP, 1, 0, 0);

    enable_paging(root);

    kernel_jump();
}