#pragma once

/// @brief The kernel code segment.
extern unsigned short kernelCodeSegment;

/// @brief The kernel data segment.
extern unsigned short kernelDataSegment;

/// @brief The user code segment.
extern unsigned short userCodeSegment;

/// @brief The user data segment.
extern unsigned short userDataSegment;

/// @brief Creates and initialises the global descriptor table for this operating system.
void create_os_gdt();

/// @brief Configures the variables from the existing global descriptor table.
void check_os_gdt();