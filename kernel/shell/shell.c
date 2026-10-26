
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
        vga_write("Commands: help clear about cpu\n");
        vga_write("          mem reboot\n");
    } else if (same(cmd, "clear")) {
        vga_clear();
    } else if (same(cmd, "about")) {
        vga_write("ChiquiOS 0.2 - own x86 kernel\n");
    } else if (same(cmd, "cpu")) {
        cpu_detect();
    } else if (same(cmd, "mem")) {
        vga_write("Memory manager: not implemented yet.\n");
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
