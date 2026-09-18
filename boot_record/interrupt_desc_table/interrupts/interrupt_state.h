#pragma once

/// @brief The stored CPU state upon an interrupt.
typedef struct __attribute__((packed))
{
    /// @brief The [second control register](https://en.wikipedia.org/wiki/Control_register#CR2). Is used to store the address of where a page fault occurred if existing.
    unsigned long CR2;

    /// @brief The data segment.
    unsigned long DS;

    /// @brief The stored register of the CPU state.
    unsigned long EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX;

    /// @brief The interrupt code.
    unsigned long InterruptCode;

    /// @brief The error code if given.
    unsigned long ErrorCode;

    // EIP      - the return address.
    // CS       - the code segment selector to return to.
    // EFLAGS   - the stored CPU flags.
    // USERESP  - user stack pointer if the interrupt came from stack mode.
    // SS       - user stack segment selector if the interrupt came from stack mode.

    /// @brief Automatic state given by the CPU upon an interrupt.
    unsigned long EIP, CS, EFLAGS, USER_ESP, SS;
} InterruptCPUState;

/// @brief An interrupt handle function.
typedef void (*InterruptHandle)(InterruptCPUState *);

/// @brief The common interrupt handler. Handles function pointers to correct locations.
/// @param state The stored CPU state from the interrupt.
void service_handle(InterruptCPUState *state);

/// @brief Installs an interrupt handle at the given interrupt code.
/// @param interruptCode The location to install the interrupt handle at.
/// @param handle The interrupt handle.
void install_handle(unsigned char interruptCode, InterruptHandle handle);