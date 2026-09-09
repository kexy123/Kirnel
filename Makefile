# COMMANDS
CC := ia16-elf-gcc
LD := ia16-elf-ld
OBJCOPY := ia16-elf-objcopy
NASM := nasm

CFLAGS := -ffreestanding -mcmodel=small -Icommon -Ikernel -std=c11

# OBJECTS
C_SOURCES := $(shell find kernel common boot_record -name '*.c')
C_OBJECTS := $(C_SOURCES:.c=.o)

ASM_SOURCES := $(shell find kernel common -name '*.asm')
ASM_OBJECTS := $(ASM_SOURCES:.asm=.o)

OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)

all: disk.img

# COMPILE C AND ASM
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(NASM) -f elf $< -o $@


# LINK ASSEMBLIES
boot_record/kernel.elf: $(OBJECTS)
	$(LD) -T linker.ld boot_record/kernel_boot.o $(filter-out boot_record/kernel_boot.o,$(C_OBJECTS)) $(ASM_OBJECTS) -o $@

boot_record/kernel.bin: boot_record/kernel.elf
	$(OBJCOPY) -O binary boot_record/kernel.elf boot_record/kernel.bin


# COMPILE BOOT
boot_record/boot.bin: boot_record/boot.asm
	$(NASM) -f bin boot_record/boot.asm -o boot_record/boot.bin

disk.img: boot_record/boot.bin boot_record/kernel.bin
	cat boot_record/boot.bin boot_record/kernel.bin > disk.img


clean:
	rm -f kernel/*.o common/*.o
	rm -f boot_record/kernel.elf boot_record/kernel.bin boot_record/boot.bin disk.img