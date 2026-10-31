
#include "gdt.h"
#include "tss.h"

typedef struct __attribute__((packed)) {
    u16 limit_low;
    u16 base_low;
    u8 base_middle;
    u8 access;
    u8 granularity;
    u8 base_high;
} gdt_entry_t;

typedef struct __attribute__((packed)) {
    u16 limit_low;
    u16 base_low;
    u8 base_middle;
    u8 access;
    u8 granularity;
    u8 base_high;
    u32 base_upper;
    u32 reserved;
} gdt_tss_entry_t;

typedef struct __attribute__((packed)) {
    gdt_entry_t entries[5];
    gdt_tss_entry_t tss;
} gdt_table_t;

typedef struct __attribute__((packed)) {
    u16 limit;
    u64 base;
} gdt_pointer_t;

static gdt_table_t gdt;

extern void gdt_load(const gdt_pointer_t *pointer);

static void set_entry(u32 index, u8 access, u8 flags)
{
    gdt.entries[index].limit_low = 0;
    gdt.entries[index].base_low = 0;
    gdt.entries[index].base_middle = 0;
    gdt.entries[index].access = access;
    gdt.entries[index].granularity = flags;
    gdt.entries[index].base_high = 0;
}

static void set_tss(tss64_t *tss)
{
    u64 base = (u64)tss;
    u32 limit = sizeof(tss64_t) - 1;

    gdt.tss.limit_low = (u16)limit;
    gdt.tss.base_low = (u16)base;
    gdt.tss.base_middle = (u8)(base >> 16);
    gdt.tss.access = 0x89;
    gdt.tss.granularity = (u8)((limit >> 16) & 0x0F);
    gdt.tss.base_high = (u8)(base >> 24);
    gdt.tss.base_upper = (u32)(base >> 32);
    gdt.tss.reserved = 0;
}

void gdt_initialize(u64 kernel_stack)
{
    set_entry(0, 0x00, 0x00);
    set_entry(1, 0x9A, 0x20);
    set_entry(2, 0x92, 0x00);
    set_entry(3, 0xFA, 0x20);
    set_entry(4, 0xF2, 0x00);

    tss_initialize(kernel_stack);
    set_tss(tss_get());

    gdt_pointer_t pointer;
    pointer.limit = sizeof(gdt) - 1;
    pointer.base = (u64)&gdt;

    gdt_load(&pointer);

    __asm__ volatile (
        "mov $0x28, %%ax\n"
        "ltr %%ax"
        :
        :
        : "rax", "memory"
    );
}
