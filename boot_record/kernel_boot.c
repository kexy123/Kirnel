#include "boot/in.h"
#include "boot/out.h"
#include "filesys/fat12/cluster.h"
#include "filesys/fat12/dir.h"

/// @brief The starting method upon boot being initiated by the boot sector.
void krnl_boot(void)
{
    print("Enter key to continue: ");
    read_char();

    print_newl();

    // TODO: Read from FAT12 the kirnelOS/sysmgr/krnlload.bin file.
    const char *kernelPath = "KIRNELOS   SYSMGR     KRNLLOADBIN";

    Entry kernel;
    FindEntryStatus findEntry = find_entry_by_path((EntryName *)kernelPath, &kernel);
    if (findEntry != FINDENTRY_FOUND)
    {
        print_ln("The kernel loader could not be found.");
    }
    else
    {
        print_ln("Kernel found.");
        char *kernel_location = (char *)0xD000;
        load_entire_entry(&kernel, kernel_location);
    }

    return;
}