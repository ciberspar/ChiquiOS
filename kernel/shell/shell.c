
#include "../include/drivers.h"

static char line[80];

static int same(const char *a, const char *b) {
    while (*a && *b && *a == *b) {
        a++;
        b++;
    }

    return *a == '\0' && *b == '\0';
}

static void execute(const char *cmd) {
    if (same(cmd, "help")) {
        vga_write("ChiquiOS 0.3 commands:\n");
        vga_write(" help  - show commands\n");
        vga_write(" clear - clear screen\n");
        vga_write(" about - system information\n");
        vga_write(" cpu   - CPU vendor\n");
        vga_write(" mem   - memory status\n");
        vga_write(" usb   - scan PCI USB controllers\n");
        vga_write(" reboot - reboot (not implemented)\n");

    } else if (same(cmd, "clear")) {
        vga_clear();

    } else if (same(cmd, "about")) {
        vga_write("ChiquiOS 0.3\n");
        vga_write("Custom 32-bit x86 kernel\n");
        vga_write("BIOS boot prototype\n");

    } else if (same(cmd, "cpu")) {
        cpu_detect();

    } else if (same(cmd, "mem")) {
        vga_write("Memory manager: not implemented yet.\n");

    } else if (same(cmd, "usb")) {
        pci_scan_usb();

    } else if (same(cmd, "reboot")) {
        vga_write("Reboot is not implemented yet.\n");

    } else if (*cmd) {
        vga_write("Unknown command. Type help.\n");
    }
}

void shell_run(void) {
    for (;;) {
        u32 n = 0;

        vga_write("chiquios> ");

        for (;;) {
            char ch = keyboard_read_char();

            if (ch == '\n') {
                vga_put_char('\n');
                line[n] = '\0';
                break;
            }

            if (ch == '\b') {
                if (n > 0) {
                    n--;
                    vga_put_char('\b');
                }
                continue;
            }

            if (n < sizeof(line) - 1) {
                line[n++] = ch;
                vga_put_char(ch);
            }
        }

        execute(line);
    }
}
