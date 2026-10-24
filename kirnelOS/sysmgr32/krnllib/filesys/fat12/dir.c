#include "dir.h"
#include "cluster.h"
#include "fat.h"
#include "filesys/master.h"
#include "memory/memcopy.h"
#include "memmgr/paging/paging.h"

/// @brief Compares if two entry names are equal in value.
/// @param a The first EntryName.
/// @param b The second EntryName.
/// @return 1 if they are equal; otherwise 0.
_Bool entries_are_equal(EntryName *a, EntryName *b)
{
    // Compare file extensions.
    for (int i = 0; i < ENTRY_FILE_EXT_BYTES; i++)
    {
        if (a->FileExtension[i] != b->FileExtension[i])
        {
            return 0;
        }
    }

    // Compare entry names.
    for (int i = 0; i < ENTRY_NAME_BYTES; i++)
    {
        if (a->Name[i] != b->Name[i])
        {
            return 0;
        }
    }

    return 1;
}

/// @brief Tries to find an entry in the memory cluster by EntryName.
/// @param cluster The MemCluster to search in.
/// @param entryCount The number of entries that this MemCluster has.
/// @param name The entry name to look for.
/// @param entryBuffer The Entry buffer that will be loaded onto when the entry is found.
/// @return The status of when the entry was found in the given cluster or not.
FindEntryStatus find_entry_in_memcluster(MemCluster cluster, unsigned short entryCount, EntryName *name, Entry *entryBuffer)
{
    Entry *entry = (Entry *)cluster;
    for (unsigned short i = 0; i < entryCount; i++)
    {
        if (entries_are_equal(name, &entry[i].Name))
        {
            copy_to(entry + i, entryBuffer, sizeof(Entry));
            return FINDENTRY_FOUND;
        }
    }

    return FINDENTRY_NOT_FOUND_IN_CLUSTER;
}

/// @brief Tries to find an entry in the given folder by EntryName.
/// @param folder The folder to search in.
/// @param name The entry name to look for.
/// @param entryBuffer The Entry buffer that will be loaded onto when the entry is found.
/// @return The status of when the entry was found or not.
FindEntryStatus find_entry_in_folder(Entry *folder, EntryName *name, Entry *entryBuffer)
{
    DiskCluster cluster = folder->FirstClusterLow;

    char buffer[BYTES_PER_CLUSTER];
    void *physicalBuffer;
    virt_to_phys((Address){.Address = (void *)buffer}, &physicalBuffer);

    NextClusterStatus nextCluster;
    do
    {
        // Check current cluster.
        ReadClusterStatus readStatus = read_cluster(cluster, (MemCluster)physicalBuffer);
        if (readStatus != CLUSTER_OK)
        {
            return FINDENTRY_INTERNAL_ERROR;
        }

        // Check if entry exists in that cluster.
        FindEntryStatus findEntry = find_entry_in_memcluster((MemCluster)buffer, bpbObject->SectorsPerCluster * bpbObject->BytesPerSector / sizeof(Entry), name, entryBuffer);
        if (findEntry == FINDENTRY_FOUND)
        {
            return FINDENTRY_FOUND;
        }

        // If not, go to next cluster.
        nextCluster = get_next_cluster(&cluster);
    } while (nextCluster == NEXTCLUSTER_OK);

    return FINDENTRY_NOT_FOUND;
}

/// @brief Tries to find an entry in the root directory by EntryName.
/// @param name The entry name to look for.
/// @param entryBuffer The Entry buffer that will be loaded onto when the entry is found.
/// @return The status of when the entry was found or not.
FindEntryStatus find_entry_in_root(EntryName *name, Entry *entryBuffer)
{
    char buffer[SECTORS_IN_ROOT * bpbObject->BytesPerSector];
    void *physicalBuffer;
    virt_to_phys((Address){.Address = (void *)buffer}, &physicalBuffer);

    // Read cluster, which is the entirety of the root directory.
    ReadClusterStatus readStatus = read_cluster(ROOT_CLUSTER, (MemCluster)physicalBuffer);
    if (readStatus != CLUSTER_OK)
    {
        return FINDENTRY_INTERNAL_ERROR;
    }

    // Check if the entry is in that cluster.
    FindEntryStatus findEntry = find_entry_in_memcluster((MemCluster)buffer, bpbObject->RootEntryCount, name, entryBuffer);
    if (findEntry == FINDENTRY_FOUND)
    {
        return FINDENTRY_FOUND;
    }

    return FINDENTRY_NOT_FOUND;
}

FindEntryStatus find_entry_by_path(FilePath path, Entry *entryBuffer)
{
    EntryName name;
    _Bool result = next_entry(path, &name, &path);

    if (!result)
    {
        return FINDENTRY_MALFORMED_PATH;
    };

    FindEntryStatus findEntry = find_entry_in_root(&name, entryBuffer);
    while (findEntry == FINDENTRY_FOUND && (result = next_entry(path, &name, &path)))
    {
        // TODO: Check if the entry is a folder.

        // Find next entry in folder.
        findEntry = find_entry_in_folder(entryBuffer, &name, entryBuffer);
        if (path == (const char *)0x00000000)
        {
            break;
        }
    }

    if (findEntry != FINDENTRY_FOUND)
    {
        return FINDENTRY_NOT_FOUND;
    }

    if (!result)
    {
        return FINDENTRY_MALFORMED_PATH;
    }

    return FINDENTRY_FOUND;
}

_Bool next_entry(const char *path, EntryName *entry, const char **nextPath)
{
    const char *endPoint = path, *extensionPoint = (const char *)0x00000000;
    for (unsigned long i = 0; i < 13; i++)
    {
        switch (path[i])
        {
        case '\0':
            *nextPath = (const char *)0x00000000;
            goto endEarly;

        case '\\':
        case '/':
            // This entry must be a directory.
            extensionPoint = (const char *)0x00000000;
            *nextPath = endPoint + 1;
            goto endEarly;

        case '.':
            // File extension.
            extensionPoint = endPoint + 1;
            break;

        default:
            break;
        }

        endPoint++;
    }

    return 0;

endEarly:
    byte_fill((void *)entry, sizeof(EntryName), ' '); // Empty characters must be spaces.
    if (extensionPoint)
    {
        if (endPoint - extensionPoint > 3)
        {
            return 0;
        }

        copy_to((void *)path, (void *)&(entry->Name), extensionPoint - path - 1); // Ignore the file extension period.
        copy_to((void *)extensionPoint, (void *)&(entry->FileExtension), endPoint - extensionPoint);
    }
    else if (endPoint == path)
    {
        // Simply equivalent to /./
        entry->Name[0] = '.';
    }
    else
    {
        if (endPoint - path > 8)
        {
            return 0;
        }

        copy_to((void *)path, (void *)(entry->Name), endPoint - path);
    }

    return 1;
}