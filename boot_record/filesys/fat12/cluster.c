#include "cluster.h"
#include "structure.h"
#include "filesys/sector_mgr.h"

ReadClusterStatus read_cluster(DiskCluster diskCluster, MemCluster memCluster)
{
    if (is_end_of_cluster(diskCluster))
    {
        return CLUSTER_INVALID_DISK_CLUSTER;
    }

    SectorReadStatus status;
    if (diskCluster == ROOT_CLUSTER)
    {
        // Read the root cluster instead.
        status = read_sectors(DRIVE_NUMBER, SECTORS_IN_ROOT, ROOT_START_SECT, memCluster);
    }
    else
    {
        LBASector sectorPosition = CLUSTER_START_SECT + diskCluster - START_CLUSTER + 1;
        status = read_sectors(DRIVE_NUMBER, BPB->SectorsPerCluster, sectorPosition, memCluster);
    }

    if (status == SECTOR_OK)
    {
        return CLUSTER_OK;
    }

    return CLUSTER_INTERNAL_ERROR;
}

_Bool is_end_of_cluster(DiskCluster diskCluster)
{
    // In FAT12, a cluster is considered an end cluster when it is greater than or equal to this binary value:
    // 0x F    F    8
    // 0b 1111_1111_1000
    return diskCluster >= 0xFF8;
}