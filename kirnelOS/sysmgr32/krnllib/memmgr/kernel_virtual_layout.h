#pragma once

#include "paging/paging.h"

#define KERNEL_DIRECTORY (*(PageDirectory **)kernelPagingDirectory.Address) // The physical location of the kernel page directory.

/// @brief The old location of the kernel space. Is only used to be transferred to the dedicated kernel space.
extern const Address oldKernelSpace;

/// @brief The dedicated space of the kernel.
extern const Address kernelSpace;

/// @brief The dedicated location of the VGA.
extern const Address kernelVGA;

/// @brief The start (bottom) of the kernel stack space for kernel booting and the first kernel thread.
extern const Address kernelStack1Start;

/// @brief The end (top) of the kernel stack space for kernel booting and the first kernel thread.
extern const Address kernelStack1End;

/// @brief The dedicated location of the global descriptor table. Only has one page.
extern const Address kernelGDT;

/// @brief The dedicated space of the allocation tree.
extern const Address kernelPageAllocTree;

/// @brief The dedicated global location of the physical address to the page directory for the kernel.
extern const Address kernelPagingDirectory;

/// @brief The dedicated location of the paging buffer for when the kernel needs to perform paging operations.
extern const Address kernelPagingBuffer;
