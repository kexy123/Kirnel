#pragma once

#include "interrupts/interrupts/interrupt_state.h"

/// @brief A system call handle function.
typedef void (*SystemCallHandle)(InterruptCPUState *);

/// @brief Executes a system call from the information in the given interrupt CPU state.
/// @param state The interrupt CPU state to pass in the system call.
void run_syscall(InterruptCPUState *state);

/// @brief Initializes all the system calls.
void init_syscalls();
