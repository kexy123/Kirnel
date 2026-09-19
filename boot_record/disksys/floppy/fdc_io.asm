[BITS 32]

section .text

global send_fdc_command

extern port_call

send_fdc_command:
    ; https://wiki.osdev.org/Floppy_Disk_Controller#MSR_bitflag_definitions
    in al, 0x3F4 ; Main status register.

    ; Check if RQM (request for master) is set, so we can communicate to it.
    test al, 0b1000_0000
    jz send_fdc_command

    ; Check the direction of information. It must be 0 for CPU -> FDC.
    test al, 0b0000_1000
    jnz send_fdc_command

    jmp port_call

    ret