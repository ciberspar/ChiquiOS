
NASM = nasm
CC = gcc
LD = ld
OBJCOPY = objcopy

BUILD = build
KERNEL_MAX = 16384

CFLAGS = -m32 -march=i386 -ffreestanding -fno-pie \
         -fno-pic -fno-stack-protector -fno-builtin \
         -fno-asynchronous-unwind-tables -O2 -Wall -Wextra

.PHONY: all clean check

all: $(BUILD)/chiquios.img

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.bin: boot/boot.asm | $(BUILD)
	$(NASM) -f bin $< -o $@
	test "$$(wc -c < $@)" -eq 512

$(BUILD)/entry.o: kernel/entry.asm | $(BUILD)
	$(NASM) -f elf32 $< -o $@

$(BUILD)/kernel.o: kernel/kernel.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/kernel.elf: $(BUILD)/entry.o $(BUILD)/kernel.o kernel/linker.ld
	$(LD) -m elf_i386 -T kernel/linker.ld \
	    -nostdlib -o $@ $(BUILD)/entry.o $(BUILD)/kernel.o

$(BUILD)/kernel.bin: $(BUILD)/kernel.elf
	$(OBJCOPY) -O binary $< $@
	test "$$(wc -c < $@)" -le $(KERNEL_MAX)

$(BUILD)/kernel.padded: $(BUILD)/kernel.bin
	cp $< $@
	truncate -s $(KERNEL_MAX) $@

$(BUILD)/chiquios.img: $(BUILD)/boot.bin $(BUILD)/kernel.padded
	dd if=/dev/zero of=$@ bs=512 count=33
	dd if=$(BUILD)/boot.bin of=$@ conv=notrunc
	dd if=$(BUILD)/kernel.padded of=$@ bs=512 seek=1 conv=notrunc
	@echo "Imagen de ChiquiOS creada: $@"

check: $(BUILD)/chiquios.img
	@echo "Bootloader:"
	@wc -c $(BUILD)/boot.bin
	@echo "Kernel:"
	@wc -c $(BUILD)/kernel.bin
	@echo "Imagen:"
	@wc -c $(BUILD)/chiquios.img

clean:
	rm -rf $(BUILD)
