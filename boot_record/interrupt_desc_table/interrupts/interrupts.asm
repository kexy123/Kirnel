[BITS 32]

section .text

;;;;; MACROS ;;;;;

; The interrupt service routine does not yield an error code.
%macro service_noerror 1
    global service%1
    service%1:
        cli

        push dword 0
        push dword %1

        ; Store the CPU's state before firing the interrupt service handle.
        jmp isr_cpu_preserve
%endmacro

; The interrupt service routine yields an error code.
%macro service_error 1
    global service%1
    service%1:
        cli

        ; The error code is already provided.
        push dword %1

        jmp isr_cpu_preserve
%endmacro

; The interrupt service routine for hardware interrupts (interrupt requests).
%macro int_req_service 2
    global irq%1
    irq%1:
        cli

        push dword 0
        push dword %2

        jmp isr_cpu_preserve
%endmacro


;;;;; INTERRUPTS ;;;;;
service_error 8 ; Double fault.

int_req_service 6, 0x26 ; IRQ6; floppy disk controller hardware interrupt.


;;;;; INTERRUPT HANDLE ;;;;;

extern service_handle

isr_cpu_preserve:
    ; Store the CPU state before firing the interrupt service handle.
    pusha
    mov eax, ds
    push eax

    ; Store the page fault: https://en.wikipedia.org/wiki/Control_register#CR2
    mov eax, cr2
    push eax

    ; Use the kernel segment from the GDT.
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp ; Store the location of where the CPU stopped.
    call service_handle

    ; Apply the segment back.
    add esp, 8
    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    ; Restore CPU state.
    popa
    add esp, 8
    sti
    iret