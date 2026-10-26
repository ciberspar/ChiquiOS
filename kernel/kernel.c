
#include "include/drivers.h"

void kernel_main(void) {
    vga_initialize();

    vga_write("================================\n");
    vga_write("          ChiquiOS 0.2\n");
    vga_write("     32-bit x86 CLI system\n");
    vga_write("================================\n\n");

    cpu_detect();
    vga_write("PS/2 keyboard: polling mode\n");
    vga_write("Type help to see commands.\n\n");

    shell_run();

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
