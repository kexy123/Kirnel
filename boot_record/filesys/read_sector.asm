[BITS 16]

section .text
global read_sector

read_sector:
    push bp
    push si

    mov bp, sp
    mov si, [bp + 6]
    cli

    ; https://en.wikipedia.org/wiki/INT_13H#INT_13h_AH=42h:_Extended_Read_Sectors_From_Drive
    mov ah, 0x42
    int 0x13

    pop si
    pop bp
    ret