
#include "../include/drivers.h"

static char line[80];

static int same(const char *a, const char *b) {
    while (*a && *b && *a == *b) {
        a++;
        b++;
    }

    return *a == '\0' && *b == '\0';
}

static void show_help(void) {
    vga_write("ChiquiOS 0.4 - Commands\n");
    vga_write("----------------------\n");
    vga_write("help  - Show commands\n");
    vga_write("clear - Clear screen\n");
    vga_write("about - System information\n");
    vga_write("cpu   - Detect CPU vendor\n");
    vga_write("mem   - Memory information\n");
    vga_write("pci   - Scan PCI devices\n");
    vga_write("usb   - Scan USB controllers\n");
    vga_write("disks - Disk controller status\n");
    vga_write("reboot - Reboot system (planned)\n");
}

static void execute(const char *cmd) {
    if (same(cmd, "help")) {
        show_help();

    } else if (same(cmd, "clear")) {
        vga_clear();

    } else if (same(cmd, "about")) {
        vga_write("ChiquiOS 0.4\n");
        vga_write("Custom 32-bit x86 operating system\n");
        vga_write("Architecture: i386\n");
        vga_write("Mode: CLI\n");
        vga_write("Boot: Custom BIOS bootloader\n");

    } else if (same(cmd, "cpu")) {
        cpu_detect();

    } else if (same(cmd, "mem")) {
        vga_write("Memory information\n");
        vga_write("RAM detection: not implemented yet.\n");

    } else if (same(cmd, "pci")) {
        pci_scan_all();

    } else if (same(cmd, "usb")) {
        pci_scan_usb();

    } else if (same(cmd, "disks")) {
        vga_write("Disk controller information\n");
        vga_write("IDE/ATA driver: not implemented.\n");
        vga_write("SATA/AHCI driver: not implemented.\n");
        vga_write("Disk reading: not implemented.\n");

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

            if (ch >= 32 && ch <= 126) {
                if (n < sizeof(line) - 1) {
                    line[n++] = ch;
                    vga_put_char(ch);
                }
            }
        }

        execute(line);
    }
}
