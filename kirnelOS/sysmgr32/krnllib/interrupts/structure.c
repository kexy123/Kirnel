#include "handles.h"
#include "interrupts/interrupts.h"
#include "interrupts/interrupt_state.h"
#include "structure.h"
#include "utils/portcall.h"

IDTEntry idtTable[256];

IDTDescriptor interruptDescriptor;

/// @brief Loads the given interrupt descriptor table with the given address to the descriptor.
/// @param descriptor The address of the IDTDescriptor.
extern void load_idt(IDTDescriptor *descriptor);

/// @brief Creates an interrupt descriptor entry at the given interruptCode in the idtTable.
/// @param interruptCode The location to create the entry at.
/// @param offset The address to where the interrupt handler is.
/// @param handle The interrupt handle to install.
/// @param segmentSelector The segment selector of this entry.
/// @param flags The flags of this interrupt entry.
void create_idt_gate(unsigned char interruptCode, unsigned long offset, InterruptHandle handle, unsigned short segmentSelector, IDTEntryFlags flags)
{
    IDTEntry *entry = &idtTable[interruptCode];

    entry->OffsetLow = (unsigned short)offset;
    entry->OffsetHigh = (unsigned short)(offset >> 16);

    entry->Reserved = 0x00;
    entry->SegmentSelector = segmentSelector;

    // entry->Flags = flags | 0x01100000;
    entry->Flags = flags;

    if (handle)
    {
        install_handle(interruptCode, handle);
    }
}

void init_idt()
{
    interruptDescriptor.Limit = sizeof(IDTEntry) * 256 - 1;
    interruptDescriptor.Base = (unsigned long)&idtTable;

    // Initialise the Programmable Interrupt Controller for hardware interrupts.
    // Specification: https://wiki.osdev.org/8259_PIC#Initialisation
    port_call(0x20, 0x11);
    port_call(0xA0, 0x11);

    port_call(0x21, INTERRUPT_REQUEST_START);        // Start first 8 interrupt requests at 0x20 (0 to 7)
    port_call(0xA1, INTERRUPT_REQUEST_START + 0x08); // Start next 8 interrupt requests at 0x28 (8 to 15)

    port_call(0x21, 0x04);
    port_call(0xA1, 0x02);

    port_call(0x21, 0x01);
    port_call(0xA1, 0x01);

    // Setup masking of the interrupts.
    port_call(0x21, 0b10111111);
    port_call(0xA1, 0b11111111);

    create_idt_gate(0x08, (unsigned long)service8, double_fault_handle, 0x08, (IDTEntryFlags){.Raw = 0b10001110}); // Interrupt 8 (#DF): double fault.
    create_idt_gate(0x26, (unsigned long)irq6, fdc_interrupt_handle, 0x08, (IDTEntryFlags){.Raw = 0b10001110});    // IRQ6: floppy disk controller.

    load_idt(&interruptDescriptor);
}