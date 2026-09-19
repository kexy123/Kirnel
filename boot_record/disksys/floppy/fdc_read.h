#pragma once

#include "disksys/structure.h"

/// @brief The status from reading from the floppy disk; should be FLOPPY_READ_OK.
typedef enum
{
    FLOPPY_READ_OK = 0,

    /// @brief The floppy disk reading returned a status error.
    FLOPPY_READ_ERROR = 1
} FloppyDiskReadStatus;

/// @brief Reads from the floppy disk onto the given buffer.
/// @param driveNumber The drive number.
/// @param location The location in the floppy disk.
/// @param numberOfSectors The number of sectors to read from. The buffer must be able to fit that many sectors.
/// @param buffer The buffer to load the data onto.
/// @return The floppy disk read status.
FloppyDiskReadStatus read_data(unsigned char driveNumber, LogicalBlockAddress location, unsigned short numberOfSectors, char *buffer);