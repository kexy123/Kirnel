#include "memory/address.h"
#include "sector_mgr.h"

/// @brief Loads sectors given a DiskReadAddressPacket.
/// @param driveNumber The drive number.
/// @param drap The DiskReadAddressPacket.
extern SectorReadStatus read_sectors_from_drap(const unsigned char driveNumber, unsigned short drapPtrSegment, unsigned short drapPtrOffset);

/// @brief Instantiates the DiskReadAddressPacket for read_sectors_from_drap().
/// @param drap The DiskReadAddressPacket to instantiate
/// @param sectorCount How many sectors to read.
/// @param startingSector The starting sector.
/// @param buffer The buffer to load into.
void init_drap(DiskReadAddressPacket *drap, unsigned short sectorCount, unsigned long long startingSector, char *buffer)
{
    drap->DAPSize = 0x10;
    drap->Unused = 0x00;

    drap->SectorCount = sectorCount;
    drap->AbsoluteLogicalBlockAddress = startingSector; // 1 LBA = 1 disk sector.

    ptr_to_seg_off(buffer, &(drap->BufferSegment), &(drap->BufferOffset));
}