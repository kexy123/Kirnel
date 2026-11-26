#pragma once

#include "string.hpp"

/// @brief The permissions of using a file.
typedef enum : unsigned char
{
    /// @brief No permissions are given.
    None = 0x00,

    /// @brief The file can only be read to.
    Read = 0x01,

    /// @brief The file can only be written to.
    Write = 0x02,

    /// @brief The file can be read and written to.
    ReadWrite = 0x03,
} FileUsagePermissions;

/// @brief The path to a file.
typedef const char *FilePath;

/// @brief The info of the File.
typedef struct
{
    /// @brief The file exists. If not, this FileInfo should not be used.
    _Bool Success;

    /// @brief The full path to the file.
    StringView Path;

    /// @brief The name of the file.
    StringView Name;

    /// @brief The file's extension.
    StringView Extension;

    /// @brief The size of the file in bytes.
    unsigned long FileSize;

    /// @brief The permissions of the usage of the file.
    FileUsagePermissions Permissions;

    /// @brief Internal data for the FileSystem.
    const void *Internal;
} FileInfo;

/// @brief The interface for functions pertaining to the file system.
typedef struct
{
    /// @brief Gets a file from the given file path.
    /// @param path The file path.
    /// @param permissions The permissions of using the file.
    /// @return The file information.
    FileInfo (*open_file)(FilePath path, FileUsagePermissions permissions);

    /// @brief Closes a file.
    /// @param file The file to close.
    void (*close_file)(FileInfo file);

    /// @brief Loads the entire file onto a contiguous buffer.
    /// @param file The file to open.
    /// @param buffer The physical address of buffer to load the entire file onto.
    /// @return True if the file can be read to; otherwise false.
    _Bool (*load_all)(FileInfo file, char *buffer);
} FileSystem;

/// @brief The file system interface object.
extern const FileSystem *fileSystem;

/// @brief Identifies what file system the operating system is in and prepares the fileSystem object.
void identify_file_system();