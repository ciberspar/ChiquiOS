
#ifndef CHIQUIOS_DRIVER_MANAGER_H
#define CHIQUIOS_DRIVER_MANAGER_H

#include "types.h"

#define DRIVER_MAX_COUNT 128
#define DRIVER_NAME_LENGTH 32

typedef enum {
    DRIVER_STATUS_EMPTY = 0,
    DRIVER_STATUS_REGISTERED,
    DRIVER_STATUS_INITIALIZED,
    DRIVER_STATUS_FAILED,
    DRIVER_STATUS_DISABLED
} driver_status_t;

typedef struct {
    char name[DRIVER_NAME_LENGTH];
    u16 vendor_id;
    u16 device_id;
    driver_status_t status;
    chiquios_status_t last_error;
} driver_info_t;

chiquios_status_t driver_manager_initialize(void);

chiquios_status_t driver_register(
    const char *name,
    u16 vendor_id,
    u16 device_id
);

const driver_info_t *driver_find(u16 vendor_id, u16 device_id);

void driver_manager_list(void);
u32 driver_manager_count(void);

#endif
