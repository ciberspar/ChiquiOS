
# ============================================================
# ChiquiOS - Makefile principal
# Arquitecturas: i386 y x86_64
# ============================================================

ARCH ?= i386

CC      := gcc
AS      := nasm
LD      := ld
OBJCOPY := objcopy

BUILD_DIR := build/$(ARCH)

# ------------------------------------------------------------
# FLAGS COMUNES
# ------------------------------------------------------------

CFLAGS_COMMON := \
	-ffreestanding \
	-fno-stack-protector \
	-fno-pic \
	-fno-pie \
	-fno-builtin \
	-fno-asynchronous-unwind-tables \
	-fno-unwind-tables \
	-Wall \
	-Wextra \
	-O2 \
	-Ikernel

LDFLAGS_COMMON := \
	-nostdlib \
	--build-id=none

# ------------------------------------------------------------
# CODIGO COMUN DEL KERNEL
# Todas estas rutas existen en el repositorio
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
# ARQUITECTURA X86-64
# ------------------------------------------------------------

ifeq ($(ARCH),x86_64)

ARCH_CFLAGS := \
	-m64 \
	-march=x86-64 \
	-mno-red-zone \
	-mcmodel=small

ARCH_ASFLAGS := -f elf64

LD_ARCH_FLAGS := -m elf_x86_64

ARCH_SOURCES := \
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

# ------------------------------------------------------------
# ARQUITECTURA I386
# ------------------------------------------------------------

else ifeq ($(ARCH),i386)

ARCH_CFLAGS := \
	-m32 \
	-march=i386

ARCH_ASFLAGS := -f elf32

LD_ARCH_FLAGS := -m elf_i386

ARCH_SOURCES := \
	kernel/arch/i386/arch.c

ARCH_ASM_SOURCES := \
	kernel/entry.asm

LINKER_SCRIPT := kernel/linker.ld

# ------------------------------------------------------------
# ARQUITECTURA NO SOPORTADA
# ------------------------------------------------------------

else

$(error Arquitectura no soportada: $(ARCH))

endif

# ------------------------------------------------------------
# OBJETOS
# ------------------------------------------------------------

CFLAGS := $(CFLAGS_COMMON) $(ARCH_CFLAGS)

C_OBJECTS := \
	$(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES) $(ARCH_SOURCES))

ASM_OBJECTS := \
	$(patsubst %.asm,$(BUILD_DIR)/%.o,$(ARCH_ASM_SOURCES))

KERNEL_ELF := $(BUILD_DIR)/kernel.elf
KERNEL_BIN := $(BUILD_DIR)/kernel.bin

# ------------------------------------------------------------
# OBJETIVO PRINCIPAL
# ------------------------------------------------------------

.PHONY: all clean check iso

all: $(KERNEL_BIN)

# ------------------------------------------------------------
# CREAR DIRECTORIOS
# ------------------------------------------------------------

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# ------------------------------------------------------------
# COMPILAR C
# ------------------------------------------------------------

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# ------------------------------------------------------------
# COMPILAR ENSAMBLADOR NASM
# ------------------------------------------------------------

$(BUILD_DIR)/%.o: %.asm
	mkdir -p $(dir $@)
	$(AS) $(ARCH_ASFLAGS) $< -o $@

# ------------------------------------------------------------
# ENLAZAR KERNEL ELF
# ------------------------------------------------------------

$(KERNEL_ELF): $(C_OBJECTS) $(ASM_OBJECTS) $(LINKER_SCRIPT)
	mkdir -p $(dir $@)
	$(LD) $(LD_ARCH_FLAGS) $(LDFLAGS_COMMON) \
		-T $(LINKER_SCRIPT) \
		-o $@ \
		$(ASM_OBJECTS) \
		$(C_OBJECTS)

# ------------------------------------------------------------
# GENERAR BINARIO
# ------------------------------------------------------------

$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

# ------------------------------------------------------------
# VERIFICACION
# ------------------------------------------------------------

check: $(KERNEL_ELF)
	@echo "======================================"
	@echo " ChiquiOS $(ARCH)"
	@echo " Compilacion y enlace completados"
	@echo "======================================"
	@file $(KERNEL_ELF)

# ------------------------------------------------------------
# ISO
# ------------------------------------------------------------

iso: $(KERNEL_BIN)
	@echo "El kernel esta compilado."
	@echo "La ISO requiere integrar el cargador BIOS/UEFI."
	@echo "Este objetivo todavia no genera una ISO."

# ------------------------------------------------------------
# LIMPIEZA
# ------------------------------------------------------------

clean:
	rm -rf build
	@echo "Compilaciones eliminadas."
