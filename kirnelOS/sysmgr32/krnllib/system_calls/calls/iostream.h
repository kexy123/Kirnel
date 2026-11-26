#pragma once

#include "interrupts/interrupts/interrupt_state.h"

/// @brief Puts a character out via the location as EBX and the length as ECX.
/// @param state The CPU state.
void charout(InterruptCPUState *state);
