#include "page_fault.h"
#include "vga/out.h"

void page_fault_handle(InterruptCPUState *state)
{
    print("(STOP) Page fault at: ");
    print_uintx(state->CR2);
    print_newl();

    while (1);
}