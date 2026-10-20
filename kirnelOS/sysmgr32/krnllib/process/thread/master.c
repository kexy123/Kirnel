#include "master.h"
#include "process/master.h"
#include "memmgr/gdt/master.h"
#include "memory/memory.h"

/// @brief Switches to the given thread via the given CPU context.
/// @param directory The page directory to switch to.
/// @param context The CPU context to switch to.
extern void context_switch(PageDirectory *directory, CPUContext context);

Thread *create_thread(Process *parentProcess, void *atInstruction, void *stack)
{
    Thread *thread = (Thread *)calloc(sizeof(Thread));

    thread->Master = parentProcess;
    thread->State = THREAD_STATE_READY;
    thread->Context = (CPUContext){
        .CS = userCodeSegment,

        .DS = userDataSegment,
        .ES = userDataSegment,
        .FS = userDataSegment,
        .GS = userDataSegment,
        .SS = userDataSegment,

        .EFLAGS = (EFlags){
            .InterruptEnabledFlag = 1,
        },

        .ESP = stack,
        .EBP = stack,

        .EIP = atInstruction,
    };

    return thread;
}

void run_thread(Thread *thread)
{
    if (thread->State & THREAD_STATE_NOT_RUNNABLE)
    {
        return;
    }

    context_switch(thread->Master->PageDirectory, thread->Context);
}
