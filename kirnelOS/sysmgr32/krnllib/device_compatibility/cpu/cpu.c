#include "cpu.h"

static _Bool hasCPUID = 0;

/// @brief Gets the CPUID bitset of features from the given register type.
/// @param registerType The register to read from. 0 for EBX, 1 for ECX, and 2 for EDX.
/// @return The bitset of the specified register.
extern unsigned long get_cpuid_feature(unsigned long registerType);

/// @brief Checks if the CPU contains a CPUID.
/// @return True if the CPU contains a CPUID; otherwise false.
extern _Bool has_cpuid();

_Bool check_cpuid()
{
    hasCPUID = has_cpuid();
    return hasCPUID;
}

_Bool has_feature(CPUIDFeature feature)
{
    if (!hasCPUID)
    {
        return 0;
    }

    unsigned long registerType = feature / 0x20;
    unsigned long bit = feature % 0x20;

    unsigned long bitset = get_cpuid_feature(registerType);
    return bitset & (1UL << bit) ? 1 : 0;
}