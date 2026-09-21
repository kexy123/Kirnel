#include "interrupts/structure.h"
#include "interrupt_state.h"
#include "utils/portcall.h"

InterruptHandle handles[256] = {0};

void service_handle(InterruptCPUState *state)
{
    InterruptHandle handle = handles[state->InterruptCode];
    if (handle)
    {
        handle(state);
    }

    // Check if the interrupt code is an interrupt request so that we can tell the PIC that the interrupt has been handled.
    if (state->InterruptCode >= INTERRUPT_REQUEST_START && state->InterruptCode < INTERRUPT_REQUEST_START + 0x10)
    {
        port_call(0x20, 0x20);
    }
}

void install_handle(unsigned char interruptCode, InterruptHandle handle)
{
    handles[interruptCode] = handle;
}