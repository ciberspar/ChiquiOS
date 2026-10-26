
#include "../include/drivers.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

static void outl(u16 port, u32 value) {
    __asm__ volatile ("outl %0, %1"
                      : : "a"(value), "Nd"(port));
}

static u32 inl(u16 port) {
    u32 value;
    __asm__ volatile ("inl %1, %0"
                      : "=a"(value) : "Nd"(port));
    return value;
}

static u32 pci_read(u8 bus, u8 device,
                    u8 function, u8 offset) {
    u32 address = 0x80000000u
        | ((u32)bus << 16)
        | ((u32)device << 11)
        | ((u32)function << 8)
        | (offset & 0xFC);

    outl(PCI_CONFIG_ADDRESS, address);
    return inl(PCI_CONFIG_DATA);
}

static void print_hex(u32 value) {
    static const char digits[] = "0123456789ABCDEF";

    for (int shift = 28; shift >= 0; shift -= 4)
        vga_put_char(digits[(value >> shift) & 0xF]);
}

void pci_scan_usb(void) {
    int found = 0;

    vga_write("Scanning PCI for USB controllers...\n");

    for (u32 bus = 0; bus < 256; bus++) {
        for (u32 dev = 0; dev < 32; dev++) {
            for (u32 fn = 0; fn < 8; fn++) {
                u32 id = pci_read(bus, dev, fn, 0x00);

                if ((id & 0xFFFF) == 0xFFFF) {
                    if (fn == 0)
                        break;
                    continue;
                }

                u32 class_info = pci_read(
                    bus, dev, fn, 0x08
                );

                u8 class_code = (class_info >> 24) & 0xFF;
                u8 subclass   = (class_info >> 16) & 0xFF;
                u8 prog_if    = (class_info >> 8) & 0xFF;

                if (class_code == 0x0C &&
                    subclass == 0x03) {
                    found++;

                    vga_write("USB controller at ");
                    print_hex(bus);
                    vga_put_char(':');
                    print_hex(dev);
                    vga_put_char(':');
                    print_hex(fn);

                    vga_write(" type=");

                    switch (prog_if) {
                    case 0x00:
                        vga_write("UHCI");
                        break;
                    case 0x10:
                        vga_write("OHCI");
                        break;
                    case 0x20:
                        vga_write("EHCI");
                        break;
                    case 0x30:
                        vga_write("xHCI");
                        break;
                    default:
                        vga_write("other");
                        break;
                    }

                    vga_write(" vendor=");
                    print_hex(id & 0xFFFF);
                    vga_write(" device=");
                    print_hex(id >> 16);
                    vga_put_char('\n');
                }

                /* No consultar funciones inexistentes
                   cuando la función 0 no es multifunción. */
                if (fn == 0) {
                    u32 header = pci_read(
                        bus, dev, 0, 0x0C
                    );

                    if (!((header >> 16) & 0x80))
                        break;
                }
            }
        }
    }

    if (found == 0)
        vga_write("No USB controllers detected.\n");
    else
        vga_write("USB controller scan complete.\n");
}
