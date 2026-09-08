[bits 16]
[org 0x7C00]

_start:
kernel_boot_loader:
    cli

    xor ax, ax
    mov ds, ax

    ; Kernel boot address is at 0x0000:0x8000
    mov es, ax
    mov bx, 0x8000

    ; https://en.wikipedia.org/wiki/INT_13H#INT_13h_AH=02h:_Read_Sectors_From_Drive
    mov ah, 0x02    ; BIOS disk reading
    mov al, 1       ; Read 1 sector at
    mov ch, 0       ; Cylinder 0
    mov cl, 2       ; Sector 2
    mov dh, 0       ; Head 0

    int 13h
    jc disk_read_error

    jmp 0x0000:0x8000


disk_read_error:
    mov si, disk_error
    jmp bios_log
    jmp boot_halt


; Similar implementation to ./bootload/bios_log.asm
bios_log:
    lodsb

    cmp al, 0
    jne bios_log_byte

    ret
bios_log_byte:
    mov ah, 0x0E
    mov bh, 0
    int 0x10

    jmp bios_log

; Halts the boot loader.
boot_halt:
    cli
    hlt
    jmp boot_halt

disk_error db "Kernel boot sector not found. Please restart the OS.", 0

; Pad boot sector
times 510 - ($ - $$) db 0
dw 0xAA55