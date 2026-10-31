
#include "../include/drivers.h"
#include "memory_monitor.h"
#include "memory.h"
#include "pmm.h"

static void print_number(u32 value)
{
    char buffer[11];
    u32 i = 0;

    if (value == 0) {
        vga_write("0");
        return;
    }

    while (value > 0) {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }

    while (i > 0) {
        char c[2] = { buffer[--i], '\0' };
        vga_write(c);
    }
}

void memory_monitor_initialize(void)
{
}

void memory_monitor_print(void)
{
    pmm_stats_t stats;

    pmm_get_stats(&stats);

    vga_write("\n--- CHIQUIOS MEMORY MONITOR ---\n");

    vga_write("RAM utilizable: ");
    print_number((u32)(memory_get_total() / 1048576ULL));
    vga_write(" MiB\n");

    vga_write("RAM libre: ");
    print_number((u32)(stats.free_pages * 4ULL / 1024ULL));
    vga_write(" MiB\n");

    vga_write("RAM usada: ");
    print_number((u32)(stats.used_pages * 4ULL / 1024ULL));
    vga_write(" MiB\n");

    vga_write("Paginas libres: ");
    print_number((u32)stats.free_pages);
    vga_write("\n");

    vga_write("Paginas usadas: ");
    print_number((u32)stats.used_pages);
    vga_write("\n");

    vga_write("Paginas reservadas: ");
    print_number((u32)stats.reserved_pages);
    vga_write("\n");

    vga_write("--------------------------------\n");
}
