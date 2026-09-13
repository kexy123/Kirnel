[BITS 16]
[ORG 0x7C00]

; Ensure that the first 3 bytes fit the FAT12 boot sector BPB structure.
jmp short _start
nop

; FAT12 BIOS parameter block
oem_name:               db "KIRNELOS"       ; 8 bytes.

bytes_per_sector:       dw 512
sectors_per_cluster:    db 1                ; 1-to-1 scale.
reserved_sectors:       dw 16               ; The first 16 sectors are for the boot_record.
fat_count:              db 2                ; FAT usually contains two tables historically for redundancy and recovery.
root_entry_count:       dw 224              ; 224 32-bytes directory entries can exist in the root.
total_sectors:          dw 2880             ; 2880 sectors corresponds to about 1.5 MB.
media_descriptor:       db 0xF0             ;                                                                       ???
sectors_per_fat:        dw 9                ; How many sectors each FAT has. This includes the two FATs.
sectors_per_track:      dw 18               ;                                                                       ???
head_count:             dw 2                ;                                                                       ???
hidden_sectors:         dd 0                ; Not necessary; this OS is currently not possible in partitions.
large_sectors:          dd 0                ; Extra field for storing the number of total sectors.

; FAT12 BIOS parameter block (extended)
drive_number:           db 0                ; The BIOS drive.
reserved:               db 0                ; Reserved for the file or operating system.
boot_signature:         db 0x29             ; Indicates that this group contains a set of specific fields under this.
volume_id:              dd 0x12123212       ; Serial number.
volume_label:           db "KIRNEL OS  "    ; 11 bytes
filesystem:             db "FAT12   "       ; 8 bytes


_start:
kernel_boot_loader:
    cli

    xor ax, ax
    mov ds, ax

    mov [drive_number], dl

    ; Kernel boot address is at 0x0000:0x8000
    mov es, ax
    mov bx, 0x8000

    ; https://en.wikipedia.org/wiki/INT_13H#INT_13h_AH=02h:_Read_Sectors_From_Drive
    mov ah, 0x02    ; BIOS disk reading
    mov al, [reserved_sectors]
    mov ch, 0       ; Cylinder 0
    mov cl, 2       ; Sector 2
    mov dh, 0       ; Head 0

    int 0x13
    jc disk_read_error

    call 0x0000:0x8000 ; Run krnl_boot from kernel_boot.c.

    ; Shift the data segment register.
    mov ax, 0x0D00
    mov ds, ax

    jmp 0x0D00:0x0000 ; Go to the kernel that was loaded into memory at 0x0D000.

; https://en.wikipedia.org/wiki/A20_line
enable_a20_line:
    cli
    in al, 0x92

    ; Enable the 2nd bit of the 0x92 IO port to enable the A20 line.
    ; 0x02 = 0b00000010
    or al, 0x02
    out 0x92, al

    ret

disk_read_error:
    mov si, disk_error
    jmp bios_log
    jmp boot_halt


; Similar implementation to boot_record/bios/print_chp.asm
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