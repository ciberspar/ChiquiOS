
#include "../include/supervisor.h"

static service_t services[] = {
    { "usb", usb_initialize, SERVICE_STOPPED, 0, CHIQUIOS_OK },
    { "internet", internet_initialize, SERVICE_STOPPED, 0, CHIQUIOS_OK }
};

#define SERVICE_COUNT (sizeof(services) / sizeof(services[0]))

static int name_equal(const char *a, const char *b)
{
    if (a == NULL || b == NULL)
        return 0;

    while (*a && *b) {
        if (*a != *b)
            return 0;
        a++;
        b++;
    }

    return *a == '\0' && *b == '\0';
}

static void service_start(service_t *service)
{
    if (service == NULL)
        return;

    service->state = SERVICE_STARTING;

    if (service->initialize == NULL) {
        service->state = SERVICE_FAILED;
        service->last_error = CHIQUIOS_ERROR;
        return;
    }

    /*
     * Las funciones actuales devuelven void.
     * Por ahora solo podemos registrar que fueron llamadas.
     * No significa que el hardware esté funcionando.
     */
    service->initialize();

    service->last_error = CHIQUIOS_OK;
    service->state = SERVICE_RUNNING;
}

void supervisor_initialize(void)
{
    u32 i;

    for (i = 0; i < SERVICE_COUNT; i++) {
        services[i].state = SERVICE_STOPPED;
        services[i].restarts = 0;
        services[i].last_error = CHIQUIOS_OK;

        service_start(&services[i]);
    }
}

void supervisor_list(void)
{
    u32 i;

    vga_write("Servicios de ChiquiOS:\n");

    for (i = 0; i < SERVICE_COUNT; i++) {
        vga_write("  ");
        vga_write(services[i].name);
        vga_write(" : ");

        switch (services[i].state) {
        case SERVICE_STARTING:
            vga_write("INICIANDO");
            break;
        case SERVICE_RUNNING:
            vga_write("INICIALIZADO");
            break;
        case SERVICE_FAILED:
            vga_write("FALLIDO");
            break;
        default:
            vga_write("DETENIDO");
            break;
        }

        vga_write("\n");
    }

    vga_write("Nota: inicializado no significa hardware operativo.\n");
}

int supervisor_restart(const char *name)
{
    u32 i;

    for (i = 0; i < SERVICE_COUNT; i++) {
        if (!name_equal(name, services[i].name))
            continue;

        services[i].state = SERVICE_STOPPED;

        service_start(&services[i]);

        if (services[i].state == SERVICE_RUNNING) {
            services[i].restarts++;

            vga_write("Servicio reinicializado: ");
            vga_write(services[i].name);
            vga_write("\n");

            return 1;
        }

        vga_write("Error al inicializar servicio: ");
        vga_write(services[i].name);
        vga_write("\n");

        return 0;
    }

    vga_write("Servicio desconocido.\n");
    return 0;
}
