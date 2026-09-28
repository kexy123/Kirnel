#pragma once

#include "disksys/structure.h"

#define SECTORS_PER_CYLINDER (18) // Specification for CHS in a 1.44 MB floppy disk.

/// @brief Converts a logical block address to [CHS](https://en.wikipedia.org/wiki/Cylinder-head-sector) positioning for a floppy disk.
/// @param address The logical block address to convert.
/// @param cylinder The location to assign the cylinder value at.
/// @param head The location to assign the head value at.
/// @param sector The location to assign the sector value at.
void lba_to_chs(LogicalBlockAddress address, unsigned char *cylinder, unsigned char *head, unsigned char *sector);