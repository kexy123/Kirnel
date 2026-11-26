#pragma once

#ifdef __cplusplus

namespace KirnelOS
{
    /// @brief An object that is the view of a sequence of characters.
    class String
    {
    private:
        /// @brief The starting location of the string.
        const char *FirstChar;

    public:
        /// @brief Reads the length of a string by its character pointer up to its first null-terminating byte.
        /// @param string The string whose length to read.
        /// @return The length of the string.
        static unsigned long LengthOf(const char *string);

        /// @brief The length of the String.
        const unsigned long Length;

        /// @brief Creates a String object from a character pointer.
        /// @param string The character pointer whose string to create.
        String(const char *string);
    };
}

#else

/// @brief The string.
typedef struct
{
    /// @brief The location of the string.
    const char *String;

    /// @brief The length of the string.
    const unsigned long Length;
} StringView;

/// @brief Duplicates a string to a free location in the heap.
/// @param string The string to duplicate.
/// @param length The length of the string to copy into a heap.
/// @return A string view pointing to the new string.
StringView dupe_string(const char *string, unsigned long length);

/// @brief Destroys a given string.
/// @param string The string to destroy.
void destroy_string(StringView string);

#endif