#pragma once

#define ENTRY_NAME_BYTES (8)     // The maximum length that a file name can be.
#define ENTRY_FILE_EXT_BYTES (3) // The maximum length that a file extension can be.

/// @brief The naming format for an entry.
typedef struct __attribute__((packed))
{
    /// @brief The name of the file/folder, padded with trailing spaces.
    char Name[ENTRY_NAME_BYTES];

    /// @brief The extension type of the file, padded with trailing spaces.
    char FileExtension[ENTRY_FILE_EXT_BYTES];
} EntryName;

/// @brief The attributes for an entry.
typedef struct __attribute__((packed))
{
    /// @brief The entry is read-only.
    unsigned ReadOnly : 1;

    /// @brief The entry is hidden.
    unsigned Hidden : 1;

    /// @brief The entry is a system file.
    unsigned System : 1;

    /// @brief The entry is the volume label.
    unsigned VolumeID : 1;

    /// @brief The entry is a directory and its contents are other entries.
    unsigned Directory : 1;

    /// @brief The entry is archived.
    unsigned Archived : 1;

    /// @brief Reserved.
    unsigned LFNReserved : 2;
} EntryAttributes;

/// @brief Data format for a file/folder entry in the FAT12 file system.
typedef struct __attribute__((packed))
{
    /// @brief The name of the entry.
    EntryName Name;

    /// @brief The attributes of the entry.
    EntryAttributes Attributes;

    /// @brief Reserved.
    unsigned char NTReserved;

    /// @brief The time of creation of this file/folder in 10 ms units.
    unsigned char CreationTimeTenths;

    /// @brief The time of creation of this file/folder.
    unsigned short CreationTime;

    /// @brief The date of creation of this file/folder.
    unsigned short CreationDate;

    /// @brief The date of when the file/folder was last accessed.
    unsigned short LastAccessDate;

    /// @brief The high bits of the starting entry cluster of this file/folder.
    unsigned short FirstClusterHigh;

    /// @brief The last time of when this file/folder was written.
    unsigned short LastWriteTime;

    /// @brief The last date of when this file/folder was written.
    unsigned short LastWriteDate;

    /// @brief The low bits of the starting entry cluster of this file/folder.
    unsigned short FirstClusterLow;

    /// @brief The size of the file.
    unsigned long FileSize;
} Entry;

/// @brief The status when finding an entry in a folder; should be FINDENTRY_FOUND.
typedef enum
{
    FINDENTRY_FOUND = 0x0,

    /// @brief The entry was not found anywhere in the folder.
    FINDENTRY_NOT_FOUND = 0x1,

    /// @brief The entry was not found in the cluster. Note that this doesn't immediately mark the entry as nonexistent in the folder.
    FINDENTRY_NOT_FOUND_IN_CLUSTER = 0x2,

    /// @brief An internal error occurred while trying to find an entry.
    FINDENTRY_INTERNAL_ERROR = 0x3
} FindEntryStatus;

/// @brief Finds an entry by an array of EntryNames forming an absolute path.
/// @param path The absolute path which is an array of EntryNames.
/// @param entryBuffer The Entry buffer that will be loaded onto when the entry is found; otherwise returns the last common directory.
/// @return The status of when the entry was found or not.
FindEntryStatus find_entry_by_path(EntryName *path, Entry *entryBuffer);