#include "handles.h"
#include "interrupts/interrupts.h"
#include "interrupts/interrupt_state.h"
#include "structure.h"
#include "utils/portcall.h"

IDTEntry idtTable[256];

IDTDescriptor interruptDescriptor;

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

void make_gates()
{
    create_idt_gate(0x08, (unsigned long)service8, double_fault_handle, 0x08, (IDTEntryFlags){.Raw = 0b10001110}); // Interrupt 8 (#DF): double fault.

    create_idt_gate(0x0E, (unsigned long)service14, page_fault_handle, 0x08, (IDTEntryFlags){.Raw = 0b10001110}); // Interrupt 14 (#PF): page fault.

    create_idt_gate(0x26, (unsigned long)irq6, fdc_interrupt_handle, 0x08, (IDTEntryFlags){.Raw = 0b10001110}); // IRQ6: floppy disk controller.
}