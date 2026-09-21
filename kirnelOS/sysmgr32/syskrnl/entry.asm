[BITS 32]

section .text

_start:
    mov ax, 0x10
    mov ds, ax
    mov si, message

    mov edi, 0xB8000
    call log
    jmp halt

; Similar implementation to boot_record/vga/out.c
log:
    lodsb

    cmp al, 0
    jne log_byte

    ret
log_byte:
    mov [edi], al
    inc edi

    mov byte [edi], 0x0F
    inc edi

    jmp log

halt:
    cli
    hlt
    jmp halt

message db "krnlload is running!", 0