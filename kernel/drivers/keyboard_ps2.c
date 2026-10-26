
#include "../include/drivers.h"

static u8 inb(u16 port) {
    u8 value;
    __asm__ volatile ("inb %1, %0"
                      : "=a"(value) : "Nd"(port));
    return value;
}

char keyboard_read_char(void) {
    static const char keys[128] = {
        [0x02]='1', [0x03]='2', [0x04]='3',
        [0x05]='4', [0x06]='5', [0x07]='6',
        [0x08]='7', [0x09]='8', [0x0A]='9',
        [0x0B]='0', [0x0C]='-', [0x0E]='\b',
        [0x10]='q', [0x11]='w', [0x12]='e',
        [0x13]='r', [0x14]='t', [0x15]='y',
        [0x16]='u', [0x17]='i', [0x18]='o',
        [0x19]='p', [0x1C]='\n',
        [0x1E]='a', [0x1F]='s', [0x20]='d',
        [0x21]='f', [0x22]='g', [0x23]='h',
        [0x24]='j', [0x25]='k', [0x26]='l',
        [0x2C]='z', [0x2D]='x', [0x2E]='c',
        [0x2F]='v', [0x30]='b', [0x31]='n',
        [0x32]='m', [0x39]=' '
    };

    for (;;) {
        if (inb(0x64) & 1) {
            u8 sc = inb(0x60);

            if (sc & 0x80)
                continue;

            if (sc < 128 && keys[sc])
                return keys[sc];
        }
    }
}
