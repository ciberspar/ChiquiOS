
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
          -fno-builtin -Wall -Wextra -O2 -Ikernel/include

LDFLAGS := -m elf_i386 -T kernel/linker.ld

C_SOURCES := \
	kernel/kernel.c \
	kernel/supervisor/supervisor.c \
	kernel/drivers/vga.c \
	kernel/drivers/cpu.c \
	kernel/drivers/keyboard_ps2.c \
	kernel/drivers/keyboard_usb.c \
	kernel/drivers/pci.c \
	kernel/drivers/usb.c \
	kernel/drivers/internet.c \
	kernel/shell/shell.c \
	kernel/drivers/red/e1000.c \
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
	if [ $$size -eq 0 ]; then echo "Error: kernel vacío"; exit 1; fi; \
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
	$(AS) -f bin -D KERNEL_BYTES=$$bytes -D KERNEL_SECTORS=$$sectors boot/boot.asm -o $(BOOT_BIN)
	test "$$(stat -c%s $(BOOT_BIN))" -eq 512
	cat $(BOOT_BIN) $(BUILD)/kernel.sectors > $@

check: $(BUILD)/chiquios.img
	@test "$$(stat -c%s $(BOOT_BIN))" -eq 512
	@test "$$(stat -c%s $(BUILD)/kernel.bin)" -le 524288
	@echo "ChiquiOS: kernel compilado y cargador BIOS generado."
	@echo "Tamaño del kernel: $$(stat -c%s $(BUILD)/kernel.bin) bytes."

iso: $(BUILD)/kernel.bin $(BUILD)/kernel.sectors boot/boot_cd.asm
	mkdir -p $(BUILD)/iso
	@bytes=$$(stat -c%s $(BUILD)/kernel.bin); \
	sectors=$$(( ($$bytes + 511) / 512 )); \
	$(AS) -f bin -D KERNEL_BYTES=$$bytes -D KERNEL_SECTORS=$$sectors boot/boot_cd.asm -o $(BOOT_CD)
	test "$$(stat -c%s $(BOOT_CD))" -eq 512
	@sectors=$$(stat -c%s $(BUILD)/kernel.sectors); \
	sectors=$$((sectors / 512)); \
	dd if=/dev/zero of=$(BOOTIMG) bs=512 count=$$((sectors + 1)) status=none
	dd if=$(BOOT_CD) of=$(BOOTIMG) conv=notrunc status=none
	dd if=$(BUILD)/kernel.sectors of=$(BOOTIMG) bs=512 seek=1 conv=notrunc status=none
	xorriso -as mkisofs \
		-o $(ISO) \
		-b boot.img \
		-c boot.cat \
		-no-emul-boot \
		-boot-load-size 4 \
		$(BUILD)/iso

clean:
	rm -rf $(BUILD)
