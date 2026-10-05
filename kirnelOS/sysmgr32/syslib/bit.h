#pragma once

/// @brief Gets the lowest exponent power of two whose value is greater than or equal to the given number. If 0, returns 0.
/// @param num The number.
/// @return The exponent power of two.
unsigned long lowest_exp2(unsigned long num);

/// @brief Gets the highest exponent power of two whose value is less than or equal to the given number. If 0, returns 0.
/// @param num The number.
/// @return The exponent power of two.
unsigned long highest_exp2(unsigned long num);