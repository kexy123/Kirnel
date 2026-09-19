#include "fdc_interrupts.h"
#include "vga/out.h"

volatile _Bool fdc_op_complete = 0;

void fdc_interrupt_handle(InterruptCPUState *state)
{
    print_ln("FDC interrupt handled!")
    fdc_op_complete = 1;
}