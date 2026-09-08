#ifndef common_format_number_int_h
#define common_format_number_int_h

/// @brief Returns a char* to the unsigned int in decimal form.
/// @param num The unsigned int.
/// @return The char* to the unsigned int in decimal form.
const char *uint_to_decimal(unsigned int num);

#define to_decimal(num) \
    _Generic((num),     \
        unsigned int: uint_to_decimal \
    )(num)

#endif