#pragma once

/// @brief The string.
typedef struct
{
    /// @brief The location of the string.
    const char *String;

    /// @brief The length of the string.
    unsigned long Length;
} StringView;

/// @brief Duplicates a string to a free location in the heap.
/// @param string The string to duplicate.
/// @param length The length of the string to copy into a heap.
/// @return A string view pointing to the new string.
StringView dupe_string(const char *string, unsigned long length);

/// @brief Destroys a given string.
/// @param string The string to destroy.
void destroy_string(StringView string);