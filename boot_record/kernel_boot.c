#include "boot/in.h"
#include "boot/out.h"
#include "filesys/fat12/cluster.h"
#include "filesys/fat12/dir.h"
#include "kernel_boot.h"

void krnl_boot(void)
{
    print("Enter key to continue: ");
    read_char();

    print_newl();

    load_kernel();

    return;
}

void load_kernel()
{
    // Read from FAT12 the kirnelOS/sysmgr32/krnlload.bin file.
    const char *kernelPath = "KIRNELOS   SYSMGR32   KRNLLOADBIN";

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
}