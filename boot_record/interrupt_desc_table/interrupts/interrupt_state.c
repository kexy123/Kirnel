#include "interrupt_state.h"

InterruptHandle handles[256] = {0};

void service_handle(InterruptCPUState *state)
{
    InterruptHandle handle = handles[state->InterruptCode];
    if (handle)
    {
        handle(state);
    }
}

void install_handle(unsigned char interruptCode, InterruptHandle handle)
{
    handles[interruptCode] = handle;
}