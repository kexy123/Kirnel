#include "int.h"

const char *uint_to_decimal(unsigned int num)
{
    static char result[11];

    // Base-10 conversion.
    int i = 0;
    do
    {
        result[i] = '0' + (num % 10);
        num /= 10;
        i++;
    } while (num > 0);
    result[i] = '\0';

    // Reverse digits.
    int right = i - 1;
    int left = 0;
    while (left < right)
    {
        char temp = result[right];
        result[right] = result[left];
        result[left] = temp;

        left++;
        right--;
    }

    return result;
}