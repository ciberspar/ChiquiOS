
ARCH ?= i386

CC      = gcc
AS      = nasm
LD      = ld
OBJCOPY = objcopy
XORRISO = xorriso

BUILD_DIR = build/$(ARCH)
ISO_DIR   = $(BUILD_DIR)/iso

CFLAGS_COMMON = -ffreestanding \
                -fno-stack-protector \
                -fno-pic \
                -fno-pie \
                -Wall \
                -Wextra \
                -O2 \
                -Ikernel

LDFLAGS_COMMON = -nostdlib

C_SOURCES = \
	kernel/kernel.c \
	kernel/core/memory.c \
	kernel/core/pmm.c \
	kernel/core/driver_manager.c \
	kernel/core/supervisor.c \
	kernel/drivers/vga.c \
	kernel/drivers/serial.c \
	kernel/drivers/cpu.c \
	kernel/drivers/keyboard.c \
	kernel/drivers/pci.c \
	kernel/drivers/usb.c \
	kernel/drivers/internet.c \
	kernel/drivers/e1000.c \
	kernel/drivers/memory_monitor.c \
	kernel/shell/shell.c \
	kernel/ui/tui.c \
	kernel/bacus/bacus.c

ifeq ($(ARCH),x86_64)

ARCH_CFLAGS = -m64 \
              -march=x86-64 \
              -mno-red-zone \
              -mcmodel=small

ARCH_ASFLAGS = -f elf64

ARCH_SOURCES = \
	kernel/arch/x86_64/arch.c \
	kernel/arch/x86_64/gdt.c \
	kernel/arch/x86_64/tss.c \
	kernel/arch/x86_64/idt.c \
	kernel/arch/x86_64/exceptions.c

ARCH_ASM_SOURCES = \
	kernel/arch/x86_64/entry.asm \
	kernel/arch/x86_64/gdt_load.asm \
	kernel/arch/x86_64/interrups.asm

LINKER_SCRIPT = kernel/arch/x86_64/linker.ld

else ifeq ($(ARCH),i386)

ARCH_CFLAGS = -m32 -march=i386

ARCH_ASFLAGS = -f elf32

ARCH_SOURCES = \
	kernel/arch/i386/arch.c

ARCH_ASM_SOURCES = \
	kernel/entry.asm

LINKER_SCRIPT = kernel/linker.ld

else
$(error Arquitectura no soportada: $(ARCH))
endif

CFLAGS = $(CFLAGS_COMMON) $(ARCH_CFLAGS)

C_OBJECTS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES) $(ARCH_SOURCES))
ASM_OBJECTS = $(patsubst %.asm,$(BUILD_DIR)/%.o,$(ARCH_ASM_SOURCES))

KERNEL_ELF = $(BUILD_DIR)/kernel.elf
KERNEL_BIN = $(BUILD_DIR)/kernel.bin
ISO_IMAGE  = $(BUILD_DIR)/ChiquiOS-$(ARCH).iso

.PHONY: all clean iso check

all: $(KERNEL_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(ISO_DIR):
	mkdir -p $(ISO_DIR)

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.asm
	mkdir -p $(dir $@)
	$(AS) $(ARCH_ASFLAGS) $< -o $@

$(KERNEL_ELF): $(C_OBJECTS) $(ASM_OBJECTS) $(LINKER_SCRIPT)
	$(LD) $(LDFLAGS_COMMON) \
		-T $(LINKER_SCRIPT) \
		-o $@ \
		$(ASM_OBJECTS) \
		$(C_OBJECTS)

$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

check: $(KERNEL_ELF)
	@echo "ChiquiOS $(ARCH): compilacion y enlace completados."

clean:
	rm -rf build

iso: $(KERNEL_BIN) | $(ISO_DIR)
	@echo "Preparando ISO de ChiquiOS..."
	@echo "La creacion de la ISO requiere integrar el cargador BIOS/UEFI."
	@echo "Kernel generado: $(KERNEL_BIN)"
