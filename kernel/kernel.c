
#include "core/types.h"
#include "core/arch.h"
#include "core/arch_config.h"
#include "core/boot_info.h"

#include "include/drivers.h"
#include "drivers/serial.h"

extern const char *chiquios_cpp_module_name(void);

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

    for (;;)
        arch_halt();
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
    vga_initialize();
    serial_initialize();

    kernel_log("Iniciando ChiquiOS");
    kernel_log(chiquios_cpp_module_name());

#if CHIQUIOS_BUILD_X86_64
    kernel_log("Arquitectura: x86-64");
#elif CHIQUIOS_BUILD_I386
    kernel_log("Arquitectura: i386");
#else
    kernel_panic("Arquitectura no compatible");
#endif

    if (boot_info == 0)
        kernel_panic("Boot information inexistente");

    kernel_log("Boot information recibida");

    kernel_log("Detectando procesador");
    cpu_detect();

    kernel_log("Buscando dispositivos PCI");
    pci_scan_all();

    kernel_log("Buscando controladores USB");
    pci_scan_usb();

    kernel_log("Detectando adaptadores de red");
    internet_initialize();

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

    for (;;)
        arch_halt();
}
