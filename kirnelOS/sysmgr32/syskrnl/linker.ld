OUTPUT_FORMAT(elf32-i386)
ENTRY(krnl_start)

SECTIONS
{
    . = 0xD000;

    .text :
    {
        KEEP(*(.text.krnl_start))
        *(.text*)
    }

    .rodata :
    {
        *(.rodata)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(.bss)
    }
}