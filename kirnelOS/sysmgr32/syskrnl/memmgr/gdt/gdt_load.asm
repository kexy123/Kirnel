[BITS 32]

section .text

global load_gdt

load_gdt:
    mov eax, [esp + 4]
    lgdt [eax]
    ret