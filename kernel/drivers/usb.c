
#include "../include/usb.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

static u32 pci_read(u8 bus, u8 slot, u8 function, u8 offset)
{
    u32 address =
        0x80000000u |
        ((u32)bus << 16) |
        ((u32)slot << 11) |
        ((u32)function << 8) |
        (offset & 0xFC);

    u32 value;

    __asm__ volatile ("outl %0, %1"
        : : "a"(address), "Nd"((u16)PCI_CONFIG_ADDRESS));

    __asm__ volatile ("inl %1, %0"
        : "=a"(value) : "Nd"((u16)PCI_CONFIG_DATA));

    return value;
}

static u8 pci_read8(u8 bus, u8 slot, u8 function, u8 offset)
{
    u32 value = pci_read(bus, slot, function, offset);
    return (u8)(value >> ((offset & 3) * 8));
}

static usb_controller_t controllers[32];
static u8 controller_count;

void usb_initialize(void)
{
    controller_count = 0;
    usb_scan_pci();
}

void usb_scan_pci(void)
{
    controller_count = 0;

    for (u16 bus = 0; bus < 256; bus++) {
        for (u8 slot = 0; slot < 32; slot++) {
            for (u8 function = 0; function < 8; function++) {
                u32 id = pci_read((u8)bus, slot, function, 0);

                if ((id & 0xFFFF) == 0xFFFF)
                    continue;

                u8 class_code = pci_read8(
                    (u8)bus, slot, function, 0x0B);

                u8 subclass = pci_read8(
                    (u8)bus, slot, function, 0x0A);

                u8 prog_if = pci_read8(
                    (u8)bus, slot, function, 0x09);

                /* PCI class 0x0C, subclass 0x03 = USB controller */
                if (class_code == 0x0C && subclass == 0x03) {
                    if (controller_count < 32) {
                        usb_controller_t *c =
                            &controllers[controller_count++];

                        c->bus = (u8)bus;
                        c->slot = slot;
                        c->function = function;
                        c->class_code = class_code;
                        c->subclass = subclass;
                        c->prog_if = prog_if;
                        c->connected = 0;
                    }
                }

                /* Avoid scanning absent PCI functions 1-7. */
                if (function == 0 &&
                    !(pci_read8((u8)bus, slot, 0, 0x0E) & 0x80))
                    break;
            }
        }
    }
}

void usb_list_devices(void)
{
    vga_write("USB host controllers: ");

    if (controller_count == 0) {
        vga_write("none detected\n");
        return;
    }

    vga_write("detected\n");

    for (u8 i = 0; i < controller_count; i++) {
        vga_write("Controller PCI ");
        vga_put_char('0' + (controllers[i].bus / 100));
        vga_put_char('0' + ((controllers[i].bus / 10) % 10));
        vga_put_char('0' + (controllers[i].bus % 10));
        vga_put_char(':');
        vga_put_char('0' + controllers[i].slot / 10);
        vga_put_char('0' + controllers[i].slot % 10);
        vga_write(" IF=");
        vga_put_char("0123456789ABCDEF"[
            controllers[i].prog_if & 0x0F]);
        vga_write("\n");
    }
}

void usb_print_summary(void)
{
    usb_list_devices();
}
