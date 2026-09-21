#include "disksys/structure.h"
#include "fdc.h"
#include "fdc_location.h"
#include "fdc_read.h"

/// @brief Sets up the 2nd channel of the direct memory address to perform any read/write access in the floppy disk to/from the buffer location.
/// @param buffer The location of where to read or write data to.
/// @param bytes The number of bytes in the buffer.
extern void setup_dma(char *buffer, unsigned short bytes);

/// @brief Prepares the direct memory address for reading from the floppy disk.
extern void prepare_dma_for_read();

/// @brief Locates the location in the floppy disk by the drive number, the head, and the cylinder.
/// @param driveNumber The drive number.
/// @param head The head.
/// @param cylinder The cylinder.
void seek(unsigned char driveNumber, unsigned char head, unsigned char cylinder)
{
    send_fdc_command(DATA_FIFO, 0x0F); // Seek command.

    send_fdc_command(DATA_FIFO, (head << 4) | driveNumber); // First four bits is the driveNumber, and last four bits is the head.
    send_fdc_command(DATA_FIFO, cylinder);

    yield_fdc_finish(1);

    read_fdc_command(DATA_FIFO); // Status.
    read_fdc_command(DATA_FIFO); // Cylinder number.
}

FloppyDiskReadStatus read_data(unsigned char driveNumber, LogicalBlockAddress location, unsigned short numberOfSectors, char *buffer)
{
    unsigned char cylinder, head, sector;
    lba_to_chs(location, &cylinder, &head, &sector);

    seek(driveNumber, head, cylinder);

    setup_dma(buffer, numberOfSectors * 512);
    prepare_dma_for_read();

    // https://wiki.osdev.org/Floppy_Disk_Controller#Read/Write
    send_fdc_command(DATA_FIFO, 0x46); // Read command.

    send_fdc_command(DATA_FIFO, (head << 4) | driveNumber);
    send_fdc_command(DATA_FIFO, cylinder);
    send_fdc_command(DATA_FIFO, head);
    send_fdc_command(DATA_FIFO, sector);
    send_fdc_command(DATA_FIFO, 2);
    send_fdc_command(DATA_FIFO, SECTORS_PER_CYLINDER);
    send_fdc_command(DATA_FIFO, 0x1B); // GAP3 default size.
    send_fdc_command(DATA_FIFO, 0xFF); // Ignored and conventional; since the 5th byte is 0x02, we read 0x02 * 128 = 512 bytes in a sector.

    yield_fdc_finish(0);

    unsigned char status0 = read_fdc_command(DATA_FIFO);
    unsigned char status1 = read_fdc_command(DATA_FIFO);
    unsigned char status2 = read_fdc_command(DATA_FIFO);
    read_fdc_command(DATA_FIFO); // Cylinder.
    read_fdc_command(DATA_FIFO); // Ending head.
    read_fdc_command(DATA_FIFO); // Ending sector.
    read_fdc_command(DATA_FIFO); // Always 2.

    status0 >>= 6;
    if (status0 > 0 && status0 < 3)
    {
        // One of the first two bits is set, indicating a floppy error.
        return FLOPPY_READ_ERROR;
    }

    return FLOPPY_READ_OK;
}