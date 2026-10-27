
#include "include/drivers.h"
#include "include/supervisor.h"
#include "include/tui.h"

void kernel_main(void)
{
    vga_initialize();

    vga_write("================================\n");
    vga_write("          ChiquiOS 0.5\n");
    vga_write("       Kernel propio x86\n");
    vga_write("================================\n\n");

    cpu_detect();
    supervisor_initialize();

    vga_write("Supervisor: inicializado\n");
    vga_write("Iniciando interfaz de texto...\n");

    tui_run();

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
