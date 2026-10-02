#include "int.h"

const char *uint_to_str(unsigned long num)
{
    static char result[11];

    int i = 10;
    result[i] = '\0';

    // Base-10 conversion.
    do
    {
        i--;
        result[i] = '0' + (num % 10);
        num /= 10;
    } while (num > 0);

    return (const char *)(result) + i;
}

const char *uint_to_strx(unsigned long num, unsigned char leadingZeros)
{
    static char result[9];
    unsigned char leadingZeroSentinel = 9 - leadingZeros;

    int i = 9;
    result[i] = '\0';

    // Base-16 conversion using capital letters.
    do
    {
        i--;

        switch (num % 16)
        {
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            result[i] = 'A' - 10 + num % 16;
            break;

        default:
            result[i] = '0' + (num % 16);
            break;
        }

        num /= 16;
    } while (num > 0 || i > leadingZeroSentinel);

    return (const char *)(result) + i;
}