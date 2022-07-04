# Compiler flags
AARCH64_TOOLCHAIN_DIR = build/aarch64-unknown-linux-gnu
ARMGNU = $(AARCH64_TOOLCHAIN_DIR)/bin/aarch64-unknown-linux-gnu
CFLAGS = -Wall -Wextra -ffreestanding -mgeneral-regs-only -MMD -mcpu=$(CPU)
ASMFLAGS = -MMD
LDFLAGS = -nostdlib -nostartfiles

# Machine and emulator targets
CPU = cortex-a53
QEMU_COMMAND = qemu-system-aarch64
QEMU_MACHINE = raspi3b
QEMU_RAM = 1024

# Source directories (add additional subdirectories here)
SRC_DIRS  = src/bootstrapper
SRC_DIRS += src/kernel
SRC_DIRS += src/peripherals
SRC_DIRS += src/common

# Output directories
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
IMAGE = kernel8
BIN_NAME = $(BUILD_DIR)/$(IMAGE).elf
IMG_NAME = $(BUILD_DIR)/$(IMAGE).img

# Setup derived variables
VPATH := src
$(foreach dir,$(SRC_DIRS),$(eval VPATH := $(VPATH):$(dir)))
$(foreach dir,$(SRC_DIRS),$(eval INCLUDES := $(INCLUDES) -I $(dir)))
SOURCES = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))
ASM_SOURCES = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.S))
OBJECTS := $(patsubst %.c, $(OBJ_DIR)/%_c.o, $(notdir $(SOURCES)))
OBJECTS += $(patsubst %.S, $(OBJ_DIR)/%_s.o, $(notdir $(ASM_SOURCES)))
HEADERS = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.h))

# Targets
build: $(OBJECTS) $(HEADERS)
	@echo "==>" linking $(OBJECTS)
	$(ARMGNU)-ld $(LDFLAGS) -T $(BUILD_DIR)/linker.ld -o $(BIN_NAME) $(OBJECTS)
	$(ARMGNU)-objcopy $(BIN_NAME) -O binary $(IMG_NAME)

$(OBJ_DIR)/%_c.o: %.c
	@echo "==>" compiling $<
	@mkdir -p $(@D)
	$(ARMGNU)-gcc $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%_s.o: %.S
	@echo "==>" building $<
	@mkdir -p $(@D)
	$(ARMGNU)-gcc $(ASMFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "==>" removing assets
	rm -rf $(OBJ_DIR)
	rm -f $(BIN_NAME)
	rm -f $(IMG_NAME)

emu: build
	@echo "==>" starting emulator
	$(QEMU_COMMAND) -m $(QEMU_RAM) -machine type=$(QEMU_MACHINE) -serial stdio -kernel $(IMG_NAME)