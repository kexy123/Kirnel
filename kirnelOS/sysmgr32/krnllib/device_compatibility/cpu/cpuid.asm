[BITS 32]

section .text

global has_cpuid

global get_cpuid_feature

has_cpuid:
    ; https://wiki.osdev.org/CPUID#Checking_CPUID_availability
    pushfd
    pushfd
    xor dword [esp], 0x00200000 ; Test the ID bit in EFLAGS on the stack by flipping it.
    popfd
    pushfd

    pop eax
    xor eax, [esp] ; Because the ID bit can be flipped, if it was modified, we want that bit to be consistently set.
    popfd

    and eax, 0x00200000 ; Test if ID bit changed. It is only modifiable if CPUID exists.
    test eax, eax
    jnz cpuid_exists

    ret

cpuid_exists:
    mov eax, 0x00000001
    ret

get_cpuid_feature:
    mov eax, 1 ; Get CPUID features.
    cpuid

    mov eax, [esp + 4] ; The register type.

    ; TODO: Convert to jump table if should be optimized.
    cmp eax, 0 ; The EBX case.
    je case_feature_ebx

    cmp eax, 1 ; The ECX case.
    je case_feature_ecx

    cmp eax, 2 ; The EDX case.
    je case_feature_edx

    xor eax, eax
    jmp switch_end

case_feature_ebx:
    mov eax, ebx
    jmp switch_end

case_feature_ecx:
    mov eax, ecx
    jmp switch_end

case_feature_edx:
    mov eax, edx
    jmp switch_end

switch_end:
    ret