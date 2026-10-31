
ARCH ?= i386

BUILD := build
CC := gcc
LD := ld
AS := nasm
OBJCOPY := objcopy

C_SOURCES := \
	kernel/core/memory.c \
	kernel/core/pmm.c \
	kernel/core/driver_manager.c \
	kernel/kernel.c \
	kernel/supervisor/supervisor.c \
	kernel/drivers/vga.c \
	kernel/drivers/cpu.c \
	kernel/drivers/keyboard_ps2.c \
	kernel/drivers/keyboard_usb.c \
	kernel/drivers/pci.c \
	kernel/drivers/usb.c \
	kernel/drivers/internet.c \
	kernel/drivers/red/e1000.c \
	kernel/core/memory_monitor.c \
	kernel/shell/shell.c \
	kernel/ui/tui.c \
	kernel/pkg/bacus.c

COMMON_CFLAGS := \
	-ffreestanding \
	-fno-pie \
	-fno-pic \
	-fno-stack-protector \
	-fno-asynchronous-unwind-tables \
	-fno-builtin \
	-Wall -Wextra -O2 \
	-std=gnu11 \
	-Ikernel/include

.PHONY: all clean check iso

# ==================================================
# i386 BIOS
# ==================================================

ifeq ($(ARCH),i386)

BUILD_DIR := $(BUILD)
ARCH_SOURCE := kernel/arch/i386/arch.c
ENTRY_SOURCE := kernel/entry.asm
LINKER_SCRIPT := kernel/linker.ld

CFLAGS := $(COMMON_CFLAGS) -m32 -march=i386
LDFLAGS := -m elf_i386 -T $(LINKER_SCRIPT)

BOOT_BIN := $(BUILD_DIR)/boot.bin

C_OBJECTS := $(patsubst kernel/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
C_OBJECTS += $(BUILD_DIR)/arch/i386/arch.o

ASM_OBJECTS := $(BUILD_DIR)/entry.o
OBJECTS := $(ASM_OBJECTS) $(C_OBJECTS)

all: $(BUILD_DIR)/chiquios.img

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/entry.o: $(ENTRY_SOURCE) | $(BUILD_DIR)
	$(AS) -f elf32 $< -o $@

$(BUILD_DIR)/%.o: kernel/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/kernel.elf: $(OBJECTS) $(LINKER_SCRIPT)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD_DIR)/kernel.sectors: $(BUILD_DIR)/kernel.bin
	@size=$$(stat -c%s $<); \
	if [ $$size -eq 0 ]; then \
		echo "ERROR: kernel vacio"; exit 1; \
	fi; \
	if [ $$size -gt 524288 ]; then \
		echo "ERROR: kernel supera el limite del cargador BIOS actual"; \
		exit 1; \
	fi; \
	sectors=$$(( (size + 511) / 512 )); \
	cp $< $@; \
	truncate -s $$((sectors * 512)) $@

$(BUILD_DIR)/chiquios.img: boot/boot.asm $(BUILD_DIR)/kernel.bin $(BUILD_DIR)/kernel.sectors
	@bytes=$$(stat -c%s $(BUILD_DIR)/kernel.bin); \
	sectors=$$(( ($$bytes + 511) / 512 )); \
	$(AS) -f bin \
		-D KERNEL_BYTES=$$bytes \
		-D KERNEL_SECTORS=$$sectors \
		boot/boot.asm -o $(BOOT_BIN)
	test "$$(stat -c%s $(BOOT_BIN))" -eq 512
	cat $(BOOT_BIN) $(BUILD_DIR)/kernel.sectors > $@

check: $(BUILD_DIR)/chiquios.img
	@test "$$(stat -c%s $(BOOT_BIN))" -eq 512
	@test "$$(stat -c%s $(BUILD_DIR)/kernel.bin)" -le 524288
	@readelf -h $(BUILD_DIR)/kernel.elf | grep -q "ELF32"
	@echo "ChiquiOS i386 BIOS: compilacion completada."

iso:
	@echo "ISO i386 todavia no implementada."
	@exit 1

# ==================================================
# x86-64
# ==================================================

else ifeq ($(ARCH),x86_64)

BUILD_DIR := $(BUILD)/x86_64
ARCH_SOURCE := kernel/arch/x86_64/arch.c
ENTRY_SOURCE := kernel/arch/x86_64/entry.asm
LINKER_SCRIPT := kernel/arch/x86_64/linker.ld

CFLAGS := $(COMMON_CFLAGS) \
	-m64 \
	-march=x86-64 \
	-mno-red-zone \
	-mcmodel=small

LDFLAGS := -m elf_x86_64 -T $(LINKER_SCRIPT)

C_OBJECTS := $(patsubst kernel/%.c,$(BUILD_DIR)/kernel/%.o,$(C_SOURCES))
C_OBJECTS += $(BUILD_DIR)/kernel/arch/x86_64/arch.o

ASM_OBJECTS := $(BUILD_DIR)/entry.o
OBJECTS := $(ASM_OBJECTS) $(C_OBJECTS)

all: $(BUILD_DIR)/kernel.elf $(BUILD_DIR)/kernel.bin

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/entry.o: $(ENTRY_SOURCE) | $(BUILD_DIR)
	$(AS) -f elf64 $< -o $@

$(BUILD_DIR)/kernel/%.o: kernel/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/kernel.elf: $(OBJECTS) $(LINKER_SCRIPT)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

$(BUILD_DIR)/kernel.bin: $(BUILD_DIR)/kernel.elf
	$(OBJCOPY) -O binary $< $@
	@test -s $@

check: $(BUILD_DIR)/kernel.elf $(BUILD_DIR)/kernel.bin
	@readelf -h $(BUILD_DIR)/kernel.elf | grep -q "ELF64"
	@readelf -h $(BUILD_DIR)/kernel.elf | grep -q "Advanced Micro Devices X86-64"
	@nm $(BUILD_DIR)/kernel.elf | grep -q " T _start"
	@nm $(BUILD_DIR)/kernel.elf | grep -q " T kernel_main"
	@test -s $(BUILD_DIR)/kernel.bin
	@echo "ChiquiOS x86-64: ELF y BIN verificados."

iso:
	@echo "ISO x86-64 todavia no implementada."
	@exit 1

else

$(error Arquitectura no soportada: $(ARCH). Usa ARCH=i386 o ARCH=x86_64)

endif

clean:
	rm -rf $(BUILD)

-include $(C_OBJECTS:.o=.d)
