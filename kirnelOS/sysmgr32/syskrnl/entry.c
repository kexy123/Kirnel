#include "vga/out.h"

/// @brief The entry point for the system kernel.
__attribute__((section(".text.krnl_start")))
void krnl_start()
{
    print_ln("Hello from the SYSKRNL!");

    while (1)
        ;
}