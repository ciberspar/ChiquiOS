
#ifndef CHIQUIOS_DRIVERS_H
#define CHIQUIOS_DRIVERS_H

#include <stdint.h>

void cpu_detect(void);
void vga_initialize(void);
void vga_clear(void);
void vga_write(const char *text);

#endif
