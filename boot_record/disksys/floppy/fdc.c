#include "fdc.h"

/// @brief Sends a command to the floppy disk controller of the given port and byte if the FDC allows it.
/// @param port The port to send the byte to.
/// @param byte The byte to send.
extern void send_fdc_command(unsigned short port, unsigned char byte);