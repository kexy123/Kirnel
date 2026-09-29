[BITS 32]

section .text

extern kernel_space

global kernel_jump

kernel_jump:
    pop eax ; The return address.

    add eax, dword [kernel_space]

    jmp eax ; Goes to the new offset. Also functions as a ret instruction as the return address is popped.