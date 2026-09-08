#include "bios/in.h"
#include "bios/out.h"
#include "format/number/int.h"

/// @brief The starting method upon BIOS boot.
void krnl_boot(void)
{
    print("Enter key: ");
    print(read_char());
    print_newl();

    // Fibonacci sequence.
    unsigned int a = 0, b = 1, c;
    for (unsigned int i = 0; i < 20; i++)
    {
        c = a + b;
        a = b;
        b = c;
        print_newl();
        print(to_decimal(a));
    }

    while (1)
        ;
}