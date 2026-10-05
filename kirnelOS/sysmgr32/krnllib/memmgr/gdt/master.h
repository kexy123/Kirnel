#pragma once

/// @brief The kernel code segment.
extern const unsigned short kernelCodeSegment;

/// @brief The kernel data segment.
extern const unsigned short kernelDataSegment;

/// @brief The user code segment.
extern const unsigned short userCodeSegment;

/// @brief The user data segment.
extern const unsigned short userDataSegment;

/// @brief Creates and initialises the global descriptor table for this operating system.
void create_os_gdt();