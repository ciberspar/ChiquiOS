
#ifndef CHIQUIOS_X64_GDT_H
#define CHIQUIOS_X64_GDT_H

#include "../../core/types.h"

void gdt_initialize(u64 kernel_stack);

#endif
