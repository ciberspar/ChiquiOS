
#include "../include/drivers.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

#define PCI_CLASS_NETWORK  0x02
#define PCI_SUBCLASS_ETHERNET 0x00
#define PCI_SUBCLASS_WIFI     0x80

static u32 pci_read(u8 bus, u8 slot, u8 function, u8 offset)
{
    u32 address = 0x80000000u
        | ((u32)bus << 16)
        | ((u32)slot << 11)
        | ((u32)function << 8)
        | (offset & 0xFC);

    u32 value;

    __asm__ volatile (
        "outl %0, %1"
        : : "a"(address), "Nd"((u16)PCI_CONFIG_ADDRESS)
    );

    __asm__ volatile (
        "inl %1, %0"
        : "=a"(value) : "Nd"((u16)PCI_CONFIG_DATA)
    );

    return value;
}

static u8 pci_read8(u8 bus, u8 slot, u8 function, u8 offset)
{
    u32 value = pci_read(bus, slot, function, offset);
    return (u8)(value >> ((offset & 3) * 8));
}

void internet_initialize(void)
{
    vga_write("Internet: buscando adaptadores PCI...\n");
    internet_list_devices();
}

void internet_list_devices(void)
{
    u32 found = 0;

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

                if (class_code == PCI_CLASS_NETWORK) {
                    if (subclass == PCI_SUBCLASS_ETHERNET) {
                        vga_write("Ethernet: adaptador detectado en PCI\n");
                        found++;
                    } else if (subclass == PCI_SUBCLASS_WIFI) {
                        vga_write("Wi-Fi: dispositivo de red detectado en PCI\n");
                        found++;
                    } else {
                        vga_write("Red: otro dispositivo PCI detectado\n");
                        found++;
                    }
                }

                if (function == 0 &&
                    !(pci_read8((u8)bus, slot, 0, 0x0E) & 0x80))
                    break;
            }
        }
    }

    if (found == 0)
        vga_write("No se encontraron adaptadores de red PCI.\n");

    vga_write("Nota: deteccion PCI; conexion aun no implementada.\n");
}
