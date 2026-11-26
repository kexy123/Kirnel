#include "kirnelOS_api/iostream.hpp"

[[gnu::section(".text.shell_start")]]
void shell_start()
{
    KirnelOS::StreamOut << "Hello, World! From my first program!";

    while (1)
        ;
}
