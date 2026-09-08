[BITS 16]

section .text
global out_chp      ; CHar Pointer
global out_ch       ; CHaracter

out_chp:
    push bp
    push si
    mov bp, sp

    mov si, [bp + 6]

chp_inc:
    lodsb           ; Read next byte and increment from SI.

    cmp al, 0       ; Byte is at AL register.
    jne chp_byte
chp_exit:
    ; Flow into the exit subprocedure.
    pop si
    pop bp
    ret
chp_byte:
    mov ah, 0x0E    ; Teletype
    mov bh, 0       ; Page 0
    int 0x10        ; BIOS interrupt; log the given byte at AL.

    jmp chp_inc


out_ch:
    push bp
    mov bp, sp
    
    mov al, [bp + 4]

    mov ah, 0x0E
    mov bh, 0

    int 0x10

    pop bp
    ret