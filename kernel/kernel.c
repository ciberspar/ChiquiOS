
#include "core/types.h"
#include "core/arch.h"
#include "core/arch_config.h"
#include "core/boot_info.h"

#include "include/drivers.h"
#include "drivers/serial.h"

static void kernel_panic(const char *message)
{
    arch_disable_interrupts();

    vga_clear();
    vga_write("\n");
    vga_write("================================\n");
    vga_write("       CHIQUIOS - KERNEL PANIC\n");
    vga_write("================================\n");
    vga_write(message);
    vga_write("\nSistema detenido.\n");

    if (serial_is_initialized()) {
        serial_write_string("[PANIC] ");
        serial_write_string(message);
        serial_write_string("\n");
    }

    for (;;) {
        arch_halt();
    }
}

static void kernel_log(const char *message)
{
    vga_write("[ChiquiOS] ");
    vga_write(message);
    vga_write("\n");

    serial_write_string("[ChiquiOS] ");
    serial_write_string(message);
    serial_write_string("\n");
}

void kernel_main(const chiquios_boot_info_t *boot_info)
{
    /* 1. Inicializar salida */

    vga_initialize();
    serial_initialize();

    kernel_log("Iniciando ChiquiOS");

#if CHIQUIOS_BUILD_X86_64
    kernel_log("Arquitectura: x86-64");
#elif CHIQUIOS_BUILD_I386
    kernel_log("Arquitectura: i386");
#else
    kernel_panic("Arquitectura no compatible");
#endif

    /* 2. Comprobar informacion de arranque */

    if (boot_info == 0) {
        kernel_panic("Boot information inexistente");
    }

    kernel_log("Boot information recibida");

    /* 3. Detectar procesador */

    kernel_log("Detectando CPU");
    cpu_detect();

    /* 4. Detectar dispositivos PCI */

    kernel_log("Buscando dispositivos PCI");
    pci_scan_all();

    /* 5. Detectar controladores USB */

    kernel_log("Buscando controladores USB");
    pci_scan_usb();

    /* 6. Iniciar deteccion de red */

    kernel_log("Detectando adaptadores de red");
    internet_initialize();

    /* 7. Mensaje final */

    kernel_log("Inicializacion basica completada");

    vga_write("\n");
    vga_write("================================\n");
    vga_write("           CHIQUIOS\n");
    vga_write("================================\n");

#if CHIQUIOS_BUILD_X86_64
    vga_write("Arquitectura: x86-64\n");
#else
    vga_write("Arquitectura: i386\n");
#endif

    vga_write("Estado: kernel en ejecucion\n");

    /* Esperar al siguiente subsistema */

    for (;;) {
        arch_halt();
    }
}
