#include "master.h"
#include "filesys/fat12/master.h"

const FileSystem *fileSystem;

void identify_file_system()
{
    // FAT-12 file system.
    fileSystem = &(FileSystem){
        .open_file = &fat12_open_file,
        .close_file = &fat12_close_file,
        
        .load_all = &fat12_load_all,
    };
}