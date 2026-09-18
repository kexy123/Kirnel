#pragma once

#include "interrupt_desc_table/interrupts/interrupt_state.h"

/// @brief The handle for double faults.
void double_fault_handle(InterruptCPUState *state);