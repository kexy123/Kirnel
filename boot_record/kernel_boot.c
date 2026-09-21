#include "vga/out.h"
#include "disksys/floppy/fdc.h"
#include "filesys/fat12/cluster.h"
#include "filesys/fat12/dir.h"
#include "interrupts/structure.h"
#include "kernel_boot.h"
#include "utils/next_stage.h"

void krnl_boot(void)
{
    init_idt();
    init_fdc();

    print_ln("Locating kernel. . .");
    load_kernel();

    jump_next_stage();

    while (1)
        ;
}

void load_kernel()
{
    // Read from FAT12 the kirnelOS/sysmgr32/syskrnl.bin file.
    const char *kernelPath = "KIRNELOS   SYSMGR32   SYSKRNL BIN";

    Entry kernel;
    FindEntryStatus findEntry = find_entry_by_path((EntryName *)kernelPath, &kernel);
    if (findEntry != FINDENTRY_FOUND)
    {
        print_ln("The kernel loader could not be found.");
        while (1)
            ;
    }
    else
    {
        print_ln("Kernel found.");
        char *kernel_location = (char *)0xD000;
        load_entire_entry(&kernel, kernel_location);
    }
}