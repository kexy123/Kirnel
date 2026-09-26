[BITS 32]

section .text

global halt
global panic

halt:
    hlt
    ret

panic:
    cli
    hlt
    jmp panic