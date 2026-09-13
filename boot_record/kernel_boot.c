#include "vga/out.h"
#include "filesys/fat12/cluster.h"
#include "filesys/fat12/dir.h"
#include "kernel_boot.h"

void krnl_boot(void)
{
    print_ln("Locating kernel. . .");
    // load_kernel();

    while (1)
        ;
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