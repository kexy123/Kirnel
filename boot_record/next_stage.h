#pragma once

/// @brief Jumps to the next stage of the boot loader.
[[noreturn]]
__attribute__((__cdecl__)) extern void jump_next_stage();
