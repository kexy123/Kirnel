#include "boot/in.h"
#include "boot/out.h"
#include "format/number/int.h"

/// @brief The starting method upon boot being initiated by the boot sector.
void krnl_boot(void)
{
    print("ENTER A: ");
    unsigned int a = str_to_uint(read_line());
    print("ENTER B: ");
    unsigned int b = str_to_uint(read_line());

    // Fibonacci sequence.
    unsigned int c;
    for (unsigned int i = 0; i < 20; i++)
    {
        c = a + b;
        a = b;
        b = c;
        print(to_str(a));
        print_newl();
    }

    while (1)
        ;
}