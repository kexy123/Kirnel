#pragma once

/// @brief The kernel code segment.
extern const unsigned short kernel_code_segmemt;

/// @brief The kernel data segment.
extern const unsigned short kernel_data_segmemt;

/// @brief The user code segment.
extern const unsigned short user_code_segmemt;

/// @brief The user data segment.
extern const unsigned short user_data_segmemt;

/// @brief Creates and initialises the global descriptor table for this operating system.
void create_os_gdt();