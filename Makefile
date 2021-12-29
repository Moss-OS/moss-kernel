# Use the ARM cross compiler
CC = /usr/local/bin/arm-none-eabi-gcc

# Compiler flags
CFLAGS = -mcpu=$(CPU) -fpic -ffreestanding $(DIRECTIVES)
CSRCFLAGS = -O2 -Wall -Wextra
LDFLAGS = -ffreestanding -O2 -nostdlib

# Constants based on Raspberry Pi model.
ifeq ($(RASPI_MODEL),1)
	CPU = arm1176jzf-s
	DIRECTIVES = -D MODEL_1
else
	CPU = cortex-a7
endif

# Source directories (add additional subdirectories here)
SRC_DIRS  = src/kernel
SRC_DIRS += src/common

# Output directories
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
IMG_NAME = $(BUILD_DIR)/kernel.img

# Setup derived variables
VPATH := src
$(foreach dir,$(SRC_DIRS),$(eval VPATH := $(VPATH):$(dir)))
SOURCES = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.c))
ASM_SOURCES = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.S))
OBJECTS := $(patsubst %.c, $(OBJ_DIR)/%.o, $(notdir $(SOURCES)))
OBJECTS += $(patsubst %.S, $(OBJ_DIR)/%.o, $(notdir $(ASM_SOURCES)))
HEADERS = $(foreach dir, $(SRC_DIRS), $(wildcard $(dir)/*.h))

# Targets
build: $(OBJECTS) $(HEADERS)
	@echo $(OBJECTS)
	$(CC) -T $(BUILD_DIR)/linker.ld -o $(IMG_NAME) $(LDFLAGS) $(OBJECTS)

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(SRC_DIRS) -$(HEADERS) -c $< -o $@ $(CSRCFLAGS)

$(OBJ_DIR)/%.o: %.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -I$(SRC_DIRS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(IMG_NAME)

emu: build
	qemu-system-arm -m 1024 -M raspi2b -serial stdio -kernel $(IMG_NAME)