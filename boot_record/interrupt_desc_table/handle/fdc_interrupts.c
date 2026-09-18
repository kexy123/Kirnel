#include "fdc_interrupts.h"
#include "vga/out.h"

void fdc_interrupt_handle(InterruptCPUState *state)
{
    print_ln("FDC interrupt handled!")
}