#include "calls.h"
#include "master.h"

/// @brief The array of call handles.
static SystemCallHandle callHandles[256];

void install_syscall(unsigned long callCode, SystemCallHandle handle)
{
    callHandles[callCode] = handle;
}

void run_syscall(InterruptCPUState *state)
{
    // The EAX register is the call code.
    SystemCallHandle syscall = callHandles[state->EAX];
    if (syscall)
    {
        syscall(state);
    }
}

void init_syscalls()
{
    install_syscall(1, charout);
}
