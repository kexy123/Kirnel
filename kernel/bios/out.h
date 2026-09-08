#ifndef kernel_bios_out_h
#define kernel_bios_out_h

const char *NEW_LINE;

/// @brief Prints a null-terminating message to the BIOS.
/// @param message The null-terminating string.
void print_s(const char *message);

/// @brief Prints a single character to the BIOS.
/// @param character The character to print.
void print_c(char character);

/// @brief Prints a new line feed to the BIOS.
void print_newl();

#define print(message) \
    _Generic((message), \
        char *: print_s, \
        const char *: print_s, \
        char: print_c \
    )(message)

#endif