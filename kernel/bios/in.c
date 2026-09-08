#include "in.h"

/// @brief Stops thread until a keyboard press has been detected.
/// @return The ASCII character of the keyboard press.
extern char in_ch();

char read_char()
{
    return in_ch();
}