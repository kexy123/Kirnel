#pragma once

/// @brief Halts the CPU thread until an interrupt is raised.
__attribute__((__cdecl__)) extern void halt();

/// @brief Halts the CPU permanently as the kernel has reached an unrecoverable state.
[[noreturn]]
__attribute__((__cdecl__)) extern void panic();
