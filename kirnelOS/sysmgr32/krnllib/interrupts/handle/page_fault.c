#include "memmgr/gdt/master.h"
#include "memmgr/kernel_virtual_layout.h"
#include "memmgr/paging/paging.h"
#include "page_fault.h"
#include "utils/flow.h"

void page_fault_handle(InterruptCPUState *state)
{
    if (state->CS == kernelCodeSegment)
    {
        switch_page(*(PageDirectory **)kernelPagingDirectory.Address);
        map_to_free(self, (Address){.Raw = state->CR2}, 1, 1, 0, 1);
    }
    else
    {
        panic();
    }
}