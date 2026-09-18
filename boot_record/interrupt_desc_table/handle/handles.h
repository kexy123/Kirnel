#pragma once

#include "interrupt_desc_table/interrupts/interrupt_state.h"

/// @brief The handle for floppy disk interrupts (IRQ 6).
void fdc_interrupt_handle(InterruptCPUState *state);

/// @brief The handle for double faults.
void double_fault_handle(InterruptCPUState *state);