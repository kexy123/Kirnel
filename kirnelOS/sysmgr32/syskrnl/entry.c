#include "vga/out.h"

/// @brief The entry point for the system kernel.
__attribute__((section(".text.krnl_start")))
void krnl_start()
{
    clear_screen();

    print_ln("Hello from the SYSKRNL!");

    print_uint(56);

    while (1)
        ;
}