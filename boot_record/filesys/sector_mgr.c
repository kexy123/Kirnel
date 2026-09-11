#include "memory/address.h"
#include "sector_mgr.h"

void init_drap(DiskReadAddressPacket *drap, unsigned short sectorCount, unsigned long long startingSector, char *buffer)
{
    drap->DAPSize = 0x10;
    drap->SectorCount = sectorCount;
    drap->AbsoluteLogicalBlockAddress = startingSector; // 1 LBA = 1 disk sector.

    ptr_to_seg_off(buffer, &(drap->BufferSegment), &(drap->BufferOffset));
}