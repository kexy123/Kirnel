[BITS 32]

section .text

global send_fdc_command

extern port_call

send_fdc_command:
    ; https://wiki.osdev.org/Floppy_Disk_Controller#MSR_bitflag_definitions
    mov dx, 0x3F4 ; The x86 in function only accepts an immediate 8-bit value, so 0x3F4 cannot be substituted.
    in al, dx ; Main status register.

    ; Check if RQM (request for master) is set, so we can communicate to it.
    test al, 0b1000_0000
    jz send_fdc_command

    ; Check the direction of information. It must be 0 for CPU sending data to the FDC.
    test al, 0b0100_0000
    jnz send_fdc_command

    jmp port_call