#include "master.h"
#include "interrupts/interrupts.h"
#include "interrupts/interrupt_state.h"
#include "structure.h"
#include "utils/portcall.h"

/// @brief Loads the given interrupt descriptor table with the given address to the descriptor.
/// @param descriptor The address of the IDTDescriptor.
extern void load_idt(IDTDescriptor *descriptor);

void init_idt()
{
    interruptDescriptor.Limit = sizeof(IDTEntry) * 256 - 1;
    interruptDescriptor.Base = (unsigned long)&idtTable;

    // Initialise the Programmable Interrupt Controller for hardware interrupts.
    // Specification: https://wiki.osdev.org/8259_PIC#Initialisation
    port_call(0x20, 0x11);
    port_call(0xA0, 0x11);

    port_call(0x21, INTERRUPT_REQUEST_START);        // Start first 8 interrupt requests at 0x20 (0 to 7).
    port_call(0xA1, INTERRUPT_REQUEST_START + 0x08); // Start next 8 interrupt requests at 0x28 (8 to 15).

    port_call(0x21, 0x04);
    port_call(0xA1, 0x02);

    port_call(0x21, 0x01);
    port_call(0xA1, 0x01);

    // Setup masking of the interrupts.
    port_call(0x21, 0b10111111);
    port_call(0xA1, 0b11111111);

    make_gates();

    load_idt(&interruptDescriptor);
}