#pragma once

#include "disksys/structure.h"

/// @brief The status when reading or writing in the floppy disk; should be ACCESS_OK.
typedef enum
{
    ACCESS_OK = 0x0
} DataAccessStatus;

/// @brief Reads the sector from the given logical block address.
/// @param driveNumber The drive number.
/// @param address The logical block address to read from.
/// @param buffer The location to load the data onto.
/// @return The status of the data access.
DataAccessStatus read_data(unsigned char driveNumber, LogicalBlockAddress address, char *buffer);