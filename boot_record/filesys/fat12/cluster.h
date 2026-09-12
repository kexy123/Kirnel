#pragma once

#define ROOT_CLUSTER 1  // Although not a cluster, the value that is used to read the root directory as if it was a cluster.
#define START_CLUSTER 2 // The starting cluster that exists in the cluster segment of FAT12.

/// @brief Refers to the position of a cluster in the disk.
typedef unsigned short DiskCluster;

/// @brief Refers to the address of the starting byte of a loaded cluster in memory.
typedef char *MemCluster;

/// @brief The status when reading a cluster via read_cluster(); should be CLUSTER_OK.
typedef enum
{
    CLUSTER_OK = 0x0,

    /// @brief An internal error (usually a disk read error) occurred while reading clusters.
    CLUSTER_INTERNAL_ERROR,

    /// @brief The given disk cluster is invalid.
    CLUSTER_INVALID_DISK_CLUSTER
} ReadClusterStatus;

/// @brief Loads the contents from the given disk cluster position into memory.
/// @param diskCluster The disk cluster position whose contents to load.
/// @param memCluster The memomry buffer to load onto.
ReadClusterStatus read_cluster(DiskCluster diskCluster, MemCluster memCluster);

/// @brief Determines if the given disk cluster is an ending cluster.
/// @param diskCluster The end cluster.
/// @return 1 if it is an end cluster; otherwise 0.
_Bool is_end_of_cluster(DiskCluster diskCluster);