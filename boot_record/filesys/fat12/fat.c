#include "cluster.h"
#include "fat.h"
#include "structure.h"
#include "disksys/floppy/fdc_read.h"

NextClusterStatus get_next_cluster(DiskCluster *cluster)
{
    if (*cluster < START_CLUSTER)
    {
        // Note that an invalid cluster wouldn't usually be a passed-in user parameter, but from the result of a malformed linked list structure.
        return NEXTCLUSTER_CORRUPTED;
    }

    unsigned char buffer[BPB->SectorsPerFAT * BPB->BytesPerSector];
    MemFAT fatTable = buffer;
    read_data(DRIVE_NUMBER, FAT1_START_SECT, BPB->SectorsPerFAT, fatTable);

    fatTable += *cluster + *cluster / 2;
    if (*cluster & 1)
    {
        //                 |-----------|
        // 00000000 0000 | 0000 00000000
        //          ^ fatTable is here
        *cluster = (fatTable[0] >> 4) + ((unsigned short)fatTable[1] << 4);
    }
    else
    {
        // |-----------|
        // 00000000 0000 | 0000 00000000
        // ^ fatTable is here
        *cluster = fatTable[0] + (fatTable[1] << 8);
        *cluster &= 0x0FFF;
    }

    if (is_end_of_cluster(*cluster))
    {
        return NEXTCLUSTER_END_OF_CLUSTER;
    }

    return NEXTCLUSTER_OK;
}