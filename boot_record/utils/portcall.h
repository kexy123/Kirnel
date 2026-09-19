#pragma once

/// @brief Passes a byte of a command to the port.
/// @param port The port to pass the byte into.
/// @param byte The byte value to pass in.
extern void port_call(unsigned short port, unsigned char byte);

/// @brief Returns the given byte from a port.
/// @param port The port to read from.
/// @return The byte that was read.
extern unsigned char port_read(unsigned short port);