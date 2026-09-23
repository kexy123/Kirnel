#pragma once

/// @brief A character in the [VGA (video graphics array)](https://en.wikipedia.org/wiki/Video_Graphics_Array).
typedef struct __attribute__((packed))
{
    /// @brief The character at this array.
    unsigned char Character;

    /// @brief The foreground and background color for the given character.
    unsigned char Color;
} VGACharacter;

#define print(message) \
    _Generic((message), \
        char *: print_s, \
        const char *: print_s, \
        char: print_c, \
        unsigned char: print_uint, \
        unsigned short: print_uint, \
        unsigned long: print_uint, \
        unsigned int: print_uint \
    )(message);

#define print_ln(message) \
    print(message); \
    print_newl();

/// @brief Prints a single character to the VGA and moves the cursor.
/// @param character The character to print.
void print_c(char character);

/// @brief Prints a null-terminating message to the VGA and moves the cursor to the end of the string.
/// @param message The null-terminating string.
void print_s(const char *message);

/// @brief Prints an unsigned integer to the VGA in base-10 and moves the cursor.
/// @param num The number to print in base-10.
void print_uint(unsigned long num);

/// @brief Prints an unsigned integer to the VGA in base-16 and moves the cursor.
/// @param num The number to print in base-16.
void print_uintx(unsigned long num);

/// @brief Fills the text after the cursor in the current line with spaces and moves the cursor to the next line in the VGA.
void print_newl();

/// @brief Clears the VGA buffer and moves the cursor to the beginning.
void clear_screen();