
#include "../include/drivers.h"

static void print_banner(void)
{
    vga_write("\n");
    vga_write("  /\\                 ChiquiOS 0.4\n");
    vga_write(" /  \\                Minimal. Ligero. Tu sistema.\n");
    vga_write("/ /\\ \\               --------------------------\n");
    vga_write("\\/  \\/\n");
    vga_write("\n");
    vga_write("Kernel       : ChiquiOS v0.4\n");
    vga_write("Arquitectura : x86 (32-bit)\n");
    vga_write("Dispositivo  : BIOS\n");
    vga_write("\n");
    vga_write("Bienvenido a ChiquiOS\n");
    vga_write("Escribe 'help' para ver los comandos disponibles.\n\n");
}

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
    vga_write("Comandos disponibles:\n");
    vga_write("  help     - Muestra esta ayuda\n");
    vga_write("  clear    - Limpia la pantalla\n");
    vga_write("  about    - Informacion de ChiquiOS\n");
    vga_write("  cpu      - Detecta el procesador\n");
    vga_write("  pci      - Explora dispositivos PCI\n");
    vga_write("  usb      - Busca controladores USB PCI\n");
    vga_write("  version  - Muestra la version\n");
    vga_write("  reboot   - Reinicia el sistema\n");
}

static void reboot_system(void)
{
    vga_write("Reiniciando ChiquiOS...\n");

    __asm__ volatile ("cli");

    for (;;) {
        u8 good = 0x02;

        while (good & 0x02) {
            __asm__ volatile ("inb $0x64, %0" : "=a"(good));
        }

        __asm__ volatile ("outb %0, $0x64" : : "a"((u8)0xFE));
    }
}

void shell_run(void)
{
    char input[80];
    unsigned int length;

    print_banner();

    for (;;) {
        length = 0;
        vga_write("chiquios@chiquios:~$ ");

        for (;;) {
            char c = keyboard_read_char();

            if (c == 0) {
                continue;
            }

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

            if (c >= 32 && c <= 126 && length < sizeof(input) - 1) {
                input[length++] = c;
                vga_put_char(c);
            }
        }

        if (text_equal(input, "help")) {
            show_help();
        } else if (text_equal(input, "clear")) {
            vga_clear();
        } else if (text_equal(input, "about")) {
            vga_write("ChiquiOS 0.4: sistema propio x86 de 32 bits.\n");
            vga_write("Kernel independiente, shell CLI y arranque BIOS.\n");
        } else if (text_equal(input, "version")) {
            vga_write("ChiquiOS 0.4 (x86, 32-bit)\n");
        } else if (text_equal(input, "cpu")) {
            cpu_detect();
        } else if (text_equal(input, "pci")) {
            pci_scan_all();
        } else if (text_equal(input, "usb")) {
            pci_scan_usb();
        } else if (text_equal(input, "reboot")) {
            reboot_system();
        } else if (length == 0) {
            /* No hacer nada si se pulsa Enter sin escribir. */
        } else {
            vga_write("Comando desconocido. Escribe 'help'.\n");
        }
    }
}
