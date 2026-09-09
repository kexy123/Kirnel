#include "boot/in.h"
#include "boot/out.h"
// #include "format/number/int.h"

/// @brief The starting method upon boot being initiated by the boot sector.
void krnl_boot(void)
{
    print("ENTER A: ");
    read_line();
    print("ENTER B: ");
    read_line();

    // Fibonacci sequence.
    for (unsigned int i = 0; i < 20; i++)
    {
        print("Hello, world!");
        print_newl();
    }

    while (1)
        ;
}