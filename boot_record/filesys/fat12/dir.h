#pragma once

#define ENTRY_NAME_BYTES 8
#define ENTRY_FILE_EXT_BYTES 3

/// @brief The naming format for an entry.
typedef struct __attribute((packed))
{
    /// @brief The name of the file/folder, padded with trailing spaces.
    char Name[ENTRY_NAME_BYTES];

    /// @brief The extension type of the file, padded with trailing spaces.
    char FileExtension[ENTRY_FILE_EXT_BYTES];
} EntryName;

/// @brief Data format for a file/folder entry in the FAT12 file system.
typedef struct __attribute__((packed))
{
    /// @brief The name of the entry.
    EntryName Name;

    /// @brief The attributes of this file/folder.
    unsigned char Attributes;

    /// @brief Reserved.
    unsigned char Reserved;

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
    unsigned int FileSize;
} Entry;