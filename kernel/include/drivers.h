
#ifndef CHIQUIOS_DRIVERS_H
#define CHIQUIOS_DRIVERS_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

void cpu_detect(void);

void vga_initialize(void);
void vga_clear(void);
void vga_write(const char *text);
void vga_put_char(char ch);

char keyboard_read_char(void);

void shell_run(void);
void kernel_main(void);

#endif
