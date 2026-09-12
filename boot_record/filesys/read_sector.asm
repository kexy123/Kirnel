[BITS 16]

section .text
global read_sectors_from_drap

read_sectors_from_drap:
    push bp
    push si
    push ds

    mov bp, sp
    mov dl, [bp + 8]

    mov ds, [bp + 10]
    mov si, [bp + 12]

    ; https://en.wikipedia.org/wiki/INT_13H#INT_13h_AH=42h:_Extended_Read_Sectors_From_Drive
    mov ah, 0x42
    int 0x13

    jc error
    mov ax, 0x0;
exit:
    pop ds
    pop si
    pop bp

    ret
error:
    mov ax, 0x1;
    jmp exit