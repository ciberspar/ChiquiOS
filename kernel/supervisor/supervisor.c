
#include "../include/supervisor.h"

static service_t services[] = {
    { "usb", usb_initialize, SERVICE_STOPPED, 0 },
    { "internet", internet_initialize, SERVICE_STOPPED, 0 }
};

#define SERVICE_COUNT (sizeof(services) / sizeof(services[0]))

void supervisor_initialize(void)
{
    u32 i;

    for (i = 0; i < SERVICE_COUNT; i++) {
        services[i].state = SERVICE_RUNNING;
        services[i].restarts = 0;
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

        if (services[i].state == SERVICE_RUNNING) {
            vga_write("REGISTRADO");
        } else if (services[i].state == SERVICE_FAILED) {
            vga_write("FALLIDO");
        } else {
            vga_write("DETENIDO");
        }

        vga_write("\n");
    }

    vga_write("Nota: REGISTRADO no garantiza que el hardware funcione.\n");
}

int supervisor_restart(const char *name)
{
    u32 i;

    for (i = 0; i < SERVICE_COUNT; i++) {
        const char *a = name;
        const char *b = services[i].name;

        while (*a && *b && *a == *b) {
            a++;
            b++;
        }

        if (*a == '\0' && *b == '\0') {
            services[i].state = SERVICE_STOPPED;

            if (services[i].initialize != 0) {
                services[i].initialize();
            }

            services[i].restarts++;
            services[i].state = SERVICE_RUNNING;

            vga_write("Servicio reinicializado: ");
            vga_write(services[i].name);
            vga_write("\n");

            return 1;
        }
    }

    vga_write("Servicio desconocido.\n");
    return 0;
}
