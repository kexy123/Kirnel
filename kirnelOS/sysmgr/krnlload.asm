[BITS 16]

_start:
    mov ax, cs
    mov ds, ax
    mov si, message
    jmp log

; Similar implementation to boot_record/bios/print_chp.asm
log:
    lodsb

    cmp al, 0
    jne log_byte

    jmp halt
    ; ret
log_byte:
    mov ah, 0x0E
    mov bh, 0
    int 0x10

    jmp log

halt:
    cli
    hlt
    jmp halt

message db "krnlload is running!", 0