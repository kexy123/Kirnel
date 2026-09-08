[BITS 16]

section .text
global print_chp ; CHar Pointer

print_chp:
    push bp
    push si
    mov bp, sp

    mov si, [bp + 6]
    ; mov si, test_message

log_inc:
    lodsb

    cmp al, 0 ; TODO: Do string length encoding
    jne log_byte
log_exit:
    pop si
    pop bp
    ret
log_byte:
    mov ah, 0x0E
    mov bh, 0
    int 0x10

    jmp log_inc

; test_message db "Hello, world...", 0