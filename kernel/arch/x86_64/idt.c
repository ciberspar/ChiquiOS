
#include "idt.h"

static idt_entry_t idt[IDT_ENTRIES];

extern void (*isr_stub_table[32])(void);

void idt_set_gate(u8 vector, void (*handler)(void))
{
    if (handler == 0)
        return;

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
    for (u32 i = 0; i < IDT_ENTRIES; i++) {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].ist = 0;
        idt[i].attributes = 0;
        idt[i].offset_middle = 0;
        idt[i].offset_high = 0;
        idt[i].reserved = 0;
    }

    for (u32 i = 0; i < 32; i++)
        idt_set_gate((u8)i, isr_stub_table[i]);

    idt_pointer_t pointer;

    pointer.limit = sizeof(idt) - 1;
    pointer.base = (u64)&idt;

    __asm__ volatile (
        "lidt %0"
        :
        : "m"(pointer)
        : "memory"
    );
}
