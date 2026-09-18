#include "handles.h"
#include "vga/out.h"

void fdc_interrupt_handle(InterruptCPUState *state)
{
    print_ln("FDC interrupt handled!")
}

void double_fault_handle(InterruptCPUState *state)
{
    print_ln("DOUBLE FAULT! OH NO");
    print_ln(state->ErrorCode);
    while (1);
}