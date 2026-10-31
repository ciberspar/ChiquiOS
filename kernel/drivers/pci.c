
#include "../include/drivers.h"
#include "../core/driver_manager.h"

#define PCI_ADDRESS 0xCF8
#define PCI_DATA    0xCFC

static void outl(u16 port, u32 value)
{
    __asm__ volatile ("outl %0, %1"
        : : "a"(value), "Nd"(port));
}

static u32 inl(u16 port)
{
    u32 value;

    __asm__ volatile ("inl %1, %0"
        : "=a"(value) : "Nd"(port));

    return value;
}

static u32 pci_read(u8 bus, u8 dev, u8 fn, u8 offset)
{
    u32 address = 0x80000000u
        | ((u32)bus << 16)
        | ((u32)dev << 11)
        | ((u32)fn << 8)
        | (offset & 0xFC);

    outl(PCI_ADDRESS, address);

    return inl(PCI_DATA);
}

static void hex32(u32 value)
{
    static const char digits[] = "0123456789ABCDEF";

    for (int shift = 28; shift >= 0; shift -= 4)
        vga_put_char(digits[(value >> shift) & 0xF]);
}

static int device_present(u32 id)
{
    return (id & 0xFFFF) != 0xFFFF;
}

static int multifunction(u8 bus, u8 dev)
{
    u32 header = pci_read(bus, dev, 0, 0x0C);

    return ((header >> 23) & 1) != 0;
}

void pci_scan_all(void)
{
    u32 found = 0;
    u32 matched = 0;

    vga_write("\n=== PCI HARDWARE SCAN ===\n");

    for (u32 bus = 0; bus < 256; bus++) {
        for (u32 dev = 0; dev < 32; dev++) {
            for (u32 fn = 0; fn < 8; fn++) {
                u32 id = pci_read(
                    (u8)bus, (u8)dev, (u8)fn, 0x00
                );

                if (!device_present(id)) {
                    if (fn == 0)
                        break;

                    continue;
                }

                u16 vendor = (u16)(id & 0xFFFF);
                u16 device = (u16)(id >> 16);

                const driver_info_t *driver =
                    driver_find(vendor, device);

                found++;

                vga_write("PCI ");
                hex32(bus);
                vga_put_char(':');
                hex32(dev);
                vga_put_char(':');
                hex32(fn);

                vga_write(" VID=");
                hex32(vendor);

                vga_write(" DID=");
                hex32(device);

                if (driver != NULL) {
                    vga_write(" DRIVER=");
                    vga_write(driver->name);
                    matched++;
                } else {
                    vga_write(" DRIVER=No registrado");
                }

                vga_put_char('\n');

                if (fn == 0 && !multifunction((u8)bus, (u8)dev))
                    break;
            }
        }
    }

    vga_write("Dispositivos PCI: ");
    hex32(found);

    vga_write("\nCoincidencias de drivers: ");
    hex32(matched);

    vga_put_char('\n');
}

void pci_scan_usb(void)
{
    u32 found = 0;

    vga_write("\nBuscando controladores USB...\n");

    for (u32 bus = 0; bus < 256; bus++) {
        for (u32 dev = 0; dev < 32; dev++) {
            for (u32 fn = 0; fn < 8; fn++) {
                u32 id = pci_read(
                    (u8)bus, (u8)dev, (u8)fn, 0x00
                );

                if (!device_present(id)) {
                    if (fn == 0)
                        break;

                    continue;
                }

                u32 info = pci_read(
                    (u8)bus, (u8)dev, (u8)fn, 0x08
                );

                u8 class_code = (u8)(info >> 24);
                u8 subclass = (u8)(info >> 16);
                u8 prog_if = (u8)(info >> 8);

                if (class_code == 0x0C && subclass == 0x03) {
                    vga_write("USB: ");

                    switch (prog_if) {
                    case 0x00: vga_write("UHCI"); break;
                    case 0x10: vga_write("OHCI"); break;
                    case 0x20: vga_write("EHCI"); break;
                    case 0x30: vga_write("xHCI"); break;
                    default: vga_write("Desconocido"); break;
                    }

                    vga_put_char('\n');
                    found++;
                }

                if (fn == 0 && !multifunction((u8)bus, (u8)dev))
                    break;
            }
        }
    }

    if (!found)
        vga_write("No se encontraron controladores USB.\n");
}
