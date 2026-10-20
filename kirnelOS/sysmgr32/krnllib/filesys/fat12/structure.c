#include "structure.h"

/// @brief The parameter block object. Note that this must be consistent with the boot file as well.
static BIOSParameterBlock object = {
    .OEM = "KIRNELOS",
    .BytesPerSector = 512,
    .SectorsPerCluster = 1,
    .ReservedSectors = 32,
    .FATCount = 2,
    .RootEntryCount = 224,
    .TotalSectors = 2880,
    .MediaDescriptor = 0xF0,
    .SectorsPerFAT = 9,
    .SectorsPerTrack = 18,
    .HeadCount = 2,
    .HiddenSectors = 0,
    .LargeSectors = 0,

    .Extended = (ExtendedBIOSParameterBlock){
        .DriveNumber = 0,
        .Reserved = 0,
        .BootSignature = 0x29,
    },
};

const BIOSParameterBlock *bpbObject = (const BIOSParameterBlock *)&object;
