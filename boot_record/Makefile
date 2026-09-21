export PATH := /usr/local/i386-elf/bin:$(PATH)

# COMMANDS
ELF_GCC := i386-elf-gcc
ELF_LD := i386-elf-ld
ELF_OBJCPY := i386-elf-objcopy

NASM := nasm
MAKE_DIR = mkdir -p $(dir $@)

C_FLAGS := -ffreestanding -nostdlib -m32 -Iboot_record -IkirnelOS/kernel_utils -std=c11

# OBJECTS
C_SOURCES := $(shell find boot_record -name '*.c')
C_OBJECTS := $(C_SOURCES:.c=.o)
C_OBJECTS := $(C_OBJECTS:boot_record/%=build_sub/boot_record/%)

ASM_SOURCES := $(shell find boot_record -name '*.asm')
ASM_OBJECTS := $(ASM_SOURCES:.asm=.o)
ASM_OBJECTS := $(ASM_OBJECTS:boot_record/%=build_sub/boot_record/%)

EXT_OBJECTS := $(shell find build_sub/kirnelOS/kernel_utils -name '*.o')

# DEPENDENCIES
first: $(ASM_OBJECTS) $(C_OBJECTS)
final: build/bootrec.bin

# Compile all .asm to .elf files.
build_sub/boot_record/%.o: boot_record/%.asm
	$(MAKE_DIR)
	$(NASM) -f elf32 $< -o $@

# Compile all .c to .elf files.
build_sub/boot_record/%.o: boot_record/%.c
	$(MAKE_DIR)
	$(ELF_GCC) $(C_FLAGS) -c $< -o $@

# Link all to the bootrec.elf file.
build_sub/bootrec.elf: boot_record/linker.ld $(ASM_OBJECTS) $(C_OBJECTS) $(EXT_OBJECTS)
	$(MAKE_DIR)
	$(ELF_LD) -T boot_record/linker.ld build_sub/boot_record/kernel_boot.o $(filter-out build_sub/boot_record/kernel_boot.o,$(C_OBJECTS)) $(ASM_OBJECTS) $(EXT_OBJECTS) -o $@

# Convert to binary object.
build/bootrec.bin: build_sub/bootrec.elf
	$(MAKE_DIR)
	$(ELF_OBJCPY) -O binary $< $@


clean:
	rm -rf build_sub/boot_record/
	rm -f build/bootrec.bin