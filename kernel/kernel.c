
#include "include/drivers.h"
#include "include/supervisor.h"

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
    vga_write("Teclado PS/2: modo de consulta\n");
    vga_write("Escribe help para ver los comandos.\n");

    shell_run();

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
