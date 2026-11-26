export PATH := /usr/local/i386-elf/bin:$(PATH)

# COMMANDS
ELF_GCC := i386-elf-gcc
ELF_GPP := i386-elf-g++

NASM := nasm
MAKE_DIR = @mkdir -p $(dir $@)

C_FLAGS := -Wall -ffreestanding -nostdlib -m32 -IkirnelOS/sysmgr32/syslib/declaration -std=c23 -fdata-sections -ffunction-sections
CPP_FLAGS := -Wall -ffreestanding -nostdlib -m32 -IkirnelOS/sysmgr32/syslib/declaration -std=c++23 -fdata-sections -ffunction-sections

# OBJECTS
C_SOURCES := $(shell find kirnelOS/sysmgr32/syslib -name '*.c')
C_OBJECTS := $(C_SOURCES:.c=_c.o)
C_OBJECTS := $(C_OBJECTS:kirnelOS/sysmgr32/syslib/%=build_sub/kirnelOS/sysmgr32/syslib/%)

CPP_SOURCES := $(shell find kirnelOS/sysmgr32/syslib -name '*.cpp')
CPP_OBJECTS := $(CPP_SOURCES:.cpp=_cpp.o)
CPP_OBJECTS := $(CPP_OBJECTS:kirnelOS/sysmgr32/syslib/%=build_sub/kirnelOS/sysmgr32/syslib/%)

ASM_SOURCES := $(shell find kirnelOS/sysmgr32/syslib -name '*.asm')
ASM_OBJECTS := $(ASM_SOURCES:.asm=_asm.o)
ASM_OBJECTS := $(ASM_OBJECTS:kirnelOS/sysmgr32/syslib/%=build_sub/kirnelOS/sysmgr32/syslib/%)

# DEPENDENCIES
first: $(ASM_OBJECTS) $(C_OBJECTS) $(CPP_OBJECTS)
final:

# Compile all .asm to .elf files.
build_sub/kirnelOS/sysmgr32/syslib/%_asm.o: kirnelOS/sysmgr32/syslib/%.asm
	$(MAKE_DIR)
	$(NASM) -f elf32 $< -o $@

# Compile all .c to .elf files.
build_sub/kirnelOS/sysmgr32/syslib/%_c.o: kirnelOS/sysmgr32/syslib/%.c
	$(MAKE_DIR)
	$(ELF_GCC) $(C_FLAGS) -c $< -o $@

# Compile all .cpp to .elf files.
build_sub/kirnelOS/sysmgr32/syslib/%_cpp.o: kirnelOS/sysmgr32/syslib/%.cpp
	$(MAKE_DIR)
	$(ELF_GPP) $(CPP_FLAGS) -c $< -o $@


clean:
	rm -rf build_sub/kirnelOS/sysmgr32/syslib/
