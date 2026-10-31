
ARCH ?= i386

BUILD := build
CC := gcc
LD := ld
AS := nasm
OBJCOPY := objcopy

C_SOURCES := \
  kernel/drivers/serial.c \
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
# i386 BIOS (se conserva)
# ==================================================

ifeq ($(ARCH),i386)

BUILD_DIR := $(BUILD)
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
	if [ $$size -eq 0 ] || [ $$size -gt 524288 ]; then \
		echo "ERROR: tamano de kernel no admitido"; exit 1; \
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
	@readelf -h $(BUILD_DIR)/kernel.elf | grep -q ELF32
	@echo "ChiquiOS i386: compilacion completada."

iso:
	@echo "Usa make ARCH=x86_64 iso para generar el ISO x86-64."

# ==================================================
# x86-64 BIOS
# ==================================================

else ifeq ($(ARCH),x86_64)

BUILD_DIR := $(BUILD)/x86_64
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

KERNEL_BIN := $(BUILD_DIR)/kernel.bin
BOOT_BIN := $(BUILD_DIR)/boot.bin
STAGE2_BIN := $(BUILD_DIR)/stage2.bin
BOOT_IMAGE := $(BUILD_DIR)/boot/boot.img
ISO_ROOT := $(BUILD_DIR)/iso
ISO_FILE := $(BUILD_DIR)/ChiquiOS-x86_64.iso

all: $(ISO_FILE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/entry.o: $(ENTRY_SOURCE) | $(BUILD_DIR)
	$(AS) -f elf64 $< -o $@

$(BUILD_DIR)/kernel/%.o: kernel/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/kernel.elf: $(OBJECTS) $(LINKER_SCRIPT)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

$(KERNEL_BIN): $(BUILD_DIR)/kernel.elf
	$(OBJCOPY) -O binary $< $@
	@test -s $@

# Cargador BIOS de 512 bytes.
$(BOOT_BIN): boot/x86_64/boot.asm
	mkdir -p $(BUILD_DIR)
	$(AS) -f bin $< -o $@
	test "$$(stat -c%s $@)" -eq 512

# Stage2: exactamente 32 sectores.
$(STAGE2_BIN): boot/x86_64/stage2.asm $(KERNEL_BIN)
	@bytes=$$(stat -c%s $(KERNEL_BIN)); \
	sectors=$$(( (bytes + 511) / 512 )); \
	$(AS) -f bin \
		-D KERNEL_BYTES=$$bytes \
		-D KERNEL_SECTORS=$$sectors \
		$< -o $@
	test "$$(stat -c%s $@)" -eq 16384

# Imagen El Torito interna:
# LBA 0 = boot sector
# LBA 1-32 = Stage2
# LBA 33 en adelante = kernel
$(BOOT_IMAGE): $(BOOT_BIN) $(STAGE2_BIN) $(KERNEL_BIN)
	mkdir -p $(dir $@)
	truncate -s 1474560 $@
	dd if=$(BOOT_BIN) of=$@ conv=notrunc status=none
	dd if=$(STAGE2_BIN) of=$@ bs=512 seek=1 conv=notrunc status=none
	dd if=$(KERNEL_BIN) of=$@ bs=512 seek=33 conv=notrunc status=none

# Construir ISO BIOS con emulacion de disquete El Torito.
$(ISO_FILE): $(BOOT_IMAGE)
	rm -rf $(ISO_ROOT)
	mkdir -p $(ISO_ROOT)/boot
	cp $(BOOT_IMAGE) $(ISO_ROOT)/boot/boot.img
	xorriso -as mkisofs \
		-V CHIQUIOS_X64 \
		-b boot/boot.img \
		-c boot/boot.catalog \
		-o $@ \
		$(ISO_ROOT)
	test -s $@

check: $(BUILD_DIR)/kernel.elf $(KERNEL_BIN) $(ISO_FILE)
	@readelf -h $(BUILD_DIR)/kernel.elf | grep -q ELF64
	@readelf -h $(BUILD_DIR)/kernel.elf | grep -q "Advanced Micro Devices X86-64"
	@nm $(BUILD_DIR)/kernel.elf | grep -q " T _start"
	@nm $(BUILD_DIR)/kernel.elf | grep -q " T kernel_main"
	@test -s $(ISO_FILE)
	@echo "ChiquiOS x86-64 BIOS: ISO generado y kernel verificado."

iso: $(ISO_FILE)

else

$(error Arquitectura no soportada: $(ARCH))

endif

clean:
	rm -rf $(BUILD)

-include $(C_OBJECTS:.o=.d)
