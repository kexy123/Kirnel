#include "disksys/floppy/fdc.h"
#include "filesys/fat12/cluster.h"
#include "filesys/fat12/dir.h"
#include "interrupts/structure.h"
#include "next_stage.h"
#include "vga/out.h"

/// @brief The starting method upon boot being initiated by the boot sector.
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

/// @brief Loads the kernel into memory at 0xD000 to jump to later.
__attribute__((section(".text.krnl_boot")))
void krnl_boot(void)
{
    print_ln("Setting interrupts. . .");
    init_idt();

    print_ln("Setting disk. . .");
    init_fdc();

    print_ln("Locating kernel. . .");
    load_kernel();

    jump_next_stage();

    while (1)
        ;
}