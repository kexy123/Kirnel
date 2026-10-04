export PATH := /usr/local/i386-elf/bin:$(PATH)

# COMMANDS
ELF_GCC := i386-elf-gcc

NASM := nasm
MAKE_DIR = @mkdir -p $(dir $@)

C_FLAGS := -ffreestanding -nostdlib -m32 -IkirnelOS/sysmgr32/krnllib -IkirnelOS/sysmgr32/syslib -std=c23 -fdata-sections -ffunction-sections

# OBJECTS
C_SOURCES := $(shell find kirnelOS/sysmgr32/krnllib -name '*.c')
C_OBJECTS := $(C_SOURCES:.c=.o)
C_OBJECTS := $(C_OBJECTS:kirnelOS/sysmgr32/krnllib/%=build_sub/kirnelOS/sysmgr32/krnllib/%)

ASM_SOURCES := $(shell find kirnelOS/sysmgr32/krnllib -name '*.asm')
ASM_OBJECTS := $(ASM_SOURCES:.asm=.o)
ASM_OBJECTS := $(ASM_OBJECTS:kirnelOS/sysmgr32/krnllib/%=build_sub/kirnelOS/sysmgr32/krnllib/%)

# DEPENDENCIES
first: $(ASM_OBJECTS) $(C_OBJECTS)
final:

# Compile all .asm to .elf files.
build_sub/kirnelOS/sysmgr32/krnllib/%.o: kirnelOS/sysmgr32/krnllib/%.asm
	$(MAKE_DIR)
	$(NASM) -f elf32 $< -o $@

# Compile all .c to .elf files.
build_sub/kirnelOS/sysmgr32/krnllib/%.o: kirnelOS/sysmgr32/krnllib/%.c
	$(MAKE_DIR)
	$(ELF_GCC) $(C_FLAGS) -c $< -o $@


clean:
	rm -rf build_sub/kirnelOS/sysmgr32/krnllib/