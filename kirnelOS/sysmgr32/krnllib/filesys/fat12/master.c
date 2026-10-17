#include "cluster.h"
#include "filesys/master.h"
#include "master.h"
#include "memory/memory.h"
#include "string.h"

FileInfo fat12_open_file(FilePath path, FileUsagePermissions permissions)
{
    Entry *entry = (Entry *)malloc(sizeof(Entry));
    FindEntryStatus status = find_entry_by_path(path, entry);

    if (status != FINDENTRY_FOUND)
    {
        free(entry);
        return (FileInfo){
            .Success = 0,
        };
    }

    return (FileInfo){
        .Success = 1,

        .Path = path,
        .Name = dupe_string(entry->Name.Name, 8),
        .Extension = dupe_string(entry->Name.FileExtension, 3),

        .Permissions = permissions,

        .FileSize = entry->FileSize,

        .Internal = (void *)entry,
    };
}

void fat12_close_file(FileInfo file)
{
    if (!file.Success)
    {
        return;
    }

    destroy_string(file.Name);
    destroy_string(file.Extension);

    free(file.Internal);
}

_Bool fat12_load_all(FileInfo file, char *buffer)
{
    load_entire_entry((const Entry *)file.Internal, buffer);
}