#pragma once

#define print(message) \
    _Generic((message), \
        char *: print_s, \
        const char *: print_s, \
        char: print_c, \
        unsigned char: print_uint, \
        unsigned short: print_uint \
    )(message);

#define print_ln(message) \
    print(message); \
    print_newl();

const char *NEW_LINE;

/// @brief Prints a null-terminating message to the boot menu.
/// @param message The null-terminating string.
void print_s(const char *message);

/// @brief Prints a single character to the boot menu.
/// @param character The character to print.
void print_c(char character);

/// @brief Prints an unsigned integer to the boot menu in base-10.
/// @param num The number to print in base-10.
void print_uint(unsigned short num);

/// @brief Prints a new line feed to the boot menu.
void print_newl();