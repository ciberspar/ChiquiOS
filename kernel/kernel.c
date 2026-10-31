#include "include/drivers.h"
#include "include/supervisor.h"
#include "tui.h"

#include "core/arch.h"
#include "core/arch_config.h"
#include "core/boot_info.h"
#include "core/memory.h"
#include "core/pmm.h"
#include "core/driver_manager.h"

static void kernel_panic(const char *message)
{
    arch_disable_interrupts();
    vga_write(message);

    for (;;) {
        arch_halt();
    }
}

void kernel_main(const chiquios_boot_info_t *boot_info)
{
    vga_initialize();

    vga_write("================================\n");
    vga_write("          ChiquiOS 1.0\n");
    vga_write("       Kernel propio i386\n");
    vga_write("================================\n\n");

    if (boot_info == NULL)
        kernel_panic("ERROR: informacion de arranque nula.\n");

    if (boot_info->magic != CHIQUIOS_BOOT_MAGIC ||
        boot_info->version != CHIQUIOS_BOOT_VERSION)
        kernel_panic("ERROR: informacion de arranque invalida.\n");

    if (boot_info->boot_method != CHIQUIOS_BOOT_BIOS)
        kernel_panic("ERROR: metodo de arranque incompatible.\n");

#if CHIQUIOS_BUILD_I386
    if (boot_info->architecture != CHIQUIOS_ARCH_I386)
        kernel_panic("ERROR: arquitectura incompatible con i386.\n");
#elif CHIQUIOS_BUILD_X86_64
    if (boot_info->architecture != CHIQUIOS_ARCH_X86_64)
        kernel_panic("ERROR: arquitectura incompatible con x86_64.\n");
#endif

    if (memory_initialize(&boot_info->memory) != CHIQUIOS_OK)
        kernel_panic("ERROR: mapa de memoria invalido.\n");

    if (pmm_initialize(&boot_info->memory) != CHIQUIOS_OK)
        kernel_panic("ERROR: no se pudo iniciar PMM.\n");

    vga_write("Arquitectura: ");
    vga_write(CHIQUIOS_ARCH_NAME);
    vga_write("\n");

    vga_write("Mapa de memoria: detectado\n");
    vga_write("Gestor de memoria fisica: iniciado\n");

    cpu_detect();

    if (driver_manager_initialize() != CHIQUIOS_OK)
        kernel_panic("ERROR: Driver Manager.\n");

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
        arch_halt();
    }
}