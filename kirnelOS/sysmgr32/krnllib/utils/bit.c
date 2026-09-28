#include "bit.h"
#include "flow.h"

unsigned long lowest_exp2(unsigned long num)
{
    if (num == 0)
    {
        panic();
    }

    // Counts leading zeroes then determines the power of two from there.
    unsigned long power = 31 - __builtin_clz(num);

    // Bitwise form to determine if a number is a power of two.
    if ((num & (num - 1)) == 0)
    {
        return power;
    }

    // The number is slightly greater than 2 << power, so increment it once more.
    return power + 1;
}