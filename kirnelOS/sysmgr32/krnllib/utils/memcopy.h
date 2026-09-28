#pragma once

/// @brief Copies the bytes from the address of source to the address of destination.
/// @param source The source address.
/// @param destination The destination address.
/// @param byteCount The number of bytes to copy.
void copy_to(void *source, void *destination, short byteCount);