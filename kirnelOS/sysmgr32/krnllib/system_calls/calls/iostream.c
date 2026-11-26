#include "iostream.h"
#include "interrupts/interrupts/interrupt_state.h"
#include "vga/out.h"

void charout(InterruptCPUState *state)
{
    StringView str = {
        .Length = state->ECX,
        .String = (const char *)state->EBX,
    };

    print(str);
}