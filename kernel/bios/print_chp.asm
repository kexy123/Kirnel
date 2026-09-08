[BITS 16]

section .text
global print_chp ; CHar Pointer

print_chp:
    push bp
    push si
    mov bp, sp

    mov si, [bp + 6]

log_inc:
    lodsb           ; Read next byte and increment from SI.

    cmp al, 0       ; Byte is at AL register.
    jne log_byte
log_exit:
    ; Flow into the exit subprocedure.
    pop si
    pop bp
    ret
log_byte:
    mov ah, 0x0E    ; Teletype
    mov bh, 0       ; Page 0
    int 0x10        ; BIOS interrupt; log the given byte at AL.

    jmp log_inc
