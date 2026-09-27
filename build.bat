@echo off

if "%1" == "-no" (
    qemu-system-i386 -drive format=raw,file=floppy.img,if=floppy -m 128M
    exit /b
)

wsl make

if "%1" == "-d" (
    @REM Debugger; run gdb; target remote :1234
    qemu-system-i386 -drive format=raw,file=floppy.img,if=floppy -S -s -m 128M
) else (
    qemu-system-i386 -drive format=raw,file=floppy.img,if=floppy -m 128M
)