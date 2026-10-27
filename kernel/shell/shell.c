
#include "../include/drivers.h"
#include "../include/supervisor.h"

static int text_equal(const char *a, const char *b)
{
    while (*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }
    return *a == *b;
}

static void show_help(void)
{
    vga_write("Comandos de ChiquiOS 0.5:\n");
    vga_write("  help              Ayuda\n");
    vga_write("  clear             Limpiar pantalla\n");
    vga_write("  about             Informacion del sistema\n");
    vga_write("  version           Version del kernel\n");
    vga_write("  cpu               Informacion de CPU\n");
    vga_write("  pci               Explorar PCI\n");
    vga_write("  usb               Explorar controladores USB PCI\n");
    vga_write("  services          Listar servicios registrados\n");
    vga_write("  restart usb       Reinicializar USB\n");
    vga_write("  restart internet  Reinicializar red\n");
    vga_write("  reboot            Reiniciar equipo\n");
}

static void reboot_system(void)
{
    vga_write("Reiniciando...\n");
    __asm__ volatile ("cli");

    for (;;) {
        u8 status;

        __asm__ volatile ("inb $0x64, %0" : "=a"(status));

        if ((status & 0x02) == 0) {
            __asm__ volatile ("outb %0, $0x64" : : "a"((u8)0xFE));
        }
    }
}

void shell_run(void)
{
    char input[80];
    unsigned int length;

    vga_write("\nChiquiOS 0.5 - CLI\n");
    vga_write("Escribe help para comenzar.\n\n");

    for (;;) {
        length = 0;
        vga_write("chiquios> ");

        for (;;) {
            char c = keyboard_read_char();

            if (c == 0) continue;

            if (c == '\n' || c == '\r') {
                vga_put_char('\n');
                input[length] = '\0';
                break;
            }

            if (c == '\b') {
                if (length > 0) {
                    length--;
                    vga_put_char('\b');
                }
                continue;
            }

            if (c >= 32 && c <= 126 &&
                length < sizeof(input) - 1) {
                input[length++] = c;
                vga_put_char(c);
            }
        }

        if (text_equal(input, "help")) {
            show_help();
        } else if (text_equal(input, "clear")) {
            vga_clear();
        } else if (text_equal(input, "about")) {
            vga_write("ChiquiOS: kernel propio x86 de 32 bits.\n");
            vga_write("Arranque BIOS y shell de texto.\n");
        } else if (text_equal(input, "version")) {
            vga_write("ChiquiOS 0.5\n");
        } else if (text_equal(input, "cpu")) {
            cpu_detect();
        } else if (text_equal(input, "pci")) {
            pci_scan_all();
        } else if (text_equal(input, "usb")) {
            pci_scan_usb();
        } else if (text_equal(input, "services")) {
            supervisor_list();
        } else if (text_equal(input, "restart usb")) {
            supervisor_restart("usb");
        } else if (text_equal(input, "restart internet")) {
            supervisor_restart("internet");
        } else if (text_equal(input, "reboot")) {
            reboot_system();
        } else if (length != 0) {
            vga_write("Comando desconocido. Escribe help.\n");
        }
    }
}
