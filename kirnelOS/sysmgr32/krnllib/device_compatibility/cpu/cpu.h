#pragma once

/// @brief A feature to check from the CPUID.
typedef enum : unsigned long
{
    // Note that EBX is 0x00 - 0x1F, ECX is 0x20 - 0x3F, and EDX is 0x40 - 0x5F.

    EDX0 = 0x2F,

    /// @brief Checks if global paging is available.
    GlobalPageFeature = 0x4D, // EDX[13]
} CPUIDFeature;

/// @brief Checks if the CPU contains a CPUID and sets hasCPUID.
extern _Bool check_cpuid();

/// @brief Checks if the CPU has a specified feature enabled.
/// @param feature The CPUID feature to check.
/// @return True if the feature exists and is enabled; otherwise false.
_Bool has_feature(CPUIDFeature feature);