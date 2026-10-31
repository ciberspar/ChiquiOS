
#include "gdt.h"

struct __attribute__((packed)) gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
};

struct __attribute__((packed)) gdt_pointer {
    uint16_t limit;
    uint64_t base;
};

static struct gdt_entry gdt[3];
static struct gdt_pointer gdtr;

static void gdt_set(
    int index,
    uint8_t access,
    uint8_t flags
) {
    gdt[index].limit_low = 0;
    gdt[index].base_low = 0;
    gdt[index].base_middle = 0;
    gdt[index].access = access;
    gdt[index].granularity = flags;
    gdt[index].base_high = 0;
}

void gdt_initialize(void)
{
    gdt_set(0, 0x00, 0x00);
    gdt_set(1, 0x9A, 0x20);
    gdt_set(2, 0x92, 0x00);

    gdtr.limit = sizeof(gdt) - 1;
    gdtr.base = (uint64_t)&gdt;

    __asm__ volatile (
        "lgdt %0"
        :
        : "m"(gdtr)
        : "memory"
    );
}
