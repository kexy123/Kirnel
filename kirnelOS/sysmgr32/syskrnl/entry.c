#include "interrupts/master.h"
#include "memmgr/master.h"
#include "vga/out.h"
#include "filesys/master.h"

/// @brief The entry point for the system kernel.
__attribute__((section(".text.krnl_start"))) void krnl_start()
{
    finalize_mem();
    print_ln("Memory finalized.");

    init_idt();
    print_ln("Interrupts created.");

    identify_file_system();
    print_ln("File system layer created.");

    print_ln("Hello from the SYSKRNL!");

    while (1)
        ;
}