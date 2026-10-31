
#include "../include/drivers.h"

void cpu_detect(void)
{
    u32 a, b, c, d;
    char vendor[13];

    __asm__ volatile (
        "cpuid"
        : "=a"(a),
          "=b"(b),
          "=c"(c),
          "=d"(d)
        : "a"(0)
    );

    vendor[0]  = (char)(b);
    vendor[1]  = (char)(b >> 8);
    vendor[2]  = (char)(b >> 16);
    vendor[3]  = (char)(b >> 24);

    vendor[4]  = (char)(d);
    vendor[5]  = (char)(d >> 8);
    vendor[6]  = (char)(d >> 16);
    vendor[7]  = (char)(d >> 24);

    vendor[8]  = (char)(c);
    vendor[9]  = (char)(c >> 8);
    vendor[10] = (char)(c >> 16);
    vendor[11] = (char)(c >> 24);

    vendor[12] = '\0';

    vga_write("CPU vendor: ");
    vga_write(vendor);
    vga_write("\n");
}
