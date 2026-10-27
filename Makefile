
BUILD := build
ISO := $(BUILD)/chiquios.iso
BOOTIMG := $(BUILD)/iso/boot.img
BOOT_CD := $(BUILD)/boot_cd.bin

KERNEL_SIZE := 16384
DISK_SIZE := 16896

CC := gcc
LD := ld
AS := nasm
OBJCOPY := objcopy

CFLAGS := -m32 -march=i386 -ffreestanding -fno-pie -fno-pic \
          -fno-stack-protector -fno-asynchronous-unwind-tables \
          -Wall -Wextra -O2 -Ikernel/include

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

$(BUILD)/kernel.padded: $(BUILD)/kernel.bin
	test "$$(stat -c%s $<)" -le $(KERNEL_SIZE)
	cp $< $@
	truncate -s $(KERNEL_SIZE) $@

$(BUILD)/chiquios.img: boot/boot.asm $(BUILD)/kernel.padded
	$(AS) -f bin boot/boot.asm -o $(BUILD)/boot.bin
	test "$$(stat -c%s $(BUILD)/boot.bin)" -eq 512
	cat $(BUILD)/boot.bin $(BUILD)/kernel.padded > $@
	test "$$(stat -c%s $@)" -eq $(DISK_SIZE)

check: $(BUILD)/chiquios.img
	test "$$(stat -c%s $(BUILD)/kernel.bin)" -le $(KERNEL_SIZE)
	test "$$(stat -c%s $(BUILD)/kernel.padded)" -eq $(KERNEL_SIZE)
	test "$$(stat -c%s $(BUILD)/chiquios.img)" -eq $(DISK_SIZE)
	echo "ChiquiOS: comprobaciones correctas"

iso: $(BUILD)/chiquios.img boot/boot_cd.asm
	mkdir -p $(BUILD)/iso
	$(AS) -f bin boot/boot_cd.asm -o $(BOOT_CD)
	test "$$(stat -c%s $(BOOT_CD))" -eq 512
	test "$$(stat -c%s $(BUILD)/chiquios.img)" -eq $(DISK_SIZE)
	dd if=/dev/zero of=$(BOOTIMG) bs=512 count=33 status=none
	dd if=$(BOOT_CD) of=$(BOOTIMG) conv=notrunc status=none
	dd if=$(BUILD)/chiquios.img of=$(BOOTIMG) bs=512 \
		skip=1 seek=1 count=32 conv=notrunc status=none
	xorriso -as mkisofs \
		-o $(ISO) \
		-b boot.img \
		-c boot.cat \
		-no-emul-boot \
		-boot-load-size 33 \
		$(BUILD)/iso

clean:
	rm -rf $(BUILD)
