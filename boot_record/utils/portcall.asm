[BITS 32]

section .text

global port_call

port_call:
    mov dx, [esp + 4] ; The port.
    mov al, [esp + 8] ; The byte.

    out dx, al

    ret