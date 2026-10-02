#pragma once

/// @brief Converts an unsigned integer into a null-terminating string.
/// @param num The number to convert.
/// @return The null-terminating string.
const char *uint_to_str(unsigned long num);

/// @brief Converts an unsigned integer into a null-terminating hexadecimal string.
/// @param num The number to convert.
/// @param leadingZeroes The number of leading zeroes to add. Note that it doesn't truncate the length of the number.
/// @return The null-terminating string in hexadecimal form using capital letters.
const char *uint_to_strx(unsigned long num, unsigned char leadingZeroes);