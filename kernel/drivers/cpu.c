
#include "../include/drivers.h"

void cpu_detect(void) {
    u32 a, b, c, d;
    char vendor[13];

    __asm__ volatile (
        "pushl %%ebx\n\t"
        "cpuid\n\t"
        "movl %%ebx, %1\n\t"
        "movl %%edx, %2\n\t"
        "movl %%ecx, %3\n\t"
        "popl %%ebx"
        : "=a"(a), "=m"(*(u32 *)&vendor[0]),
          "=m"(*(u32 *)&vendor[4]),
          "=m"(*(u32 *)&vendor[8])
        : "a"(0)
        : "ecx", "edx", "memory"
    );

    vendor[12] = '\0';

    vga_write("CPU vendor: ");
    vga_write(vendor);
    vga_write("\n");
}
