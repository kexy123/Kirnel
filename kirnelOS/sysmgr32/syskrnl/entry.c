#include "interrupts/master.h"
#include "memmgr/master.h"
#include "vga/out.h"
#include "filesys/master.h"
#include "process/master.h"
#include "process/task/master.h"
#include "system_calls/master.h"

/// @brief The entry point for the system kernel.
__attribute__((section(".text.krnl_start"))) void krnl_start()
{
    finalize_mem();
    print_ln("Memory finalized.");

    init_idt();
    print_ln("Interrupts created.");

    identify_file_system();
    print_ln("File system layer created.");

    init_tss();
    print_ln("Scheduler initialized.");

    init_syscalls();
    print_ln("System calls initialized.");
    
    print_ln("Hello from the SYSKRNL!");

    FilePath path = "KIRNELOS/SYSMGR32/SHELL.BIN";
    Process *process = create_process(path);

    run_thread(process->MainThread);

    while (1)
        ;
}