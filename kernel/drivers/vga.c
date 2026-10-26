
#include "../include/drivers.h"

#define VGA_MEMORY ((volatile unsigned short *)0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_COLOR 0x0A

static unsigned int x = 0;
static unsigned int y = 0;

void vga_clear(void) {
    for (unsigned int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_MEMORY[i] =
            ((unsigned short)VGA_COLOR << 8) | ' ';
    }

    x = 0;
    y = 0;
}

void vga_initialize(void) {
    vga_clear();
}

void vga_write(const char *text) {
    while (*text) {
        char ch = *text++;

        if (ch == '\n') {
            x = 0;
            y++;
        } else {
            VGA_MEMORY[y * VGA_WIDTH + x] =
                ((unsigned short)VGA_COLOR << 8) |
                (unsigned char)ch;

            x++;
        }

        if (x >= VGA_WIDTH) {
            x = 0;
            y++;
        }

        if (y >= VGA_HEIGHT) {
            vga_clear();
        }
    }
}
