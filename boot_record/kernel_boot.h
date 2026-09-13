#pragma once

/// @brief The starting method upon boot being initiated by the boot sector.
void krnl_boot(void);

/// @brief Loads the kernel into memory at 0xD000 to jump to later.
void load_kernel();