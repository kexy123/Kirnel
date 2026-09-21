#include "double_fault.h"
#include "vga/out.h"

void double_fault_handle(InterruptCPUState *state)
{
    print_ln("DOUBLE FAULT! OH NO");
    print_ln(state->ErrorCode);
    while (1);
}