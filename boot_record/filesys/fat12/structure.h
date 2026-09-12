#pragma once

#include "filesys/sector_mgr.h"
#include "dir.h"

#define BPB ((const BIOSParameterBlock *const)BOOT_SECTOR) // The BIOS Parameter Block.

#define DRIVE_NUMBER (BPB->Extended.DriveNumber) // The drive number.

#define SECTORS_IN_ROOT (BPB->RootEntryCount * sizeof(Entry) / BPB->BytesPerSector) // The number of sectors in the root.

#define FAT1_START_SECT (BPB->ReservedSectors)                            // The FAT1 table lives directly after the reserved sectors.
#define ROOT_START_SECT (FAT1_START + BPB->FATCount * BPB->SectorsPerFAT) // The root directory lives directly after the FATs.
#define CLUSTER_START_SECT (ROOT_START + SECTORS_IN_ROOT)                 // The starting cluster lives directly after the root directory.

/// @brief A uint8_t.
typedef unsigned char byte;

/// @brief A word; a uint16_t.
typedef unsigned short word;

/// @brief A double word; a uint32_t.
typedef unsigned long dWord;

/// @brief The extended BIOS parameter block.
typedef struct __attribute((packed))
{
    /// @brief The drive number.
    byte DriveNumber;

    /// @brief Reserved.
    byte Reserved;

    /// @brief The boot signature, which defines the format of the fields under this struct.
    byte BootSignature;
} ExtendedBIOSParameterBlock;

/// @brief The structure of the BIOS parameter block from the FAT12 file system.
typedef struct __attribute__((packed))
{
    /// @brief Unused.
    byte Jump[3];

    /// @brief The original equipment manufacturer.
    byte OEM[8];

    /// @brief The number of bytes per sector.
    word BytesPerSector;

    /// @brief The number of sectors per cluster.
    byte SectorsPerCluster;

    /// @brief The number of starting sectors that are reserved, usually for the boot loader.
    word ReservedSectors;

    /// @brief The number of FATs (File Allocation Tables) that exist.
    byte FATCount;

    /// @brief The number of entries that can exist in the root directory.
    word RootEntryCount;

    /// @brief The total number of sectors that this FAT system can use.
    word TotalSectors;

    /// @brief ???
    byte MediaDescriptor;

    /// @brief The number of sectors that each FAT has.
    word SectorsPerFAT;

    /// @brief ???
    word SectorsPerTrack;

    /// @brief ???
    word HeadCount;

    /// @brief ???
    dWord HiddenSectors;

    /// @brief Extra field for storing the number of total sectors.
    dWord LargeSectors;

    /// @brief The extended parameter block.
    ExtendedBIOSParameterBlock Extended;
} BIOSParameterBlock;