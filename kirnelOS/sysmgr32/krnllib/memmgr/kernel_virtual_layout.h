#pragma once

#include "paging/paging.h"

/// @brief The old location of the kernel space. Is only used to be transferred to the dedicated kernel space.
extern const Address oldKernelSpace;

/// @brief The dedicated space of the kernel.
extern const Address kernelSpace;

/// @brief The dedicated location of the VGA.
extern const Address kernelVGA;

/// @brief The start (bottom) of the kernel stack space.
extern const Address kernelStackStart;

/// @brief The end (top) of the kernel stack space.
extern const Address kernelStackEnd;

/// @brief The dedicated location of the global descriptor table. Only has one page.
extern const Address kernelGDT;

/// @brief The dedicated space of the allocation tree.
extern const Address kernelPageAllocTree;

/// @brief The dedicated global location of the physical address to the page directory for the kernel.
extern const Address kernelPagingDirectory;

/// @brief The dedicated space of page allocation for the kernel.
extern const Address kernelPageStart;