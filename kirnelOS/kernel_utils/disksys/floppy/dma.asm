[BITS 32]

section .text

global setup_dma
global prepare_dma_for_read

reset_flip_flop:
    ; Reset DMA flip-flop. This is to preserve the sections of the values being passed in that have to be split into multiple bytes.
    mov al, 0xFF ; Flip-flop reset register.
    out 0x0C, al

    ret

setup_dma:
    ; DMA initialisation for floppy disks: https://wiki.osdev.org/ISA_DMA#Floppy_Disk_DMA_Initialization

    ; Mask DMA channel 2.
    mov al, 0b0000_0110
    out 0x0A, al ; Single channel mask register.


    ; Configuring the address of the buffer.
    call reset_flip_flop

    mov eax, [esp + 4] ; Address of the buffer.
    ; Assign the first 24 bits of the address. 0x04 is the first 16-bit address register.
    out 0x04, al ; Bits 0-7.
    shr eax, 8
    out 0x04, al ; Bits 8-15.

    ; 0x81 is the page address register.
    shr eax, 8
    out 0x81, al ; Bits 16-23.


    ; Configuring the number of bytes in the buffer.
    call reset_flip_flop

    mov eax, [esp + 8] ; The number of bytes.
    dec ax ; DMA requires the number of bytes - 1.
    ; Assign the 16 bits. 0x05 is the 16-bit byte count register.
    out 0x05, al ; Bits 0-7
    mov al, ah
    out 0x05, al ; Bits 8-15


    ; Unmask DMA channel 2.
    mov al, 0b0000_0010
    out 0x0A, al

    ret


prepare_dma_for_read:
    ; Mask DMA channel 2.
    mov al, 0b0000_0110
    out 0x0A, al

    ; Prepare floppy disk read.
    mov al, 0b0100_0110
    out 0x0B, al ; https://wiki.osdev.org/ISA_DMA#DMA_Mode_Registers_0x0B_and_0xD6_(Write)

    ; Unmask DMA channel 2.
    mov al, 0b0000_0010
    out 0x0A, al

    ret