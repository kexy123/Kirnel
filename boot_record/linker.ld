OUTPUT_FORMAT(elf32-i386)
ENTRY(krnl_boot)

SECTIONS
{
    . = 0x8000;

    .text :
    {
        *(.text.krnl_boot)
        *(.text)
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