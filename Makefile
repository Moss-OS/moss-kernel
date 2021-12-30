# Compiler flags
AARCH64_TOOLCHAIN_DIR = build/aarch64-unknown-linux-gnu
ARM_GCC = $(AARCH64_TOOLCHAIN_DIR)/bin/aarch64-unknown-linux-gnu-gcc
CFLAGS = -mcpu=$(CPU) -fpic -ffreestanding
CSRCFLAGS = -O2 -Wall -Wextra
LDFLAGS = -ffreestanding -O2 -nostdlib

# Machine and emulator targets
CPU = cortex-a53
QEMU_COMMAND = qemu-system-aarch64
QEMU_MACHINE = raspi3b
QEMU_RAM = 1024

# Source directories (add additional subdirectories here)
SRC_DIRS  = src/bootstrapper
SRC_DIRS += src/kernel
SRC_DIRS += src/common

# Output directories
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
IMG_NAME = $(BUILD_DIR)/kernel.img

# Setup derived variables
VPATH := src
$(foreach dir,$(SRC_DIRS),$(eval VPATH := $(VPATH):$(dir)))
$(foreach dir,$(SRC_DIRS),$(eval INCLUDES := $(INCLUDES) -I $(dir)))
SOURCES = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))
ASM_SOURCES = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.S))
OBJECTS := $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SOURCES)))
OBJECTS += $(patsubst %.S, $(OBJ_DIR)/%.o, $(notdir $(ASM_SOURCES)))
HEADERS = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.h))

# Targets
build: $(OBJECTS) $(HEADERS)
	@echo "==>" linking $(OBJECTS)
	$(ARM_GCC) -T $(BUILD_DIR)/linker.ld -o $(IMG_NAME) $(LDFLAGS) $(OBJECTS)

$(OBJ_DIR)/%.o: %.c
	@echo "==>" compiling $<
	@mkdir -p $(@D)
	$(ARM_GCC) $(CFLAGS) $(INCLUDES) -c $< -o $@ $(CSRCFLAGS)

$(OBJ_DIR)/%.o: %.S
	@echo "==>" building $<
	@mkdir -p $(@D)
	$(ARM_GCC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "==>" removing assets
	rm -rf $(OBJ_DIR)
	rm -f $(IMG_NAME)

emu: build
	@echo "==>" starting emulator
	$(QEMU_COMMAND) -m $(QEMU_RAM) -machine type=$(QEMU_MACHINE) -serial stdio -kernel $(IMG_NAME)