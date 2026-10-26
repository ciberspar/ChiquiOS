
#include "../include/drivers.h"

#define PCI_ADDRESS 0xCF8
#define PCI_DATA    0xCFC

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

static u32 pci_read(u8 bus, u8 dev, u8 fn, u8 offset) {
    u32 address = 0x80000000u
        | ((u32)bus << 16)
        | ((u32)dev << 11)
        | ((u32)fn << 8)
        | (offset & 0xFC);

    outl(PCI_ADDRESS, address);
    return inl(PCI_DATA);
}

static void hex32(u32 value) {
    static const char digits[] = "0123456789ABCDEF";

    for (int shift = 28; shift >= 0; shift -= 4) {
        vga_put_char(digits[(value >> shift) & 0xF]);
    }
}

static int device_present(u32 id) {
    return (id & 0xFFFF) != 0xFFFF;
}

static int multifunction(u8 bus, u8 dev) {
    u32 header = pci_read(bus, dev, 0, 0x0C);
    return ((header >> 23) & 1) != 0;
}

void pci_scan_all(void) {
    int found = 0;

    vga_write("ChiquiOS 0.4 - PCI inventory\n");

    for (u32 bus = 0; bus < 256; bus++) {
        for (u32 dev = 0; dev < 32; dev++) {
            for (u32 fn = 0; fn < 8; fn++) {
                u32 id = pci_read(bus, dev, fn, 0x00);

                if (!device_present(id)) {
                    if (fn == 0)
                        break;
                    continue;
                }

                u32 info = pci_read(bus, dev, fn, 0x08);

                u8 class_code = (info >> 24) & 0xFF;
                u8 subclass   = (info >> 16) & 0xFF;
                u8 prog_if    = (info >> 8) & 0xFF;

                vga_write("PCI ");
                hex32(bus);
                vga_put_char(':');
                hex32(dev);
                vga_put_char(':');
                hex32(fn);

                vga_write(" VID=");
                hex32(id & 0xFFFF);
                vga_write(" DID=");
                hex32(id >> 16);

                vga_write(" CLASS=");
                hex32(class_code);
                vga_put_char(':');
                hex32(subclass);

                vga_write(" IF=");
                hex32(prog_if);
                vga_put_char('\n');

                found++;

                if (fn == 0 && !multifunction(bus, dev))
                    break;
            }
        }
    }

    vga_write("PCI devices found: ");
    hex32((u32)found);
    vga_put_char('\n');
}

void pci_scan_usb(void) {
    int found = 0;

    vga_write("Scanning PCI USB controllers...\n");

    for (u32 bus = 0; bus < 256; bus++) {
        for (u32 dev = 0; dev < 32; dev++) {
            for (u32 fn = 0; fn < 8; fn++) {
                u32 id = pci_read(bus, dev, fn, 0x00);

                if (!device_present(id)) {
                    if (fn == 0)
                        break;
                    continue;
                }

                u32 info = pci_read(bus, dev, fn, 0x08);
                u8 class_code = (info >> 24) & 0xFF;
                u8 subclass = (info >> 16) & 0xFF;
                u8 prog_if = (info >> 8) & 0xFF;

                if (class_code == 0x0C && subclass == 0x03) {
                    vga_write("USB controller: ");
                    hex32(bus);
                    vga_put_char(':');
                    hex32(dev);
                    vga_put_char(':');
                    hex32(fn);
                    vga_write(" type=");

                    switch (prog_if) {
                    case 0x00: vga_write("UHCI"); break;
                    case 0x10: vga_write("OHCI"); break;
                    case 0x20: vga_write("EHCI"); break;
                    case 0x30: vga_write("xHCI"); break;
                    default:   vga_write("other"); break;
                    }

                    vga_put_char('\n');
                    found++;
                }

                if (fn == 0 && !multifunction(bus, dev))
                    break;
            }
        }
    }

    if (!found)
        vga_write("No USB controllers detected.\n");
    else
        vga_write("USB controller scan complete.\n");
}
