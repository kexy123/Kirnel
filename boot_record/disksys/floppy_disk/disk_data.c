#include "disk_data.h"
#include "disksys/structure.h"

#define DISK_READ_PORT 0x3F5

// Specification for CHS in a 1.44 MB floppy disk.
#define SECTORS_PER_CYLINDER 18

/// @brief Waits until the disk can be accessed and commanded to, then passes a byte of a command to the port.
/// @param port The port to pass the byte into.
/// @param byte The byte value to pass in.
extern void pass_command(unsigned short port, unsigned char byte);

/// @brief Sets up the 2nd channel of the direct memory address to perform any read/write access in the floppy disk to/from the buffer location.
/// @param buffer The location of where to read or write data to.
/// @param bytes The number of bytes in the buffer.
extern void setup_dma(char *buffer, unsigned short bytes);

/// @brief Converts a logical block address to [CHS](https://en.wikipedia.org/wiki/Cylinder-head-sector) positioning for a floppy disk.
/// @param address The logical block address to convert.
/// @param cylinder The location to assign the cylinder value at.
/// @param head The location to assign the head value at.
/// @param sector The location to assign the sector value at.
void chs_from_lba(LogicalBlockAddress address, unsigned char *cylinder, unsigned char *head, unsigned char *sector)
{
    *cylinder = address / (2 * SECTORS_PER_CYLINDER);
    unsigned char remainder = address % (2 * SECTORS_PER_CYLINDER);

    *head = remainder / SECTORS_PER_CYLINDER;

    *sector = remainder % SECTORS_PER_CYLINDER + 1; // Sectors are 1-indexed.
}

DataAccessStatus read_data(unsigned char driveNumber, LogicalBlockAddress address, char *buffer)
{
    unsigned char cylinder, head, sector;
    chs_from_lba(address, &cylinder, &head, &sector);

    setup_dma(buffer, 512);

    // The byte commands for reading sectors in a floppy disk: https://wiki.osdev.org/Floppy_Disk_Controller#Read/Write
    pass_command(DISK_READ_PORT, 0x06); // Read command.

    pass_command(DISK_READ_PORT, (head * 4) | driveNumber);
    pass_command(DISK_READ_PORT, cylinder);
    pass_command(DISK_READ_PORT, head);
    pass_command(DISK_READ_PORT, sector);
    pass_command(DISK_READ_PORT, 0x02);
    pass_command(DISK_READ_PORT, SECTORS_PER_CYLINDER);
    pass_command(DISK_READ_PORT, 0x1B); // Gap 1 default size.
    pass_command(DISK_READ_PORT, 0xFF); // Ignored and conventional; since the 5th byte is 0x02, we read 0x02 * 128 = 512 bytes in a sector.

    // TODO: Implement IDT and wait for IRQ6.

    return ACCESS_OK;
}