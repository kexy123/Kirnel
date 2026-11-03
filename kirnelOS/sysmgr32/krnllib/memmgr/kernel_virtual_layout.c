#include "kernel_virtual_layout.h"
#include "paging/paging.h"

const Address oldKernelSpace = {.Raw = 0x00000000};

const Address kernelSpace = {.Raw = 0xC0000000};

const Address kernelVGA = {.Raw = 0xC00B8000};

const Address kernelHeapStart = {.Raw = KERNEL_HEAP_START};

const Address kernelHeapEnd = {.Raw = KERNEL_HEAP_END};

const Address kernelStack1Start = {.Raw = 0xDFFF0000};

const Address kernelStack1End = {.Raw = 0xDFFFF000};

const Address kernelGDT = {.Raw = 0xDFFFF000};

const Address kernelPageAllocTree = {.Raw = 0xE0000000};

const Address kernelPagingDirectory = {.Raw = 0xFF7FDFFC};

const Address kernelPagingBuffer = {.Raw = 0xFF7FE000};
