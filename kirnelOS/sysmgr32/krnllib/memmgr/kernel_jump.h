#pragma once

/// @brief Jumps the kernel to its dedicated kernel space. This should only be used once.
__attribute__((__cdecl__)) void kernel_jump();
