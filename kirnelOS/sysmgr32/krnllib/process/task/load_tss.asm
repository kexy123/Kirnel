[BITS 32]

section .text

global load_tss

load_tss:
    mov ax, [esp + 4] ; The segment selector.
    ltr ax ; Load the task state segment.
    
    ret
