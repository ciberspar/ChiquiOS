
#include "../include/drivers.h"

#define PCI_ADDR 0xCF8
#define PCI_DATA 0xCFC

static void outl(u16 port, u32 value)
{
    __asm__ volatile ("outl %0, %1" : : "a"(value), "Nd"(port));
}

static u32 inl(u16 port)
{
    u32 value;
    __asm__ volatile ("inl %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

static u32 pci_read(u8 bus, u8 dev, u8 fn, u8 offset)
{
    u32 address = 0x80000000u
        | ((u32)bus << 16)
        | ((u32)dev << 11)
        | ((u32)fn << 8)
        | (offset & 0xFC);

    outl(PCI_ADDR, address);
    return inl(PCI_DATA);
}

static void print_hex(u32 value)
{
    static const char digits[] = "0123456789ABCDEF";

    for (int shift = 28; shift >= 0; shift -= 4)
        vga_put_char(digits[(value >> shift) & 15]);
}

void e1000_probe(void)
{
    u32 found = 0;

    vga_write("Buscando Intel E1000 por PCI...\n");

    for (u32 bus = 0; bus < 256; bus++) {
        for (u32 dev = 0; dev < 32; dev++) {
            for (u32 fn = 0; fn < 8; fn++) {
                u32 id = pci_read((u8)bus, (u8)dev, (u8)fn, 0);

                if ((id & 0xFFFF) == 0xFFFF) {
                    if (fn == 0)
                        break;
                    continue;
                }

                u16 vendor = (u16)(id & 0xFFFF);
                u16 device = (u16)(id >> 16);

                if (vendor == 0x8086 &&
                    (device == 0x100E ||
                     device == 0x100F ||
                     device == 0x1004 ||
                     device == 0x10D3 ||
                     device == 0x153A)) {

                    u32 bar0 = pci_read(
                        (u8)bus, (u8)dev, (u8)fn, 0x10);

                    vga_write("Intel E1000 compatible: PCI ");
                    print_hex(bus);
                    vga_put_char(':');
                    print_hex(dev);
                    vga_put_char(':');
                    print_hex(fn);

                    vga_write(" VID=");
                    print_hex(vendor);
                    vga_write(" DID=");
                    print_hex(device);
                    vga_write(" BAR0=");
                    print_hex(bar0);
                    vga_put_char('\n');

                    found++;
                }

                if (fn == 0 &&
                    !(pci_read((u8)bus, (u8)dev, 0, 0x0C)
                      & 0x00800000u))
                    break;
            }
        }
    }

    if (!found)
        vga_write("No se encontro un E1000 compatible.\n");
    else
        vga_write("Deteccion E1000 terminada; red aun no activa.\n");
}
