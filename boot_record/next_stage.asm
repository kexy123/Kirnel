[BITS 32]

section .text

extern kernelStackEnd

global jump_next_stage

jump_next_stage:
    mov ax, 0x10

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ss, ax
    mov esp, [kernelStackEnd]

    jmp 0x08:0xC0000000
    ret