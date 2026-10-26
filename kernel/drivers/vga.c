
#include "../include/drivers.h"

#define VGA ((volatile u16 *)0xB8000)
#define WIDTH 80
#define HEIGHT 25
#define COLOR 0x0A

static u32 x;
static u32 y;

void vga_clear(void) {
    for (u32 i = 0; i < WIDTH * HEIGHT; i++)
        VGA[i] = ((u16)COLOR << 8) | ' ';

    x = 0;
    y = 0;
}

static void scroll(void) {
    for (u32 i = 0; i < (HEIGHT - 1) * WIDTH; i++)
        VGA[i] = VGA[i + WIDTH];

    for (u32 i = (HEIGHT - 1) * WIDTH;
         i < HEIGHT * WIDTH; i++)
        VGA[i] = ((u16)COLOR << 8) | ' ';

    y = HEIGHT - 1;
}

void vga_put_char(char ch) {
    if (ch == '\n') {
        x = 0;
        y++;
    } else if (ch == '\r') {
        x = 0;
    } else if (ch == '\b') {
        if (x > 0) {
            x--;
            VGA[y * WIDTH + x] = ((u16)COLOR << 8) | ' ';
        }
    } else {
        VGA[y * WIDTH + x] =
            ((u16)COLOR << 8) | (u8)ch;
        x++;

        if (x >= WIDTH) {
            x = 0;
            y++;
        }
    }

    if (y >= HEIGHT)
        scroll();
}

void vga_write(const char *text) {
    while (*text)
        vga_put_char(*text++);
}

void vga_initialize(void) {
    vga_clear();
}
