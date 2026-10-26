
#include "../include/drivers.h"

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((volatile u16*)0xB8000)

#define COLOR_BLUE   0x09
#define COLOR_CYAN   0x0B
#define COLOR_WHITE  0x0F
#define COLOR_GRAY   0x07

static u8 current_color = COLOR_CYAN;
static u16 cursor_x = 0;
static u16 cursor_y = 0;

static u16 vga_entry(char c, u8 color)
{
    return (u16)c | ((u16)color << 8);
}

static void update_cursor(void)
{
    u16 pos = cursor_y * VGA_WIDTH + cursor_x;

    __asm__ volatile ("outb %0, %1"
        : : "a"((u8)0x0F), "Nd"((u16)0x3D4));

    __asm__ volatile ("outb %0, %1"
        : : "a"((u8)(pos & 0xFF)), "Nd"((u16)0x3D5));

    __asm__ volatile ("outb %0, %1"
        : : "a"((u8)0x0E), "Nd"((u16)0x3D4));

    __asm__ volatile ("outb %0, %1"
        : : "a"((u8)(pos >> 8)), "Nd"((u16)0x3D5));
}

static void scroll_screen(void)
{
    for (u16 y = 1; y < VGA_HEIGHT; y++) {
        for (u16 x = 0; x < VGA_WIDTH; x++) {
            VGA_MEMORY[(y - 1) * VGA_WIDTH + x] =
                VGA_MEMORY[y * VGA_WIDTH + x];
        }
    }

    for (u16 x = 0; x < VGA_WIDTH; x++) {
        VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + x] =
            vga_entry(' ', current_color);
    }

    cursor_y = VGA_HEIGHT - 1;
}

void vga_initialize(void)
{
    current_color = COLOR_CYAN;
    cursor_x = 0;
    cursor_y = 0;
    vga_clear();
}

void vga_clear(void)
{
    for (u16 i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_MEMORY[i] = vga_entry(' ', current_color);
    }

    cursor_x = 0;
    cursor_y = 0;
    update_cursor();
}

void vga_put_char(char c)
{
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else if (c == '\b') {
        if (cursor_x > 0) {
            cursor_x--;
            VGA_MEMORY[cursor_y * VGA_WIDTH + cursor_x] =
                vga_entry(' ', current_color);
        }
    } else if (c == '\t') {
        cursor_x = (cursor_x + 4) & ~3;
    } else {
        VGA_MEMORY[cursor_y * VGA_WIDTH + cursor_x] =
            vga_entry(c, current_color);
        cursor_x++;
    }

    if (cursor_x >= VGA_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }

    if (cursor_y >= VGA_HEIGHT) {
        scroll_screen();
    }

    update_cursor();
}

void vga_write(const char *text)
{
    while (*text) {
        vga_put_char(*text++);
    }
}
