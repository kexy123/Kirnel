#pragma once

#define INTERRUPT_REQUEST_START (0x20) // The starting interrupt request vector.

/// @brief The IDT entry flags.
typedef union
{
    /// @brief The raw bits of the IDT flags.
    unsigned char Raw;

    struct __attribute__((packed))
    {
        /// @brief The gate type.
        unsigned GateType : 4;

        /// @brief Reserved; always 0.
        unsigned ZeroReserved : 1;

        /// @brief The CPU privilege level of this interrupt.
        unsigned PrivilegeLevel : 2;

        /// @brief The interrupt is present. Must be set in order to be used.
        unsigned Present : 1;
    };
} IDTEntryFlags;

/// @brief Specification of an [interrupt descriptor table entry](https://wiki.osdev.org/Interrupt_Descriptor_Table#Gate_Descriptor).
typedef struct __attribute__((packed))
{
    /// @brief The low bits of the address to the entry point for when the interrupt is called.
    unsigned short OffsetLow;

    /// @brief The [segment selector](https://wiki.osdev.org/Segment_Selector) to use when accessing the address.
    unsigned short SegmentSelector;

    /// @brief Reserved; always 0.
    unsigned char Reserved;

    /// @brief The flags of the IDT entry.
    IDTEntryFlags Flags;

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
extern IDTDescriptor interruptDescriptor;

/// @brief Initiates the interrupt descriptor table.
void init_idt();