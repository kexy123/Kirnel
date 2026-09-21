[BITS 32]

section .text

global port_call
global port_read

port_call:
    mov dx, [esp + 4] ; The port.
    mov al, [esp + 8] ; The byte.

    out dx, al

    ret

port_read:
    mov dx, [esp + 4] ; The port.

    in al, dx

    ret