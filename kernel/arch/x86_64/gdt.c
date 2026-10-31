
#include "gdt.h"
#include "tss.h"

struct __attribute__((packed)) gdt_entry {
    u16 limit_low;
    u16 base_low;
    u8 base_middle;
    u8 access;
    u8 granularity;
    u8 base_high;
};

struct __attribute__((packed)) gdt_tss_entry {
    u16 limit_low;
    u16 base_low;
    u8 base_middle;
    u8 access;
    u8 granularity;
    u8 base_high;
    u32 base_upper;
    u32 reserved;
};

struct __attribute__((packed)) gdt_pointer {
    u16 limit;
    u64 base;
};

static struct gdt_entry gdt[5];
static struct gdt_tss_entry tss_descriptor;
static struct gdt_pointer gdtr;

extern void gdt_load(const struct gdt_pointer *pointer);

static void gdt_set(
    u32 index,
    u8 access,
    u8 flags
) {
    gdt[index].limit_low = 0;
    gdt[index].base_low = 0;
    gdt[index].base_middle = 0;
    gdt[index].access = access;
    gdt[index].granularity = flags;
    gdt[index].base_high = 0;
}

static void gdt_set_tss(tss64_t *tss)
{
    u64 base = (u64)tss;
    u32 limit = sizeof(tss64_t) - 1;

    tss_descriptor.limit_low = limit & 0xFFFF;
    tss_descriptor.base_low = base & 0xFFFF;
    tss_descriptor.base_middle = (base >> 16) & 0xFF;
    tss_descriptor.access = 0x89;
    tss_descriptor.granularity = (limit >> 16) & 0x0F;
    tss_descriptor.base_high = (base >> 24) & 0xFF;
    tss_descriptor.base_upper = (u32)(base >> 32);
    tss_descriptor.reserved = 0;
}

void gdt_initialize(u64 kernel_stack)
{
    gdt_set(0, 0x00, 0x00);
    gdt_set(1, 0x9A, 0x20);
    gdt_set(2, 0x92, 0x00);
    gdt_set(3, 0xFA, 0x20);
    gdt_set(4, 0xF2, 0x00);

    tss_initialize(kernel_stack);
    gdt_set_tss(tss_get());

    gdtr.limit = sizeof(gdt) + sizeof(tss_descriptor) - 1;
    gdtr.base = (u64)&gdt[0];

    gdt_load(&gdtr);

    __asm__ volatile (
        "mov $0x28, %%ax\n"
        "ltr %%ax\n"
        :
        :
        : "rax", "memory"
    );
}
