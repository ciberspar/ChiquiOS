
#include "../include/drivers.h"

/*
 * ChiquiOS USB keyboard interface.
 * The host-controller driver must provide USB HID input.
 */

static volatile char usb_key_buffer[64];
static volatile u8 usb_key_read_pos = 0;
static volatile u8 usb_key_write_pos = 0;
static volatile u8 usb_keyboard_ready = 0;

void keyboard_usb_initialize(void)
{
    usb_key_read_pos = 0;
    usb_key_write_pos = 0;
    usb_keyboard_ready = 0;
}

void keyboard_usb_set_ready(u8 ready)
{
    usb_keyboard_ready = ready;
}

void keyboard_usb_push_char(char c)
{
    u8 next = (u8)((usb_key_write_pos + 1) % 64);

    if (next == usb_key_read_pos)
        return;

    usb_key_buffer[usb_key_write_pos] = c;
    usb_key_write_pos = next;
}

char keyboard_usb_read_char(void)
{
    if (!usb_keyboard_ready)
        return 0;

    if (usb_key_read_pos == usb_key_write_pos)
        return 0;

    char c = usb_key_buffer[usb_key_read_pos];
    usb_key_read_pos = (u8)((usb_key_read_pos + 1) % 64);

    return c;
}
