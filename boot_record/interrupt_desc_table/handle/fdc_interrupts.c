#include "fdc_interrupts.h"

volatile _Bool fdc_op_complete = 0;

void fdc_interrupt_handle(InterruptCPUState *state)
{
    fdc_op_complete = 1;
}