[BITS 32]

section .text

global load_idt

load_idt:
    mov eax, [esp + 4]
    lidt [eax] ; Load the interrupt descriptor table.
    sti

    ret