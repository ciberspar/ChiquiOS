
#include "driver_manager.h"
#include "../include/drivers.h"

static driver_info_t drivers[DRIVER_MAX_COUNT];
static u32 registered_count = 0;
static u8 manager_ready = 0;

static void copy_name(char *dest, const char *src)
{
    u32 i = 0;

    while (i < DRIVER_NAME_LENGTH - 1 && src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }

    dest[i] = '\0';
}

static const char *status_name(driver_status_t status)
{
    switch (status) {
    case DRIVER_STATUS_REGISTERED:
        return "Registrado";
    case DRIVER_STATUS_INITIALIZED:
        return "Iniciado";
    case DRIVER_STATUS_FAILED:
        return "Error";
    case DRIVER_STATUS_DISABLED:
        return "Desactivado";
    default:
        return "Vacio";
    }
}

chiquios_status_t driver_manager_initialize(void)
{
    u32 i;

    registered_count = 0;

    for (i = 0; i < DRIVER_MAX_COUNT; i++) {
        drivers[i].name[0] = '\0';
        drivers[i].vendor_id = 0;
        drivers[i].device_id = 0;
        drivers[i].status = DRIVER_STATUS_EMPTY;
        drivers[i].last_error = CHIQUIOS_OK;
    }

    manager_ready = 1;
    return CHIQUIOS_OK;
}

chiquios_status_t driver_register(
    const char *name,
    u16 vendor_id,
    u16 device_id)
{
    u32 i;

    if (!manager_ready || name == NULL || name[0] == '\0')
        return CHIQUIOS_ERROR;

    if (registered_count >= DRIVER_MAX_COUNT)
        return CHIQUIOS_ERROR;

    for (i = 0; i < registered_count; i++) {
        if (drivers[i].vendor_id == vendor_id &&
            drivers[i].device_id == device_id)
            return CHIQUIOS_ERROR;
    }

    copy_name(
        drivers[registered_count].name,
        name
    );

    drivers[registered_count].vendor_id = vendor_id;
    drivers[registered_count].device_id = device_id;
    drivers[registered_count].status = DRIVER_STATUS_REGISTERED;
    drivers[registered_count].last_error = CHIQUIOS_OK;

    registered_count++;

    return CHIQUIOS_OK;
}

void driver_manager_list(void)
{
    u32 i;

    vga_write("\n=== CHIQUIOS DRIVER MANAGER ===\n");

    for (i = 0; i < registered_count; i++) {
        vga_write("- ");
        vga_write(drivers[i].name);
        vga_write(" : ");
        vga_write(status_name(drivers[i].status));
        vga_write("\n");
    }

    vga_write("Total de drivers registrados: ");

    if (registered_count == 0)
        vga_write("0");

    vga_write("\n");
}

u32 driver_manager_count(void)
{
    return registered_count;
}
