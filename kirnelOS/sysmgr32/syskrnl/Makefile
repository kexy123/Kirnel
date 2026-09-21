export PATH := /usr/local/i386-elf/bin:$(PATH)

# COMMANDS
ELF_GCC := i386-elf-gcc
ELF_LD := i386-elf-ld
ELF_OBJCPY := i386-elf-objcopy

NASM := nasm
MAKE_DIR = mkdir -p $(dir $@)

C_FLAGS := -ffreestanding -nostdlib -m32 -IkirnelOS/sysmgr32/syskrnl -IkirnelOS/kernel_utils -std=c11

# OBJECTS
C_SOURCES := $(shell find kirnelOS/sysmgr32/syskrnl -name '*.c')
C_OBJECTS := $(C_SOURCES:.c=.o)
C_OBJECTS := $(C_OBJECTS:kirnelOS/%=build_sub/kirnelOS/%)

ASM_SOURCES := $(shell find kirnelOS/sysmgr32/syskrnl -name '*.asm')
ASM_OBJECTS := $(ASM_SOURCES:.asm=.o)
ASM_OBJECTS := $(ASM_OBJECTS:kirnelOS/%=build_sub/kirnelOS/%)

EXT_OBJECTS := $(shell find build_sub/kirnelOS/kernel_utils -name '*.o')

# DEPENDENCIES
first: $(C_OBJECTS) $(ASM_OBJECTS)
final: build/kirnelOS/sysmgr32/syskrnl.bin

# Compile all .asm to .elf files.
build_sub/kirnelOS/%.o: kirnelOS/%.asm
	$(MAKE_DIR)
	$(NASM) -f elf32 $< -o $@

# Compile all .c to .elf files.
build_sub/kirnelOS/%.o: kirnelOS/%.c
	$(MAKE_DIR)
	$(ELF_GCC) $(C_FLAGS) -c $< -o $@

# Link all to syskrnl.elf file.
build_sub/kirnelOS/sysmgr32/syskrnl.elf: kirnelOS/sysmgr32/syskrnl/linker.ld $(C_OBJECTS) $(ASM_OBJECTS)
	$(MAKE_DIR)
	$(ELF_LD) -T kirnelOS/sysmgr32/syskrnl/linker.ld build_sub/kirnelOS/sysmgr32/syskrnl/entry.o $(filter-out build_sub/kirnelOS/sysmgr32/syskrnl/entry.o,$(C_OBJECTS) $(ASM_OBJECTS)) -o $@

# Convert to binary object.
build/kirnelOS/sysmgr32/syskrnl.bin: build_sub/kirnelOS/sysmgr32/syskrnl.elf
	$(MAKE_DIR)
	$(ELF_OBJCPY) -O binary $< $@


clean:
	rm -rf build_sub/kirnelOS/sysmgr32/syskrnl/
	rm -f build/kirnelOS/sysmgr32/syskrnl.bin