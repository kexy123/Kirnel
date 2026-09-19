#pragma once

#define INTERRUPT_REQUEST_START (0x20) // The starting interrupt request vector.

/// @brief Specification of an [interrupt descriptor table entry](https://wiki.osdev.org/Interrupt_Descriptor_Table#Gate_Descriptor).
typedef struct __attribute__((packed))
{
    /// @brief The low bits of the address to the entry point for when the interrupt is called.
    unsigned short OffsetLow;

    /// @brief The [segment selector](https://wiki.osdev.org/Segment_Selector) to use when accessing the address.
    unsigned short SegmentSelector;

    /// @brief Reserved; always 0.
    unsigned char Reserved;

    /// @brief The present bit, privilege level, unset bit, and gate type.
    unsigned char Flags;

    /// @brief The high bits of the address to the entry point for when the interrupt is called.
    unsigned short OffsetHigh;
} IDTEntry;

/// @brief The descriptor for the interrupt descriptor table.
typedef struct __attribute__((packed))
{
    /// @brief The size of the interrupt descriptor table.
    unsigned short Limit;

    /// @brief The starting address of the interrupt descriptor table.
    unsigned long Base;
} IDTDescriptor;

typedef void (*InterruptFunction)(void);

/// @brief The array of interrupt descriptor table entries.
extern IDTEntry idtTable[256];

/// @brief The assigned interrupt descriptor table descriptor.
extern IDTDescriptor *descriptor;

/// @brief Initiates the interrupt descriptor table.
void init_idt();