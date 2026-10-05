#pragma once

/// @brief Copies the bytes from the address of source to the address of destination.
/// @param source The source address.
/// @param destination The destination address.
/// @param byteCount The number of bytes to copy.
void copy_to(void *source, void *destination, short byteCount);

/// @brief Zeroes out the given number of bytes starting from the source address.
/// @param source The source address.
/// @param bytes The number of bytes to zero out.
void zero_fill(void *source, unsigned long bytes);

/// @brief Fills the given number of bytes starting from the source address with all set bits.
/// @param source The source address.
/// @param bytes The number of bytes whose bits to be all set.
void one_fill(void *source, unsigned long bytes);