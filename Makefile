
# ============================================================
# ChiquiOS - Multilanguage Kernel Build
# Architectures: i386 / x86_64
# ============================================================

ARCH ?= i386

CC      := gcc
CXX     := g++
AS      := nasm
LD      := ld
OBJCOPY := objcopy

BUILD_DIR := build/$(ARCH)

CPPFLAGS := -Ikernel -Ikernel/include

COMMON_FLAGS := \
	-ffreestanding \
	-fno-stack-protector \
	-fno-pic \
	-fno-pie \
	-fno-builtin \
	-fno-common \
	-fno-asynchronous-unwind-tables \
	-fno-unwind-tables \
	-Wall \
	-Wextra \
	-O2

CFLAGS := $(COMMON_FLAGS) -std=gnu11

CXXFLAGS := $(COMMON_FLAGS) \
	-std=c++17 \
	-fno-exceptions \
	-fno-rtti \
	-fno-use-cxa-atexit \
	-fno-threadsafe-statics

LDFLAGS := -nostdlib --build-id=none

# ------------------------------------------------------------
# COMMON C SOURCES
# ------------------------------------------------------------

C_SOURCES := \
	kernel/kernel.c \
	kernel/core/memory.c \
	kernel/core/pmm.c \
	kernel/core/driver_manager.c \
	kernel/core/memory_monitor.c \
	kernel/supervisor/supervisor.c \
	kernel/drivers/vga.c \
	kernel/drivers/serial.c \
	kernel/drivers/cpu.c \
	kernel/drivers/keyboard_ps2.c \
	kernel/drivers/keyboard_usb.c \
	kernel/drivers/pci.c \
	kernel/drivers/usb.c \
	kernel/drivers/internet.c \
	kernel/drivers/red/e1000.c \
	kernel/shell/shell.c \
	kernel/ui/tui.c \
	kernel/pkg/bacus.c

# ------------------------------------------------------------
# C++ SOURCES
# ------------------------------------------------------------

CXX_SOURCES := \
	kernel/cpp/system_info.cpp

# ------------------------------------------------------------
# ARCHITECTURE CONFIGURATION
# ------------------------------------------------------------

ifeq ($(ARCH),i386)

ARCH_FLAGS := \
	-m32 \
	-march=i386 \
	-mno-sse \
	-mno-mmx

AS_FORMAT := elf32
LD_FORMAT := elf_i386

ARCH_C_SOURCES := \
	kernel/arch/i386/arch.c

ARCH_ASM_SOURCES := \
	kernel/entry.asm

LINKER_SCRIPT := kernel/linker.ld

else ifeq ($(ARCH),x86_64)

ARCH_FLAGS := \
	-m64 \
	-march=x86-64 \
	-mno-red-zone \
	-mno-sse \
	-mno-mmx

AS_FORMAT := elf64
LD_FORMAT := elf_x86_64

ARCH_C_SOURCES := \
	kernel/arch/x86_64/arch.c \
	kernel/arch/x86_64/gdt.c \
	kernel/arch/x86_64/tss.c \
	kernel/arch/x86_64/idt.c \
	kernel/arch/x86_64/exceptions.c

ARCH_ASM_SOURCES := \
	kernel/arch/x86_64/entry.asm \
	kernel/arch/x86_64/gdt_load.asm \
	kernel/arch/x86_64/interrups.asm

LINKER_SCRIPT := kernel/arch/x86_64/linker.ld

else
$(error Unsupported architecture: $(ARCH))
endif

# ------------------------------------------------------------
# OBJECTS
# ------------------------------------------------------------

C_OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,\
	$(C_SOURCES) $(ARCH_C_SOURCES))

CXX_OBJECTS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,\
	$(CXX_SOURCES))

ASM_OBJECTS := $(patsubst %.asm,$(BUILD_DIR)/%.o,\
	$(ARCH_ASM_SOURCES))

ALL_OBJECTS := \
	$(ASM_OBJECTS) \
	$(C_OBJECTS) \
	$(CXX_OBJECTS)

KERNEL_ELF := $(BUILD_DIR)/kernel.elf
KERNEL_BIN := $(BUILD_DIR)/kernel.bin

# ------------------------------------------------------------
# BUILD
# ------------------------------------------------------------

.PHONY: all clean check

all: $(KERNEL_BIN)

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(ARCH_FLAGS) \
		-MMD -MP -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(ARCH_FLAGS) \
		-MMD -MP -c $< -o $@

$(BUILD_DIR)/%.o: %.asm
	mkdir -p $(dir $@)
	$(AS) -f $(AS_FORMAT) $< -o $@

$(KERNEL_ELF): $(ALL_OBJECTS) $(LINKER_SCRIPT)
	mkdir -p $(dir $@)
	$(LD) -m $(LD_FORMAT) $(LDFLAGS) \
		-T $(LINKER_SCRIPT) \
		-o $@ \
		$(ALL_OBJECTS)

$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

# ------------------------------------------------------------
# VALIDATION
# ------------------------------------------------------------

check: $(KERNEL_ELF) $(KERNEL_BIN)
	@echo "=================================="
	@echo " ChiquiOS $(ARCH)"
	@echo " Build completed"
	@echo "=================================="
	readelf -h $(KERNEL_ELF)
	nm -g $(KERNEL_ELF) | grep -q "kernel_main"
	nm -g $(KERNEL_ELF) | grep -q "chiquios_cpp_module_name"

# ------------------------------------------------------------
# CLEAN
# ------------------------------------------------------------

clean:
	rm -rf build
	@echo "Build directory cleaned."

-include $(shell find build -name '*.d' 2>/dev/null)
