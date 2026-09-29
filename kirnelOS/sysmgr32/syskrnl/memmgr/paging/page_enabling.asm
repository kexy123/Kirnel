[BITS 32]

section .text

extern pagingEnabled

global enable_paging

enable_paging:
    ; https://wiki.osdev.org/X86_Paging#32-bit_Paging
    ; Move the address to CR3.
    mov eax, [esp + 4] ; The PageDirectory address.
    mov cr3, eax

    ; Enable the paging and protection bits of CR0.
    mov eax, cr0
    or eax, 0x80000001
    mov cr0, eax

    mov [pagingEnabled], byte 0x01

    ret