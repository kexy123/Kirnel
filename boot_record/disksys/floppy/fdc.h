#pragma once

/// @brief The [ports](https://wiki.osdev.org/Floppy_Disk_Controller#Registers) of the floppy disk controller.
typedef enum : unsigned short
{
    /// @brief Used to configure the floppy disk controller.
    DIGITAL_OUTPUT_REGISTER = 0x3F2,

    /// @brief For data read and write operations as well as configuration for reading and writing.
    DATA_FIFO = 0x3F5
} FloppyDiskRegisters;

/// @brief Sends a command to the floppy disk controller of the given port and byte if the FDC allows it.
/// @param port The port to send the byte to.
/// @param byte The byte to send.
extern void send_fdc_command(unsigned short port, unsigned char byte);

/// @brief Waits until the floppy disk controller has completed its operation and raised an IRQ6.
/// @param senseInterrupt Once IRQ6 is raised, should a sense interrupt command (0x08) be sent to the DATA_FIFO.
void yield_fdc_finish(_Bool senseInterrupt);

/// @brief Initiates the floppy disk controller.
void init_fdc();