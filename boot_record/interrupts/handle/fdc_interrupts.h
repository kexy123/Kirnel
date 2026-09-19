#pragma once

#include "interrupts/interrupts/interrupt_state.h"

/// @brief A flag; is used to determine when the FDC operation is complete and raised an IRQ6.
extern volatile _Bool fdc_op_complete;

/// @brief The handle for floppy disk interrupts (IRQ6).
void fdc_interrupt_handle(InterruptCPUState *state);