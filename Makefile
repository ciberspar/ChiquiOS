
BUILD := build
ISO := $(BUILD)/chiquios.iso
BOOTIMG := $(BUILD)/iso/boot.img
BOOT_BIN := $(BUILD)/boot.bin
BOOT_CD := $(BUILD)/boot_cd.bin

CC := gcc
LD := ld
AS := nasm
OBJCOPY := objcopy

CFLAGS := -m32 -march=i386 -ffreestanding -fno-pie -fno-pic \
          -fno-stack-protector -fno-asynchronous-unwind-tables \
          -fno-builtin -Wall -Wextra -O2 \
          -std=gnu11 -Ikernel/include

LDFLAGS := -m elf_i386 -T kernel/linker.ld

C_SOURCES := \
	kernel/core/memory.c \
	kernel/core/pmm.c \
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
	kernel/shell/shell.c \
	kernel/ui/tui.c \
	kernel/pkg/bacus.c

ASM_OBJECTS := $(BUILD)/entry.o
C_OBJECTS := $(patsubst kernel/%.c,$(BUILD)/%.o,$(C_SOURCES))
OBJECTS := $(ASM_OBJECTS) $(C_OBJECTS)

.PHONY: all clean check iso

all: $(BUILD)/chiquios.img

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/entry.o: kernel/entry.asm | $(BUILD)
	$(AS) -f elf32 $< -o $@

$(BUILD)/%.o: kernel/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/kernel.elf: $(OBJECTS) kernel/linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

$(BUILD)/kernel.bin: $(BUILD)/kernel.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/kernel.sectors: $(BUILD)/kernel.bin
	@size=$$(stat -c%s $<); \
	if [ $$size -eq 0 ]; then \
		echo "Error: kernel vacío"; exit 1; \
	fi; \
	if [ $$size -gt 524288 ]; then \
		echo "Error: el cargador BIOS actual admite hasta 512 KiB"; \
		exit 1; \
	fi; \
	sectors=$$(( (size + 511) / 512 )); \
	cp $< $@; \
	truncate -s $$((sectors * 512)) $@

$(BUILD)/chiquios.img: boot/boot.asm $(BUILD)/kernel.bin $(BUILD)/kernel.sectors
	@bytes=$$(stat -c%s $(BUILD)/kernel.bin); \
	sectors=$$(( ($$bytes + 511) / 512 )); \
	$(AS) -f bin \
		-D KERNEL_BYTES=$$bytes \
		-D KERNEL_SECTORS=$$sectors \
		boot/boot.asm -o $(BOOT_BIN)
	test "$$(stat -c%s $(BOOT_BIN))" -eq 512
	cat $(BOOT_BIN) $(BUILD)/kernel.sectors > $@

check: $(BUILD)/chiquios.img
	@test "$$(stat -c%s $(BOOT_BIN))" -eq 512
	@test "$$(stat -c%s $(BUILD)/kernel.bin)" -le 524288
	@echo "ChiquiOS: compilación BIOS completada."
	@echo "Kernel: $$(stat -c%s $(BUILD)/kernel.bin) bytes."

# ISO temporalmente deshabilitada:
# boot_cd.asm necesita implementar el mismo protocolo E820
# y boot_info que el cargador BIOS antes de habilitarla.
iso:
	@echo "ISO deshabilitada temporalmente."
	@echo "Primero hay que adaptar y probar boot/boot_cd.asm."
	@exit 1

clean:
	rm -rf $(BUILD)
