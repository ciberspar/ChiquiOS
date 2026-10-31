
#ifndef CHIQUIOS_SERIAL_H
#define CHIQUIOS_SERIAL_H

#include "../core/types.h"

void serial_initialize(void);
void serial_write_char(char c);
void serial_write_string(const char *text);
void serial_write_hex(u64 value);
void serial_write_dec(u64 value);
int serial_is_initialized(void);

#endif
