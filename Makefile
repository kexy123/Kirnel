# COMMANDS
CC := ia16-elf-gcc
LD := ia16-elf-ld
OBJCOPY := ia16-elf-objcopy
NASM := nasm

CFLAGS := -ffreestanding -mcmodel=small -Icommon -Ikernel -std=c11

# OBJECTS
C_SOURCES := $(shell find kernel common -name '*.c')
C_OBJECTS := $(C_SOURCES:.c=.o)

ASM_SOURCES := $(shell find kernel common -name '*.asm')
ASM_OBJECTS := $(ASM_SOURCES:.asm=.o)

OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)

BOOT_ASM_SOURCES := $(shell find boot_record -name '*.asm')
BOOT_ASM_OBJECTS := $(BOOT_ASM_SOURCES:.asm=.o)

BOOT_C_SOURCES := $(shell find boot_record -name '*.c')
BOOT_C_OBJECTS := $(BOOT_C_SOURCES:.c=.o)

BOOT_OBJECTS := $(BOOT_C_OBJECTS) $(BOOT_ASM_OBJECTS)

all: floppy.img

# COMPILE BOOT SECTOR
boot.bin: boot.asm
	$(NASM) -f bin $< -o $@

# COMPILE C AND ASM
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(NASM) -f elf $< -o $@


# LINK BOOT ASSEMBLIES
boot_record/kernel.elf: $(BOOT_OBJECTS)
	$(LD) -T linker.ld boot_record/kernel_boot.o $(filter-out boot_record/kernel_boot.o,$(BOOT_C_OBJECTS)) $(BOOT_ASM_OBJECTS) -o $@

boot_record/kernel.bin: boot_record/kernel.elf
	$(OBJCOPY) -O binary boot_record/kernel.elf boot_record/kernel.bin


# COMPILE BOOT
boot.bin: boot.asm
	$(NASM) -f bin boot.asm -o boot.bin

boot_record/boot.img: boot.bin boot_record/kernel.bin
	cat boot.bin boot_record/kernel.bin > boot_record/boot.img
# 16 SECTORS
	truncate -s 8192 boot_record/boot.img

floppy.img: boot_record/boot.img
	truncate -s 1440K floppy.img
	mkfs.fat -F 12 -R 16 -S 512 floppy.img
	dd if=boot_record/boot.img of=floppy.img bs=512 count=16 conv=notrunc


clean:
	rm -f $(OBJECTS) boot_record/*.o
	rm -f boot_record/kernel.elf boot_record/kernel.bin boot.bin boot_record/boot.img