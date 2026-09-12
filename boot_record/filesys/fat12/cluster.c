#include "cluster.h"
#include "structure.h"
#include "filesys/sector_mgr.h"

ReadClusterStatus read_cluster(DiskCluster diskCluster, MemCluster memCluster)
{
    if (diskCluster == END_OF_CLUSTER)
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
        LBASector sectorPosition = CLUSTER_START_SECT + diskCluster - START_CLUSTER;
        status = read_sectors(DRIVE_NUMBER, BPB->SectorsPerCluster, sectorPosition, memCluster);
    }

    if (status == SECTOR_OK)
    {
        return CLUSTER_OK;
    }

    return CLUSTER_INTERNAL_ERROR;
}