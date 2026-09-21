#include "cluster.h"
#include "dir.h"
#include "fat.h"
#include "disksys/floppy/fdc_read.h"
#include "structure.h"

_Bool is_end_of_cluster(DiskCluster diskCluster)
{
    // In FAT12, a cluster is considered an end cluster when it is greater than or equal to this binary value:
    // 0x F    F    8
    // 0b 1111_1111_1000
    return diskCluster >= 0xFF8;
}

ReadClusterStatus read_cluster(DiskCluster diskCluster, MemCluster memCluster)
{
    if (is_end_of_cluster(diskCluster))
    {
        return CLUSTER_INVALID_DISK_CLUSTER;
    }

    FloppyDiskReadStatus status;
    if (diskCluster == ROOT_CLUSTER)
    {
        // Read the root cluster instead.
        status = read_data(DRIVE_NUMBER, ROOT_START_SECT, SECTORS_IN_ROOT, memCluster);
    }
    else
    {
        LogicalBlockAddress sectorPosition = CLUSTER_START_SECT + diskCluster - START_CLUSTER;
        status = read_data(DRIVE_NUMBER, sectorPosition, BPB->SectorsPerCluster, memCluster);
    }

    if (status == FLOPPY_READ_OK)
    {
        return CLUSTER_OK;
    }

    return CLUSTER_INTERNAL_ERROR;
}

ReadClusterStatus load_entire_entry(Entry *entry, char *location)
{
    DiskCluster diskCluster = entry->FirstClusterLow;

    NextClusterStatus nextCluster;
    do
    {
        ReadClusterStatus readCluster = read_cluster(diskCluster, location);
        if (readCluster != CLUSTER_OK)
        {
            return CLUSTER_INTERNAL_ERROR;
        }

        location += BYTES_PER_CLUSTER;
        nextCluster = get_next_cluster(&diskCluster);
    } while (nextCluster == NEXTCLUSTER_OK);

    if (nextCluster == NEXTCLUSTER_END_OF_CLUSTER)
    {
        return CLUSTER_OK;
    }

    return CLUSTER_INTERNAL_ERROR;
}