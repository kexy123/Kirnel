export PATH := /usr/local/i386-elf/bin:$(PATH)

# COMMANDS
ELF_GCC := i386-elf-gcc

NASM := nasm
MAKE_DIR = @mkdir -p $(dir $@)

C_FLAGS := -ffreestanding -nostdlib -m32 -IkirnelOS/kernel_utils -std=c11

# OBJECTS
C_SOURCES := $(shell find kirnelOS/kernel_utils -name '*.c')
C_OBJECTS := $(C_SOURCES:.c=.o)
C_OBJECTS := $(C_OBJECTS:kirnelOS/kernel_utils/%=build_sub/kirnelOS/kernel_utils/%)

ASM_SOURCES := $(shell find kirnelOS/kernel_utils -name '*.asm')
ASM_OBJECTS := $(ASM_SOURCES:.asm=.o)
ASM_OBJECTS := $(ASM_OBJECTS:kirnelOS/kernel_utils/%=build_sub/kirnelOS/kernel_utils/%)

# DEPENDENCIES
first: $(ASM_OBJECTS) $(C_OBJECTS)

# Compile all .asm to .elf files.
build_sub/kirnelOS/kernel_utils/%.o: kirnelOS/kernel_utils/%.asm
	$(MAKE_DIR)
	$(NASM) -f elf32 $< -o $@

# Compile all .c to .elf files.
build_sub/kirnelOS/kernel_utils/%.o: kirnelOS/kernel_utils/%.c
	$(MAKE_DIR)
	$(ELF_GCC) $(C_FLAGS) -c $< -o $@


clean:
	rm -rf build_sub/kirnelOS/kernel_utils/