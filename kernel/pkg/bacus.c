
#include "../include/drivers.h"

void bacus_show_status(void)
{
    vga_write("Bacus-Pkg para ChiquiOS 0.5\n");
    vga_write("Estado: interfaz inicial.\n");
    vga_write("Instalacion .bp: pendiente de almacenamiento.\n");
}
