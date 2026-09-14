[BITS 32]

section .text

global setup_dma
global pass_command

reset_flip_flop:
    ; Reset DMA flip-flop. This is to preserve the sections of the values being passed in that have to be split into multiple bytes.
    mov dx, 0x0C ; Flip-flop reset register.
    xor al, al
    out dx, al

    ret

setup_dma:
    ; DX register specification: https://wiki.osdev.org/ISA_DMA#The_Registers

    ; Mask DMA channel 2.
    mov dx, 0x0A ; Single channel mask register.
    mov al, 0b0000_0110
    out dx, al


    call reset_flip_flop

    mov eax, [esp + 4] ; Address of the buffer.
    ; Assign the first 24 bits of the address.
    mov dx, 0x04 ; Start address register for channel 2.
    out dx, al ; Bits 0-7
    shr eax, 8
    out dx, al ; Bits 8-15
    shr eax, 8
    mov dx, 0x81 ; Channel 2 page address register.
    out dx, al ; Bits 16-23


    call reset_flip_flop

    mov ax, [esp + 8] ; The number of bytes.
    dec ax ; DMA requires the number of bytes - 1.
    ; Assign the 16 bits.
    mov dx, 0x05 ; Start count register for channel 2.
    out dx, al ; Bits 0-7
    mov al, ah
    out dx, al ; Bits 8-15

    ; What does this do?
    mov dx, 0x0B ; Mode register
    mov al, 0b0100_0110 ; https://wiki.osdev.org/ISA_DMA#DMA_Mode_Registers_0x0B_and_0xD6_(Write)
    out dx, al

    ; Unmask DMA channel 2.
    mov dx, 0x0A
    mov al, 0b0000_0010
    out dx, al

    ret

pass_command:
    ; Check if we can currently pass in a command.
    mov dx, 0x3F4
    in al, dx

    ; https://wiki.osdev.org/Floppy_Disk_Controller#MSR_bitflag_definitions
    test al, 0b1000_0000
    jz pass_command ; The request for master control must be set to access the data.

    test al, 0b0100_0000
    jnz pass_command ; The direction of data transfer must be the CPU -> FIFO IO port (0).

pass:
    ; Pass in command.
    mov dx, [esp + 4] ; The port.
    mov al, [esp + 8] ; The byte.

    out dx, al

    ret