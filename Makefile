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

all: disk.img

# COMPILE C AND ASM
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(NASM) -f elf $< -o $@


# LINK ASSEMBLIES
kernel.elf: $(OBJECTS)
	$(LD) -T linker.ld kernel/kernel.o $(filter-out kernel/kernel.o,$(C_OBJECTS)) $(ASM_OBJECTS) -o $@

kernel.bin: kernel.elf
	$(OBJCOPY) -O binary kernel.elf kernel.bin


# COMPILE BOOT
boot.bin: boot.asm
	$(NASM) -f bin boot.asm -o boot.bin

disk.img: boot.bin kernel.bin
	cat boot.bin kernel.bin > disk.img


clean:
	rm -f kernel/*.o common/*.o
	rm -f kernel.elf kernel.bin boot.bin disk.img