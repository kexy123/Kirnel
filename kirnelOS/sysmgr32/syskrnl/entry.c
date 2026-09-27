#include "vga/out.h"
#include "memmgr/mem_segments.h"
#include "memmgr/page_alloc.h"

/// @brief The entry point for the system kernel.
__attribute__((section(".text.krnl_start"))) void krnl_start()
{
    clear_screen();

    print_ln("Hello from the SYSKRNL!");

    analyse_mem_segments();
    init_allocator();

    while (1)
        ;
}