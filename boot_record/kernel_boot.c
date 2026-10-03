#include "disksys/floppy/fdc.h"
#include "filesys/fat12/cluster.h"
#include "filesys/fat12/dir.h"
#include "interrupts/structure.h"
#include "memmgr/kernel_virtual_layout.h"
#include "memmgr/master.h"
#include "memmgr/paging/paging.h"
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

        void *location = allocate_strict((kernel.FileSize >> PAGE_SIZE_EXP) + 1);
        map(self, kernel_space, location, 0x10, 1, 0, 1, 1); // Dedicated kernel space.

        load_entire_entry(&kernel, (char *)location);
    }
}

/// @brief Loads the kernel into memory at 0xD000 to jump to later.
__attribute__((section(".text.krnl_boot")))
void krnl_boot(void)
{
    clear_screen();

    print_ln("Setting interrupts. . .");
    init_idt();

    print_ln("Setting disk. . .");
    init_fdc();

    print_ln("Initialising memory. . .");
    init_mem();

    print_ln("Locating kernel. . .");
    load_kernel();

    jump_next_stage();

    while (1)
        ;
}