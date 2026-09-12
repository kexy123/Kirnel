#pragma once

/// @brief Reads one keyboard character input from the boot menu.
/// @return The ASCII character of the keyboard press.
char read_char();

/// @brief Reads a line of characters until a carraige return is received or it has reached 255 characters.
/// @return The string of the input.
const char *read_line();