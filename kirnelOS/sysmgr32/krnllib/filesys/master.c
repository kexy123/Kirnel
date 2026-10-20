#include "master.h"
#include "filesys/fat12/master.h"

/// @brief The primary FileSystem object.
static FileSystem privateFileSystem = {};

const FileSystem *fileSystem = (const FileSystem *)&privateFileSystem;

void identify_file_system()
{
    // FAT-12 file system.
    privateFileSystem.open_file = &fat12_open_file;
    privateFileSystem.close_file = &fat12_close_file;

    privateFileSystem.load_all = &fat12_load_all;
}