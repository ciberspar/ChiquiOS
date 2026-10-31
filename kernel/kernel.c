
#include "include/drivers.h"
#include "include/supervisor.h"
#include "tui.h"

#include "core/boot_info.h"
#include "core/memory.h"
#include "core/pmm.h"
#include "core/driver_manager.h"

void kernel_main(const chiquios_boot_info_t *boot_info)
{
    pmm_stats_t memory_stats;

    vga_initialize();

    vga_write("================================\n");
    vga_write("          ChiquiOS 1.0\n");
    vga_write("       Kernel propio i386\n");
    vga_write("================================\n\n");

    if (boot_info == NULL ||
        boot_info->magic != CHIQUIOS_BOOT_MAGIC ||
        boot_info->version != CHIQUIOS_BOOT_VERSION) {
        vga_write("ERROR: informacion de arranque invalida.\n");

        for (;;) {
            __asm__ volatile ("cli; hlt");
        }
    }

    if (memory_initialize(&boot_info->memory) != CHIQUIOS_OK) {
        vga_write("ERROR: mapa de memoria invalido.\n");

        for (;;) {
            __asm__ volatile ("cli; hlt");
        }
    }

    if (pmm_initialize(&boot_info->memory) != CHIQUIOS_OK) {
        vga_write("ERROR: no se pudo iniciar PMM.\n");

        for (;;) {
            __asm__ volatile ("cli; hlt");
        }
    }

    pmm_get_stats(&memory_stats);

    vga_write("Mapa de memoria: detectado\n");
    vga_write("Gestor de memoria fisica: iniciado\n");

    cpu_detect();

    if (driver_manager_initialize() != CHIQUIOS_OK) {
        vga_write("ERROR: Driver Manager.\n");

        for (;;) {
            __asm__ volatile ("cli; hlt");
        }
    }

    /* Intel E1000: IDs reconocidos por el detector actual. */
    driver_register("Intel E1000", 0x8086, 0x100E);
    driver_register("Intel E1000", 0x8086, 0x100F);
    driver_register("Intel E1000", 0x8086, 0x1004);
    driver_register("Intel E1000", 0x8086, 0x10D3);
    driver_register("Intel E1000", 0x8086, 0x153A);

    vga_write("\nDriver Manager: inicializado\n");

    driver_manager_list();

    pci_scan_all();

    supervisor_initialize();

    vga_write("\nSupervisor: inicializado\n");
    vga_write("Iniciando interfaz de texto...\n");

    tui_run();

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
