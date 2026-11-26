#pragma once

#include "interrupts/interrupts/interrupt_state.h"

/// @brief The handle for system calls from user programs.
void syscall_handle(InterruptCPUState *state);
