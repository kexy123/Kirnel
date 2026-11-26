#include "kirnelOS_api/iostream.hpp"

KirnelOS::IOStream::IOStream() {}

const KirnelOS::IOStream &KirnelOS::IOStream::operator<<(String str) const
{
    __syscall_iostream_out(str);

    return *this;
}

const KirnelOS::IOStream KirnelOS::StreamOut{};
