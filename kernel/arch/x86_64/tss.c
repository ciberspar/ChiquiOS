
#include "tss.h"

static tss64_t kernel_tss;

void tss_initialize(u64 kernel_stack)
{
    u8 *bytes = (u8 *)&kernel_tss;

    for (u32 i = 0; i < sizeof(tss64_t); i++)
        bytes[i] = 0;

    kernel_tss.rsp0 = kernel_stack;
    kernel_tss.iomap_base = sizeof(tss64_t);
}

tss64_t *tss_get(void)
{
    return &kernel_tss;
}
