
#ifndef CHIQUIOS_X64_IDT_H
#define CHIQUIOS_X64_IDT_H

#include "../../core/types.h"

#define IDT_ENTRIES 256

typedef struct __attribute__((packed)) {
    u16 offset_low;
    u16 selector;
    u8 ist;
    u8 attributes;
    u16 offset_middle;
    u32 offset_high;
    u32 reserved;
} idt_entry_t;

typedef struct __attribute__((packed)) {
    u16 limit;
    u64 base;
} idt_pointer_t;

void idt_initialize(void);
void idt_set_gate(u8 vector, void (*handler)(void));

#endif
