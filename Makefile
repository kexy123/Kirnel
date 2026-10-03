export PATH := /usr/local/i386-elf/bin:$(PATH)

# COMMANDS
ELF_GCC := i386-elf-gcc
ELF_LD := i386-elf-ld
ELF_OBJCPY := i386-elf-objcopy

NASM := nasm
MAKE_DIR = @mkdir -p $(dir $@)

# OBJECTS
SUBBUILD_LOCATIONS := $(shell find . -mindepth 2 -name Makefile -printf '%h\n')

# DEPENDENCIES
all: floppy.img

# Compile boot.asm.
build/boot.bin: boot.asm
	$(NASM) -f bin $< -o $@

# Compile everything in kirnelOS.
.PHONY: build_all

build_all:
	@for dir in $(SUBBUILD_LOCATIONS); do \
		$(MAKE) first -f "$$dir/Makefile"; \
	done

# Run final parts of the project. This is to ensure that they have the necessary files to link when the first pass was executed.
	@for dir in $(SUBBUILD_LOCATIONS); do \
		$(MAKE) final -f "$$dir/Makefile"; \
	done

# Compile the boot segment.
build/os.img: build/boot.bin build/bootrec.bin
	$(MAKE_DIR)
	cat $^ > $@
# 16 sectors
	truncate -s 16384 $@

# Compile to the floppy image.
floppy.img: build_all build/os.img
	truncate -s 1440K floppy.img
	mkfs.fat -F 12 -R 32 -S 512 floppy.img

	dd if=build/os.img of=floppy.img bs=512 count=32 conv=notrunc

	mmd -i floppy.img ::/kirnelOS
	mcopy -i floppy.img -s build/kirnelOS/* ::/kirnelOS


clean:
	rm -rf build/ build_sub/