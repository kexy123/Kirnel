#pragma once

#include "cluster.h"
#include "structure.h"

/// @brief Refers to the address of the starting byte of a FAT in memory.
typedef unsigned char *MemFAT;

/// @brief The status when getting the next cluster; should be NEXTCLUSTER_OK.
typedef enum
{
    NEXTCLUSTER_OK = 0x0,

    /// @brief The end of the cluster was found.
    NEXTCLUSTER_END_OF_CLUSTER = 0x1,

    /// @brief The linked structure of the cluster is corrupted; it has no correct terminating cluster.
    NEXTCLUSTER_CORRUPTED = 0x2,
} NextClusterStatus;

/// @brief Goes to the next cluster from the FAT (File Allocation Table).
/// @param cluster The disk cluster to read and set the next cluster to.
/// @return The status when retrieving the next cluster.
NextClusterStatus get_next_cluster(DiskCluster *cluster);