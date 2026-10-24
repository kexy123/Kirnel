[BITS 32]

section .text

global context_switch

extern print_uintx

context_switch:
    add esp, 4

    pop eax
    mov cr3, eax

    pop ds
    pop es
    pop fs
    pop gs

    popad
    iretd