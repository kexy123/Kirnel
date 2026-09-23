#pragma once

/// @brief List of memory segment types.
typedef enum : unsigned long
{
    /// @brief The memory is usable to code in the operating system.
    Usable = 1,

    /// @brief The memory is reserved to hardware components.
    Reserved = 2,

    /// @brief Memory that can be used after the [ACPI](https://en.wikipedia.org/wiki/ACPI) reads them.
    ACPIReclaimable = 3,

    /// @brief Memory that must be preserved by the [ACPI](https://en.wikipedia.org/wiki/ACPI).
    ACPINVS = 4,

    /// @brief Unusuable memory and must be avoided.
    Unusable = 5,

    /// @brief Memory that must be accepted before use.
    Unaccepted = 6
} MemorySegmentType;

/// @brief A memory segment entry.
typedef struct __attribute__((packed))
{
    /// @brief The starting address of the segment entry.
    unsigned long long BaseAddress;

    /// @brief The length of the segment.
    unsigned long long SegmentLength;

    /// @brief The memory segment type.
    MemorySegmentType RegionType;

    /// @brief Extended attributes by the [ACPI](https://en.wikipedia.org/wiki/ACPI).
    unsigned long ExtendedAttributes;
} MemorySegmentEntry;