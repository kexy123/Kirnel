[BITS 32]

section .text

global __syscall_iostream_out

__syscall_iostream_out:
    mov ebx, [esp + 4] ; The address of the string to output.
    mov ecx, [esp + 8] ; The length to output.

    mov eax, 0x0001 ; System call: output to stream.
    int 0x80

    ret