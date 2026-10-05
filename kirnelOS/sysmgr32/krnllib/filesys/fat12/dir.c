#include "dir.h"
#include "cluster.h"
#include "fat.h"
#include "memory/memcopy.h"

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
            copy_to(entryBuffer, entry + i, sizeof(Entry));
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
    MemCluster memCluster = buffer;

    NextClusterStatus nextCluster;
    do
    {
        // Check current cluster.
        ReadClusterStatus readStatus = read_cluster(cluster, memCluster);
        if (readStatus != CLUSTER_OK)
        {
            return FINDENTRY_INTERNAL_ERROR;
        }

        // Check if entry exists in that cluster.
        FindEntryStatus findEntry = find_entry_in_memcluster(memCluster, BPB->SectorsPerCluster * BPB->BytesPerSector / sizeof(Entry), name, entryBuffer);
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
    char buffer[SECTORS_IN_ROOT * BPB->BytesPerSector];
    MemCluster memCluster = buffer;

    // Read cluster, which is the entirety of the root directory.
    ReadClusterStatus readStatus = read_cluster(ROOT_CLUSTER, memCluster);
    if (readStatus != CLUSTER_OK)
    {
        return FINDENTRY_INTERNAL_ERROR;
    }

    // Check if the entry is in that cluster.
    FindEntryStatus findEntry = find_entry_in_memcluster(memCluster, BPB->RootEntryCount, name, entryBuffer);
    if (findEntry == FINDENTRY_FOUND)
    {
        return FINDENTRY_FOUND;
    }

    return FINDENTRY_NOT_FOUND;
}

FindEntryStatus find_entry_by_path(EntryName *path, Entry *entryBuffer)
{
    FindEntryStatus findEntry = find_entry_in_root(path, entryBuffer);

    path++;
    while (path->Name[0] != '\0')
    {
        if (findEntry != FINDENTRY_FOUND)
        {
            return FINDENTRY_NOT_FOUND;
        }

        // TODO: Check if the entry is a folder.

        // Find next entry in folder.
        findEntry = find_entry_in_folder(entryBuffer, path, entryBuffer);
        path++;
    }

    if (findEntry != FINDENTRY_FOUND)
    {
        return FINDENTRY_NOT_FOUND;
    }

    return FINDENTRY_FOUND;
}