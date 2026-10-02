[BITS 32]

section .text

global load_gdt

load_gdt:
    mov eax, [esp + 4] ; The descriptor location.

    cli
    lgdt [eax]
    sti

    ret