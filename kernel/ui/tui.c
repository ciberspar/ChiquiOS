
#include "../include/tui.h"
#include "../include/drivers.h"
#include "../include/supervisor.h"

void tui_run(void)
{
    char key;

    for (;;) {
        vga_clear();
        vga_write("================================\n");
        vga_write("         ChiquiOS 0.5\n");
        vga_write("       Centro del sistema\n");
        vga_write("================================\n\n");

        vga_write("1. Informacion de CPU\n");
        vga_write("2. Explorar dispositivos PCI\n");
        vga_write("3. Ver controladores USB\n");
        vga_write("4. Ver servicios\n");
        vga_write("5. Abrir terminal\n");
        vga_write("6. Instalador (proximamente)\n");
        vga_write("\nSelecciona una opcion: ");

        key = keyboard_read_char();
        vga_put_char(key);
        vga_write("\n\n");

        switch (key) {
        case '1':
            cpu_detect();
            break;

        case '2':
            pci_scan_all();
            break;

        case '3':
            pci_scan_usb();
            break;

        case '4':
            supervisor_list();
            break;

        case '5':
            shell_run();
            break;

        case '6':
            vga_write("El instalador aun no puede escribir en disco.\n");
            vga_write("Primero necesitamos implementar almacenamiento.\n");
            break;

        default:
            vga_write("Opcion desconocida.\n");
            break;
        }

        vga_write("\nPulsa una tecla para volver al menu...");
        (void)keyboard_read_char();
    }
}
