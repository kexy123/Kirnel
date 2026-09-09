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
number_of_fats:         db 2                ; FAT usually contains two tables historically for redundancy and recovery.
root_entries:           dw 224              ; 224 32-bytes directory entries can exist in the root.
total_sectors:          dw 2880             ; 2880 sectors corresponds to about 1.5 MB.
media_descriptor:       db 0xF0             ;                                                                       ???
sectors_per_fat:        dw 9                ; How many sectors each FAT has. This includes the two FATs.
sectors_per_track:      dw 18               ;                                                                       ???
number_of_heads:        dw 2                ;                                                                       ???
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

    ; Kernel boot address is at 0x0000:0x8000
    mov es, ax
    mov bx, 0x8000

    ; https://en.wikipedia.org/wiki/INT_13H#INT_13h_AH=02h:_Read_Sectors_From_Drive
    mov ah, 0x02    ; BIOS disk reading
    mov al, 2       ; Read 2 sectors at
    mov ch, 0       ; Cylinder 0
    mov cl, 2       ; Sector 2
    mov dh, 0       ; Head 0

    int 0x13
    jc disk_read_error

    jmp 0x0000:0x8000


disk_read_error:
    mov si, disk_error
    jmp bios_log
    jmp boot_halt


; Similar implementation to kernel/bios/print_chp.asm
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