#include "out.h"

const char *new_line = "\n\r"; // New line + carriage return.

/// @brief Prints a null-terminating message to the BIOS.
/// @param message The null-terminating string.
extern void out_chp(const char *message);

/// @brief Prints a character to the BIOS.
/// @param character The character.
extern void out_ch(char character);

void print_s(const char *message)
{
    out_chp(message);
}

void print_c(char character)
{
    out_ch(character);
}

void print_newl()
{
    out_chp(new_line);
}