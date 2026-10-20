#include "bit.h"
#include "filesys/master.h"
#include "master.h"
#include "memmgr/kernel_virtual_layout.h"
#include "memmgr/paging/page_alloc.h"
#include "memmgr/paging/paging.h"
#include "memory/memory.h"

Process *create_process(FilePath executable)
{
    static unsigned long currentProcID = 0; // Note that currentProcID will start at 1.

    Process *newProcess = calloc(sizeof(Process));

    // Open file.
    FileInfo program = fileSystem->open_file(executable, Read);
    if (!program.Success)
    {
        return (Process *)0x00000000;
    }

    // Load then close file.
    unsigned long numPages = (program.FileSize >> PAGE_SIZE_EXP) + 1, remainder;
    void *location = allocate_strict(numPages, &remainder);
    _Bool success = fileSystem->load_all(program, (char *)location);

    fileSystem->close_file(program);
    if (!success)
    {
        deallocate(lowest_exp2(numPages), location);
        return (Process *)0x00000000;
    }

    // Create primary thread.
    newProcess->MainThread = create_thread(newProcess, (void *)__CODE_SPACE, (void *)__STACK_END);

    // Create page directory for this process.
    PageDirectory *directory = create_directory();
    newProcess->PageDirectory = directory;

    // Map the code space.
    switch_page(directory);
    map(self, (Address){.Raw = __CODE_SPACE}, location, numPages + remainder, 0, 1, 0);
    switch_page((PageDirectory *)kernelPagingDirectory.Address); // TODO: Make this efficient?

    currentProcID++;
    newProcess->ProcessID = currentProcID;

    return newProcess;
}
