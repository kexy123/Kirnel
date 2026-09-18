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
sectors_per_fat:        dw 9                ; How many sectors each FAT has.
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


;;;;; GLOBAL DESCRIPTOR TABLE ;;;;;

; https://wiki.osdev.org/Global_Descriptor_Table#Segment_Descriptor
gdt_start:
    null_descriptor:
        dq 0

    KERNEL_CODE_SELECTOR equ 0x08
    kernel_code: ; At 0x08
        dw 0xFFFF ; Limit address low

        dw 0x0000 ; Base address low
        db 0x00 ; Base address mid

        ;    PD SECRA
        db 0b10011010 ; Access flags
        ; Offset | ID  | Name                       | # | Description
        ; -------|-----|----------------------------|---|-----------------------------------------------------------------------------------------------------------------
        ; 7      | P   | Present bit                | 1 | Should be set to make this segment present.
        ; 6      | DPL | Descriptor privilege level | 2 | Part of the protection ring (https://en.wikipedia.org/wiki/Protection_ring).
        ; 4      | S   | Descriptor type bit        | 1 | 0 to define a system segment; 1 to define a code or data segment.
        ; 3      | E   | Executable bit             | 1 | 0 to declare that it's a data segment; 1 to declare that it's executable code.
        ; 2      | D/C | Direction/conforming bit   | 1 | Dependent on E: if E is 0 (data), this bit is the direction that grows the segment up (0) or down (1).
        ;                                                 If E is 1 (code), this bit determines if code can only be run in its protection ring (0) or the ring lower (1).
        ; 1      | R/W | Read/write access bit      | 1 | Dependent on E: if E is 0 (data), this bit determines if the data is writable (1) or not (0).
        ;                                                 If E is 1 (code), this bit determines if code is only readable (1) or not (0).
        ; 0      | A   | Accessed bit               | 1 | The CPU sets it when the segment is accessed.

        ;    GDLR
        db 0b1100_1111 ; Extra flags and Limit address high
        ; Offset | ID | Name        | # | Description
        ; -------|----|-------------|---|-------------------------------------------------------------------------------
        ; 3      | G  | Granularity | 1 | The size of the limit address should be multiplied by 4 KiB (1) or not (0).
        ; 2      | DB | Size flag   | 1 | Defines a 16-bit (0) or 32-bit (1) protected mode segment.
        ; 1      | L  | Long-mode   | 1 | Defines a 64-bit (1) code segment or not (0). DB and L are mutually exclusive.
        ; 0      | R  | Reserved    | 1 | Reserved.

        db 0x00 ; Base address high

    kernel_data: ; At 0x10
        dw 0xFFFF
        dw 0x0000
        db 0x00
        db 0b10010010
        db 0b1000_1111
        db 0x00
gdt_end:

; https://wiki.osdev.org/Global_Descriptor_Table#GDTR
gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; The size of the global descriptor table
    dd gdt_start ; The pointer to the first entry in the global descriptor table


;;;;; MAIN BOOT LOADER ;;;;;

_start:
kernel_boot_loader:
    cli

    xor ax, ax
    mov ds, ax

    mov [drive_number], dl

    NEXT_BOOTLOADER_ENTRY equ 0x8000
    ; Kernel boot address is at 0x0000:0x8000
    mov es, ax
    mov bx, NEXT_BOOTLOADER_ENTRY

    ; https://en.wikipedia.org/wiki/INT_13H#INT_13h_AH=02h:_Read_Sectors_From_Drive
    mov ah, 0x02    ; BIOS disk reading
    mov al, [reserved_sectors]
    mov ch, 0       ; Cylinder 0
    mov cl, 2       ; Sector 2
    mov dh, 0       ; Head 0

    int 0x13
    jnc enable_protected_mode ; If the next stage of the boot loader was loaded into memory, begin enabling protected mode.

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


;;;;; ENABLING PROTECTED MODE ;;;;;

enable_protected_mode:
enable_a20_line:
    ; https://en.wikipedia.org/wiki/A20_line
    cli
    in al, 0x92

    ; Enable the 2nd bit of the 0x92 IO port to enable the A20 line.
    ; 0x02 = 0b00000010
    or al, 0x02
    out 0x92, al

load_gdt:
    lgdt [gdt_descriptor]

set_cr0_register:
    ; https://en.wikipedia.org/wiki/Control_register#CR0
    mov eax, cr0
    or eax, 1
    mov cr0, eax
; [BITS 32]

    ; mov al, 0x41
    ; mov ah, 0x0F
    ; mov [0xB8000], ax

    jmp 0x08:0x8000
    ; jmp KERNEL_CODE_SELECTOR:NEXT_BOOTLOADER_ENTRY

halt:
    cli
    hlt
    jmp halt


;;;;; PAD BOOT SECTOR ;;;;;
times 510 - ($ - $$) db 0
dw 0xAA55