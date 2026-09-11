#pragma once

#define BOOT_SECTOR ((unsigned char* const)0x7C00)

/// @brief The DiskAddressPacket (DAP) for reading sectors via the INT 13h AH=42h.
typedef struct __attribute__((packed))
{
    /// @brief The size of the Disk Address Packet (0x10).
    unsigned char       DAPSize;

    /// @brief Unused.
    unsigned char       Unused;

    /// @brief The number of sectors to read.
    unsigned short      SectorCount;

    /// @brief The segment pointer of where to load the sectors onto.
    unsigned short      BufferSegment;

    /// @brief The offset pointer of where to load the sectors onto.
    unsigned short      BufferOffset;

    /// @brief The starting position of the first sector to read using logical block addressing.
    unsigned long long  AbsoluteLogicalBlockAddress;
} DiskReadAddressPacket;

/// @brief Loads sectors given a DiskReadAddressPacket.
/// @param drap The DiskReadAddressPacket.
extern void read_sector(const DiskReadAddressPacket *drap);

/// @brief Instantiates the DiskReadAddressPacket for read_sector().
/// @param drap The DiskReadAddressPacket to instantiate
/// @param sectorCount How many sectors to read.
/// @param startingSector The starting sector.
/// @param buffer The buffer to load into.
void init_drap(DiskReadAddressPacket *drap, unsigned short sectorCount, unsigned long long startingSector, char *buffer);