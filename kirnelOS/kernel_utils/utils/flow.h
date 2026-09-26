#pragma once

/// @brief Halts the CPU thread until an interrupt is raised.
extern void halt();

/// @brief Halts the CPU permanently as the kernel has reached an unrecoverable state.
extern void panic();