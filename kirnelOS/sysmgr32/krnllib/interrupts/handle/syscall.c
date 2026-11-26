#include "memmgr/gdt/master.h"
#include "page_fault.h"
#include "utils/flow.h"
#include "system_calls/master.h"

void syscall_handle(InterruptCPUState *state)
{
    if (state->CS != userCodeSegment)
    {
        panic();
    }

    run_syscall(state);
}