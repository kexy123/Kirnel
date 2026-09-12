#ifndef common_format_number_int_h
#define common_format_number_int_h

/// @brief Returns a char* to the unsigned int in decimal form.
/// @param num The unsigned int.
/// @return The char* to the unsigned int in decimal form.
const char *uint_to_str(unsigned int num);

/// @brief Converts a char* to an unsigned int.
/// @param num The char*.
/// @return The unsigned int result.
unsigned int str_to_uint(const char *num);

#define to_str(num) \
    _Generic((num),     \
        unsigned int: uint_to_str \
    )(num)

#endif