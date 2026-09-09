#include "in.h"
#include "out.h"

const char CARRIAGE_RETURN = '\r';

/// @brief Stops thread until a keyboard press has been detected.
/// @return The ASCII character of the keyboard press.
extern char in_ch();

char read_char()
{
    return in_ch();
}

const char *read_line()
{
    static char buffer[256];

    // Can only read up to 255 characters.
    unsigned int i = 0;
    for (; i < 255; i++)
    {
        char input = in_ch();
        print_c(input);

        if (input == CARRIAGE_RETURN)
        {
            // End early on the carraige return pressed via the Enter key.
            print_c('\n');
            break;
        }

        buffer[i] = input;
    }

    buffer[i] = '\0';

    return buffer;
}