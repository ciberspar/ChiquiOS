
#ifndef CHIQUIOS_X64_TSS_H
#define CHIQUIOS_X64_TSS_H

#include "../../core/types.h"

typedef struct __attribute__((packed))
{
    u32 reserved0;
    u64 rsp0;
    u64 rsp1;
    u64 rsp2;
    u64 reserved1;
    u64 ist1;
    u64 ist2;
    u64 ist3;
    u64 ist4;
    u64 ist5;
    u64 ist6;
    u64 ist7;
    u64 reserved2;
    u16 reserved3;
    u16 iomap_base;
} tss64_t;

void tss_initialize(u64 kernel_stack);
tss64_t *tss_get(void);

#endif
