
#include "idt.h"

static idt_entry_t idt[IDT_ENTRIES];
static idt_pointer_t idtr;

void idt_set_gate(u8 vector, void (*handler)(void))
{
    u64 address = (u64)handler;

    idt[vector].offset_low = (u16)address;
    idt[vector].selector = 0x08;
    idt[vector].ist = 0;
    idt[vector].attributes = 0x8E;
    idt[vector].offset_middle = (u16)(address >> 16);
    idt[vector].offset_high = (u32)(address >> 32);
    idt[vector].reserved = 0;
}

void idt_initialize(void)
{
    u32 i;

    for (i = 0; i < IDT_ENTRIES; i++) {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].ist = 0;
        idt[i].attributes = 0;
        idt[i].offset_middle = 0;
        idt[i].offset_high = 0;
        idt[i].reserved = 0;
    }

    idtr.limit = sizeof(idt) - 1;
    idtr.base = (u64)&idt;

    __asm__ volatile (
        "lidt %0"
        :
        : "m"(idtr)
        : "memory"
    );
}
