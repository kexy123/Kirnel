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