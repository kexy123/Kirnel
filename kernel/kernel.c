#include "bios/log.h"
#include "format/number/int.h"

/// @brief The starting method upon BIOS boot.
void krnl_boot(void)
{
    // Fibonacci sequence.
    unsigned int a = 0, b = 1, c;
    for (unsigned int i = 0; i < 20; i++)
    {
        c = a + b;
        a = b;
        b = c;
        print_newl();
        print_s(to_decimal(a));
    }

    while (1)
        ;
}