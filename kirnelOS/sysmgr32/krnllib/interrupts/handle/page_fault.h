#pragma once

#include "interrupts/interrupts/interrupt_state.h"

/// @brief The handle for page faults.
void page_fault_handle(InterruptCPUState *state);