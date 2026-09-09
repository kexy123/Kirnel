[BITS 16]

section .text
global in_ch        ; CHaracter

in_ch:
    mov ah, 0x00

    int 0x16        ; BIOS interrupt; read any keyboard key.
    ; AL is the keyboard key in ASCII.
    ret