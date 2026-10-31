
#include "serial.h"

#define COM1 0x3F8

static int serial_ready = 0;

static inline void outb(u16 port, u8 value)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

static inline u8 inb(u16 port)
{
    u8 value;

    __asm__ volatile (
        "inb %1, %0"
        : "=a"(value)
        : "Nd"(port)
    );

    return value;
}

void serial_initialize(void)
{
    serial_ready = 0;

    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x80);

    outb(COM1 + 0, 0x01);
    outb(COM1 + 1, 0x00);

    outb(COM1 + 3, 0x03);
    outb(COM1 + 2, 0xC7);
    outb(COM1 + 4, 0x0B);

    serial_ready = 1;

    serial_write_string("[ChiquiOS] COM1 inicializado\n");
}

void serial_write_char(char c)
{
    if (!serial_ready)
        return;

    u32 timeout = 1000000;

    while (!(inb(COM1 + 5) & 0x20))
    {
        if (--timeout == 0)
            return;
    }

    outb(COM1, (u8)c);
}

void serial_write_string(const char *text)
{
    if (!text)
        return;

    while (*text)
    {
        if (*text == '\n')
            serial_write_char('\r');

        serial_write_char(*text++);
    }
}

void serial_write_hex(u64 value)
{
    static const char digits[] = "0123456789ABCDEF";

    serial_write_string("0x");

    for (int shift = 60; shift >= 0; shift -= 4)
    {
        serial_write_char(digits[(value >> shift) & 0xF]);
    }
}

void serial_write_dec(u64 value)
{
    char buffer[21];
    int i = 0;

    if (value == 0)
    {
        serial_write_char('0');
        return;
    }

    while (value > 0 && i < 20)
    {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }

    while (i > 0)
        serial_write_char(buffer[--i]);
}

int serial_is_initialized(void)
{
    return serial_ready;
}
