#include "string.hpp"

unsigned long KirnelOS::String::LengthOf(const char *string)
{
    const char *old = string;

    while (*string != '\0')
    {
        string++;
    }

    return string - old;
}

// Initializes the FirstChar and length fields.
KirnelOS::String::String(const char *string) : FirstChar(string), Length(KirnelOS::String::LengthOf(string)) {}