OUTPUT_FORMAT(elf32-i386)
ENTRY(shell_start)

SECTIONS
{
    . = 0x00000000;

    .text :
    {
        KEEP(*(.text.shell_start))
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