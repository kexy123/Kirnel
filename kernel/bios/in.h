/// @brief Reads one keyboard character input.
/// @return The ASCII character of the keyboard press.
char read_char();

/// @brief Reads a line of characters until a carraige return is received or the output buffer will overflow.
/// @return The string of the input.
const char *read_line_and_output();