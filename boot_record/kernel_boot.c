#include "boot/in.h"
#include "boot/out.h"

/// @brief The starting method upon boot being initiated by the boot sector.
void krnl_boot(void)
{
    print("Enter key to continue: ");
    read_char();

    print_newl();

    // TODO: Read from FAT12 the kirnelOS/sysmgr/kernel_load.c file.

    while (1)
        ;
}