#include "memmgr/gdt/master.h"
#include "memmgr/kernel_virtual_layout.h"
#include "memmgr/paging/paging.h"
#include "page_fault.h"
#include "utils/flow.h"

void page_fault_handle(InterruptCPUState *state)
{
    if (state->CS == kernelCodeSegment)
    {
        switch_page(KERNEL_DIRECTORY);
        map_to_free(selfMemorySpace, (Address){.Raw = state->CR2}, 1, 1, 0, state->CR2 >= kernelSpace.Raw);
        return;
    }

    if (state->CS == userCodeSegment)
    {
        if (state->CR2 < kernelSpace.Raw)
        {
            check_paging();
            map_to_free(selfMemorySpace, (Address){.Raw = state->CR2}, 1, 1, 1, 0);
        }
        else
        {
            // Segmentation fault, so dump user process.
        }

        return;
    }

    panic();
}