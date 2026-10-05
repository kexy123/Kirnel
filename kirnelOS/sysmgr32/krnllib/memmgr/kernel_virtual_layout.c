#include "kernel_virtual_layout.h"
#include "paging/paging.h"

const Address oldKernelSpace = {.Raw = 0x00000000};

const Address kernelSpace = {.Raw = 0xC0000000};

const Address kernelVGA = {.Raw = 0xC00B8000};

const Address kernelStackStart = {.Raw = 0xDFFFE000};

const Address kernelStackEnd = {.Raw = 0xDFFFF000};

const Address kernelGDT = {.Raw = 0xDFFFF000};

const Address kernelPageAllocTree = {.Raw = 0xE0000000};

const Address kernelPageStart = {.Raw = 0xD0000000};

const Address kernelPagingDirectory = {.Raw = 0xFF7FEFFC};

/// @brief The dedicated location of where to assign pages at.
static Address kernelPages = (Address)(kernelPageStart);