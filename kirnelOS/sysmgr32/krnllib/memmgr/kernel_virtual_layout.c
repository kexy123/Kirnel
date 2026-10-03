#include "kernel_virtual_layout.h"
#include "paging/paging.h"

const Address old_kernel_space = {.Raw = 0x00000000};

const Address kernel_space = {.Raw = 0xC0000000};

const Address kernel_gdt = {.Raw = 0xDFFFF000};

const Address kernel_page_alloc_tree = {.Raw = 0xE0000000};

const Address kernel_page_start = {.Raw = 0xD0000000};

/// @brief The dedicated location of where to assign pages at.
static Address kernel_pages = (Address)(kernel_page_start);