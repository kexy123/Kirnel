#include "vga/out.h"
#include "memmgr/gdt/master.h"
#include "memmgr/mem_segments.h"
#include "memmgr/page_alloc.h"
#include "memmgr/paging.h"

/// @brief The entry point for the system kernel.
__attribute__((section(".text.krnl_start"))) void krnl_start()
{
    clear_screen();

    print_ln("Hello from the SYSKRNL!");

    create_os_gdt();
    analyse_mem_segments();
    init_allocator();

    PageDirectory *root = create_directory();
    map(root, (void *)0x00000000, (void *)0x00000000, 32768, 1, 0, 0);
    map(root, (void *)ALLOC_START, (void *)ALLOC_START, allocationTreeLength >> PAGE_SIZE_EXP, 1, 0, 0);
    enable_paging(root);

    print_ln("Paging enabled.");

    while (1)
        ;
}