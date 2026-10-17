#pragma once

#include "filesys/master.h"

FileInfo fat12_open_file(FilePath path, FileUsagePermissions permissions);

void fat12_close_file(FileInfo file);

_Bool fat12_load_all(FileInfo file, char *buffer);