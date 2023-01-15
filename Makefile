# Compiler flags
AARCH64_TOOLCHAIN_DIR =
ARMGNU = aarch64-linux-gnu
CFLAGS = -Iinclude -Wall -Wextra -ffreestanding -mgeneral-regs-only -mcpu=$(CPU) -g
ASMFLAGS = -Iinclude -g
LDFLAGS = -nostdlib

# Machine and emulator targets
CPU = cortex-a53
QEMU_COMMAND = /mnt/c/Program\ Files/qemu/qemu-system-aarch64.exe

# Source directories (add additional subdirectories here)
SRC_DIRS  = src/bootstrapper
SRC_DIRS += src/kernel
SRC_DIRS += src/peripherals
SRC_DIRS += src/common
SRC_DIRS += src/process

# Output directories
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
IMAGE = kernel8
BIN_NAME = $(BUILD_DIR)/$(IMAGE).elf
IMG_NAME = $(BUILD_DIR)/$(IMAGE).img
LST_NAME = $(BUILD_DIR)/$(IMAGE).lst

# Setup derived variables
VPATH := src
$(foreach dir,$(SRC_DIRS),$(eval VPATH := $(VPATH):$(dir)))
SOURCES = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))
ASM_SOURCES = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.S))
OBJECTS := $(patsubst %.c, $(OBJ_DIR)/%_c.o, $(notdir $(SOURCES)))
OBJECTS += $(patsubst %.S, $(OBJ_DIR)/%_s.o, $(notdir $(ASM_SOURCES)))

# Targets
build: $(OBJECTS) $(HEADERS)
	@echo "==>" linking $(OBJECTS)
	$(ARMGNU)-ld $(LDFLAGS) -T $(BUILD_DIR)/linker.ld -o $(BIN_NAME) $(OBJECTS)
	$(ARMGNU)-objdump -D $(BIN_NAME) > $(LST_NAME)
	$(ARMGNU)-objcopy $(BIN_NAME) -O binary $(IMG_NAME)

$(OBJ_DIR)/%_c.o: %.c
	@echo "==>" compiling $<
	@mkdir -p $(@D)
	$(ARMGNU)-gcc $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%_s.o: %.S
	@echo "==>" building $<
	@mkdir -p $(@D)
	$(ARMGNU)-gcc $(ASMFLAGS) -c $< -o $@

clean:
	@echo "==>" removing assets
	rm -rf $(OBJ_DIR)
	rm -f $(BIN_NAME)
	rm -f $(IMG_NAME)
	rm -f $(LST_NAME)

emu: build
	@echo "==>" starting emulator
	$(QEMU_COMMAND) -m 1024 -machine type=raspi3b -serial stdio -kernel $(IMG_NAME)

debug: build
	@echo "==>" starting emulator
	$(QEMU_COMMAND) -m 1024 -machine type=raspi3b -serial stdio -kernel $(IMG_NAME) -s -S

deploy: build
	@echo "==>" copying to SD card
	cp -f ./build/kernel8.img /Volumes/boot/
	cp -f ./build/config.txt /Volumes/boot/
	sync
	diskutil unmount /Volumes/boot/
	@echo "==>" SD card ready