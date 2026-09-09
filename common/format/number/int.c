#include "int.h"
// #include "kernel/bios/kill.h"

const char *uint_to_str(unsigned int num)
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

unsigned int str_to_uint(const char *num)
{
    unsigned int result = 0;
    for (int i = 0; i < 5; i++)
    {
        switch (num[i])
        {
        case '\0':
            return result;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            result *= 10;
            result += num[i] - '0';
            break;
            
        default:
            return 0;
        }
    }

    return result;
}