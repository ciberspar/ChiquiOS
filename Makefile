
NASM = nasm
CC = gcc
LD = ld
OBJCOPY = objcopy

BUILD = build
KERNEL_MAX = 16384

CFLAGS = -m32 -march=i386 -ffreestanding -fno-pie \
         -fno-pic -fno-stack-protector -fno-builtin \
         -fno-asynchronous-unwind-tables -O2 -Wall -Wextra

KERNEL_OBJS = \
	$(BUILD)/entry.o \
	$(BUILD)/kernel.o \
	$(BUILD)/cpu.o \
	$(BUILD)/vga.o \
	$(BUILD)/keyboard_ps2.o \
	$(BUILD)/pci.o \
	$(BUILD)/shell.o

.PHONY: all check clean

all: $(BUILD)/chiquios.img

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.bin: boot/boot.asm | $(BUILD)
	$(NASM) -f bin $< -o $@
	test "$$(stat -c%s $@)" -eq 512

$(BUILD)/entry.o: kernel/entry.asm | $(BUILD)
	$(NASM) -f elf32 $< -o $@

$(BUILD)/kernel.o: kernel/kernel.c kernel/include/drivers.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/cpu.o: kernel/drivers/cpu.c kernel/include/drivers.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/vga.o: kernel/drivers/vga.c kernel/include/drivers.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/keyboard_ps2.o: kernel/drivers/keyboard_ps2.c kernel/include/drivers.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/pci.o: kernel/drivers/pci.c kernel/include/drivers.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/shell.o: kernel/shell/shell.c kernel/include/drivers.h | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/kernel.elf: $(KERNEL_OBJS) kernel/linker.ld
	$(LD) -m elf_i386 -T kernel/linker.ld -nostdlib \
		-o $@ $(KERNEL_OBJS)

$(BUILD)/kernel.bin: $(BUILD)/kernel.elf
	$(OBJCOPY) -O binary $< $@
	test "$$(stat -c%s $@)" -le $(KERNEL_MAX)

$(BUILD)/kernel.padded: $(BUILD)/kernel.bin
	cp $< $@
	truncate -s $(KERNEL_MAX) $@

$(BUILD)/chiquios.img: $(BUILD)/boot.bin $(BUILD)/kernel.padded
	dd if=/dev/zero of=$@ bs=512 count=33 status=none
	dd if=$(BUILD)/boot.bin of=$@ conv=notrunc status=none
	dd if=$(BUILD)/kernel.padded of=$@ bs=512 seek=1 conv=notrunc status=none

check: $(BUILD)/chiquios.img
	@echo "ChiquiOS image:"
	@stat -c "%n: %s bytes" $(BUILD)/chiquios.img
	@echo "Build successful"

clean:
	rm -rf $(BUILD)


ISO := build/chiquios.iso
BOOTIMG := build/iso/boot.img

.PHONY: iso

iso: $(BUILD)/chiquios.img
	mkdir -p build/iso
	dd if=/dev/zero of=$(BOOTIMG) bs=512 count=2880
	dd if=$(BUILD)/chiquios.img of=$(BOOTIMG) conv=notrunc
	xorriso -as mkisofs -o $(ISO) -b boot.img -c boot.cat build/iso
