# COMMANDS
CC := ia16-elf-gcc
LD := ia16-elf-ld
OBJCOPY := ia16-elf-objcopy
NASM := nasm

CFLAGS := -ffreestanding -nostdlib -mcmodel=small -IkirnelOS -std=c11
BOOT_CFLAGS := -ffreestanding -nostdlib -mcmodel=small -Iboot_record -std=c11

# OBJECTS
C_SOURCES := $(shell find kirnelOS -name '*.c')
C_OBJECTS := $(C_SOURCES:.c=.o)
C_OBJECTS := $(C_OBJECTS:kirnelOS/%=build/kirnelOS/%)

ASM_SOURCES := $(shell find kirnelOS -name '*.asm')
ASM_OBJECTS := $(ASM_SOURCES:.asm=.bin)
ASM_OBJECTS := $(ASM_OBJECTS:kirnelOS/%=build/kirnelOS/%)

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

boot_record/%.o: boot_record/%.c
	$(CC) $(BOOT_CFLAGS) -c $< -o $@

boot_record/%.o: boot_record/%.asm
	$(NASM) -f elf $< -o $@

# COMPILE C AND ASM
build/kirnelOS/%.o: kirnelOS/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/kirnelOS/%.bin: kirnelOS/%.asm
	mkdir -p $(dir $@)
	$(NASM) -f bin $< -o $@


# LINK BOOT ASSEMBLIES
boot_record/kernel.elf: $(BOOT_OBJECTS)
	$(LD) -T boot_record/linker.ld boot_record/kernel_boot.o $(filter-out boot_record/kernel_boot.o,$(BOOT_C_OBJECTS)) $(BOOT_ASM_OBJECTS) -o $@

boot_record/kernel.bin: boot_record/kernel.elf
	$(OBJCOPY) -O binary boot_record/kernel.elf boot_record/kernel.bin


# COMPILE BOOT
boot.bin: boot.asm
	$(NASM) -f bin boot.asm -o boot.bin

boot_record/boot.img: boot.bin boot_record/kernel.bin
	cat boot.bin boot_record/kernel.bin > boot_record/boot.img
# 16 SECTORS
	truncate -s 8192 boot_record/boot.img

floppy.img: boot_record/boot.img $(OBJECTS)
	truncate -s 1440K floppy.img
	mkfs.fat -F 12 -R 16 -S 512 floppy.img
	dd if=boot_record/boot.img of=floppy.img bs=512 count=16 conv=notrunc
	mcopy -i floppy.img -s build/* ::/


clean:
	rm -rf build/
	rm -f $(BOOT_OBJECTS) boot_record/*.o
	rm -f boot_record/kernel.elf boot_record/kernel.bin boot.bin boot_record/boot.img