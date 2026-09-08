#include "log.h"

const char *new_line = "\n\r"; // New line + carriage return.

/// @brief Prints a null-terminating message to the BIOS.
/// @param message The null-terminating string.
extern void print_chp(const char *message);

void print_s(const char *message)
{
    print_chp(message);
}

void print_newl()
{
    print_chp(new_line);
}