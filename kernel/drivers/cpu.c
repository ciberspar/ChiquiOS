
#include "../include/drivers.h"

static void cpuid(
    uint32_t leaf,
    uint32_t *a,
    uint32_t *b,
    uint32_t *c,
    uint32_t *d
) {
    __asm__ volatile (
        "cpuid"
        : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
        : "a"(leaf), "c"(0)
    );
}

void cpu_detect(void) {
    uint32_t a, b, c, d;

    cpuid(0, &a, &b, &c, &d);

    /* El kernel sigue siendo x86 de 32 bits.
       La detección detallada se mostrará más adelante. */
    (void)a;
    (void)b;
    (void)c;
    (void)d;
}
