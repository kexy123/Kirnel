[BITS 32]

section .text

global jump_next_stage

jump_next_stage:
    mov ax, 0x10

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x90000

    jmp 0x08:0xD000
    ret