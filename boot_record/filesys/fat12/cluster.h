#pragma once

#define END_OF_CLUSTER 0 // The sentinel when reading a linked list of DiskCluster positions.
#define ROOT_CLUSTER 1   // Although not a cluster, the value that is used to read the root directory as if it was a cluster.
#define START_CLUSTER 2  // The starting cluster that exists in the cluster segment of FAT12.

/// @brief Refers to the position of a cluster in the disk.
typedef unsigned long DiskCluster;

/// @brief Refers to the address of the starting byte of a loaded cluster in memory.
typedef char *MemCluster;

/// @brief The status when reading a cluster via read_cluster(); should be OK.
typedef enum
{
    OK,

    /// @brief A disk error occurred while reading sectors.
    DISK_ERROR,

    /// @brief The given disk cluster is invalid.
    INVALID_DISK_CLUSTER
} ReadClusterStatus;

/// @brief Loads the contents from the given disk cluster position into memory.
/// @param diskCluster The disk cluster position whose contents to load.
/// @param memCluster The memomry buffer to load onto.
ReadClusterStatus read_cluster(DiskCluster diskCluster, MemCluster memCluster);