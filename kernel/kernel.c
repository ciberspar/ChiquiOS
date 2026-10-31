
#include "core/types.h"
#include "core/memory.h"
#include "core/pmm.h"
#include "core/driver_manager.h"
#include "core/arch.h"
#include "core/arch_config.h"

#include "include/drivers.h"
#include "include/supervisor.h"

#include "drivers/vga.h"
#include "drivers/serial.h"

#include "arch/x86_64/gdt.h"

static void kernel_panic(const char *message)
{
    arch_disable_interrupts();

    vga_clear();
    vga_write_string("\n");
    vga_write_string("================================\n");
    vga_write_string("       CHIQUIOS - KERNEL PANIC\n");
    vga_write_string("================================\n");
    vga_write_string(message);
    vga_write_string("\nSistema detenido.\n");

    if (serial_is_initialized())
    {
        serial_write_string("[PANIC] ");
        serial_write_string(message);
        serial_write_string("\n");
    }

    for (;;)
        arch_halt();
}

static void kernel_log(const char *message)
{
    vga_write_string("[ChiquiOS] ");
    vga_write_string(message);
    vga_write_string("\n");

    serial_write_string("[ChiquiOS] ");
    serial_write_string(message);
    serial_write_string("\n");
}

void kernel_main(void *boot_info)
{
    /*
     * 1. Inicialización de salida.
     */

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

    /*
     * 2. Validación del cargador.
     */

    if (boot_info == 0)
        kernel_panic("Boot information inexistente");

    kernel_log("Boot information recibida");

    /*
     * 3. Arquitectura.
     *
     * El cargador ya debe haber establecido el modo
     * de CPU correspondiente antes de entrar aquí.
     */

#if CHIQUIOS_BUILD_X86_64
    kernel_log("Ejecutando en modo x86-64");
#endif

    /*
     * 4. Memoria física.
     *
     * El formato de boot_info debe coincidir con
     * memory_map_t antes de utilizarlo.
     *
     * No convertir punteros ni estructuras a ciegas.
     */

    kernel_log("Inicializando memoria");

    /*
     * La conversión del mapa del cargador a memory_map_t
     * se integrará cuando se unifique el protocolo de arranque.
     */

    kernel_log("Memoria: inicializacion pendiente de protocolo");

    /*
     * 5. CPU.
     */

    kernel_log("Detectando CPU");

    cpu_detect();

    /*
     * 6. PCI.
     */

    kernel_log("Inicializando PCI");

    pci_scan();

    /*
     * 7. Drivers.
     */

    kernel_log("Inicializando dispositivos");

    /*
     * El gestor de drivers registra dispositivos.
     * La inicialización individual debe comprobar
     * el resultado de cada controlador.
     */

    /*
     * 8. Supervisor.
     */

    kernel_log("Preparando supervisor");

    /*
     * No iniciar servicios que todavía no tengan
     * inicializadores comprobables.
     */

    /*
     * 9. Interfaz.
     */

    kernel_log("ChiquiOS kernel iniciado");

    vga_write_string("\n");
    vga_write_string("================================\n");
    vga_write_string("           CHIQUIOS\n");
    vga_write_string("================================\n");
    vga_write_string("Arquitectura: ");
#if CHIQUIOS_BUILD_X86_64
    vga_write_string("x86-64\n");
#else
    vga_write_string("i386\n");
#endif
    vga_write_string("Estado: kernel en ejecucion\n");

    /*
     * Bucle provisional hasta disponer de scheduler.
     */

    for (;;)
        arch_halt();
}
