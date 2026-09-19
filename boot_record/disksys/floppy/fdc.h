#pragma once

/// @brief The [ports](https://wiki.osdev.org/Floppy_Disk_Controller#Registers) of the floppy disk controller.
typedef enum : unsigned short
{
    /// @brief Used to configure the floppy disk controller.
    DIGITAL_OUTPUT_REGISTER = 0x3F2,

    /// @brief For data read and write operations as well as configuration for reading and writing.
    DATA_FIFO = 0x3F5
} FloppyDiskCommands;

/// @brief Sends a command to the floppy disk controller of the given port and byte if the FDC allows it.
/// @param port The port to send the byte to.
/// @param byte The byte to send.
extern void send_fdc_command(unsigned short port, unsigned char byte);

/// @brief Initiates the floppy disk controller.
void init_fdc();