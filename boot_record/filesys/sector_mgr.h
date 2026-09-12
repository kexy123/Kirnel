#pragma once

#define BOOT_SECTOR ((unsigned char *const)0x7C00) // The position of the boot sector when loaded into memory.

/// @brief The DiskAddressPacket (DAP) for reading sectors via the INT 13h AH=42h.
typedef struct __attribute__((packed))
{
    /// @brief The size of the Disk Address Packet (0x10).
    unsigned char DAPSize;

    /// @brief Unused; must be 0x00.
    unsigned char Unused;

    /// @brief The number of sectors to read.
    unsigned short SectorCount;

    /// @brief The offset pointer of where to load the sectors onto.
    unsigned short BufferOffset;

    /// @brief The segment pointer of where to load the sectors onto.
    unsigned short BufferSegment;

    /// @brief The starting position of the first sector to read using logical block addressing.
    unsigned long long AbsoluteLogicalBlockAddress;
} DiskReadAddressPacket;

/// @brief The status when reading the sector via a DiskReadAddressPacket; should be OK.
typedef enum
{
    SECTOR_OK = 0x0,

    /// @brief A disk error happened while reading sectors.
    CLUSTER_DISK_ERROR = 0x1
} SectorReadStatus;

/// @brief Reads the given number of sectors at a starting point onto the buffer.
/// @param driveNumber The drive number. Should be DRIVE_NUMBER.
/// @param sectorCount How many sectors to read.
/// @param startingSector The starting sector.
/// @param buffer The buffer to load onto.
/// @return The sector read status.
SectorReadStatus read_sectors(const unsigned char driveNumber, unsigned short sectorCount, unsigned long long startingSector, char *buffer);