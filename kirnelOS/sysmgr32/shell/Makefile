export PATH := /usr/local/i386-elf/bin:$(PATH)

# COMMANDS
ELF_GCC := i386-elf-g++
ELF_LD := i386-elf-ld
ELF_OBJCPY := i386-elf-objcopy

NASM := nasm
MAKE_DIR = @mkdir -p $(dir $@)

CPP_FLAGS := -Wall -ffreestanding -nostdlib -m32 -IkirnelOS/sysmgr32/shell -std=c++23 -fdata-sections -ffunction-sections

# OBJECTS
CPP_SOURCES := $(shell find kirnelOS/sysmgr32/shell -name '*.cpp')
CPP_OBJECTS := $(CPP_SOURCES:.cpp=.o)
CPP_OBJECTS := $(CPP_OBJECTS:kirnelOS/%=build_sub/kirnelOS/%)

EXT_OBJECTS := $(shell find build_sub/kirnelOS/sysmgr32/syslib -name '*.o')

# DEPENDENCIES
first: $(CPP_OBJECTS) $(ASM_OBJECTS)
final: build/kirnelOS/sysmgr32/shell.bin

# Compile all .cpp to .elf files.
build_sub/kirnelOS/%.o: kirnelOS/%.cpp
	$(MAKE_DIR)
	$(ELF_GCC) $(CPP_FLAGS) -c $< -o $@

# Link all to shell.elf file.
build_sub/kirnelOS/sysmgr32/shell.elf: kirnelOS/sysmgr32/shell/linker.ld $(CPP_OBJECTS)
	$(MAKE_DIR)
	$(ELF_LD) --gc-sections -T kirnelOS/sysmgr32/shell/linker.ld build_sub/kirnelOS/sysmgr32/shell/entry.o $(filter-out build_sub/kirnelOS/sysmgr32/shell/entry.o,$(CPP_OBJECTS)) $(EXT_OBJECTS) -o $@

# Convert to binary object.
build/kirnelOS/sysmgr32/shell.bin: build_sub/kirnelOS/sysmgr32/shell.elf
	$(MAKE_DIR)
	$(ELF_OBJCPY) -O binary $< $@


clean:
	rm -rf build_sub/kirnelOS/sysmgr32/shell/
	rm -f build/kirnelOS/sysmgr32/shell.bin
