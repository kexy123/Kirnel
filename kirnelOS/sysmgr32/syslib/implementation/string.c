#include "string.h"
#include "memory/memory.h"
#include "memory/memcopy.h"

StringView dupe_string(const char *string, unsigned long length)
{
    char *newString = (char *)malloc(length);

    copy_to((void *)string, (void *)newString, length);

    return (StringView){
        .String = newString,
        .Length = length,
    };
}

void destroy_string(StringView string)
{
    free((void *)string.String);
}