#pragma once

#include "paging/paging.h"

/// @brief The old location of the kernel space. Is only used to be transferred to the dedicated kernel space.
extern const Address old_kernel_space;

/// @brief The dedicated space of the kernel.
extern const Address kernel_space;

/// @brief The dedicated location of the global descriptor table. Only has one page.
extern const Address kernel_gdt;

/// @brief The dedicated space of the allocation tree.
extern const Address kernel_page_alloc_tree;

/// @brief The dedicated space of page allocation for the kernel.
extern const Address kernel_page_start;