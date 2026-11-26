#include "string.hpp"

/// @brief Performs a system call to output characters to the stream.
/// @param str The string to output.
extern "C" __attribute__((__cdecl__)) void __syscall_iostream_out(KirnelOS::String str);

namespace KirnelOS
{
    /// @brief A class regarding the input-output stream of this program.
    class IOStream
    {
    public:
        /// @brief Instantiates an input-output object.
        IOStream();

        /// @brief Sends the string to the stream's output.
        /// @param str The string to send.
        /// @return Itself.
        const IOStream &operator<<(String str) const;
    };

    /// @brief An object regarding stream outputting of this program.
    const extern IOStream StreamOut;
}