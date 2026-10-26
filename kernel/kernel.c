
/* ChiquiOS Kernel 0.1.0
 * Arquitectura: x86 de 32 bits
 * Sin libc ni sistema operativo anfitrion.
 */

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

#define VGA_MEMORY ((volatile u16 *)0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_COLOR 0x0A

static u32 cursor_x = 0;
static u32 cursor_y = 0;

static void clear_screen(void)
{
    for (u32 i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        VGA_MEMORY[i] = ((u16)VGA_COLOR << 8) | ' ';

    cursor_x = 0;
    cursor_y = 0;
}

static void put_char(char c)
{
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else {
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }

        if (cursor_y < VGA_HEIGHT) {
            u32 pos = cursor_y * VGA_WIDTH + cursor_x;
            VGA_MEMORY[pos] = ((u16)VGA_COLOR << 8) | (u8)c;
            cursor_x++;
        }
    }

    if (cursor_y >= VGA_HEIGHT) {
        cursor_y = 0;
    }
}

static void print(const char *s)
{
    while (*s)
        put_char(*s++);
}

void kernel_main(void)
{
    clear_screen();

    print("ChiquiOS CLI 0.1.0\n");
    print("Kernel propio de 32 bits iniciado.\n");
    print("------------------------------------\n");
    print("Bienvenido a ChiquiOS.\n");
    print("El siguiente paso sera crear ChiquiShell.\n");
    print("\n");
    print("Estado: kernel iniciado correctamente.\n");

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
