
#ifndef CHIQUIOS_SUPERVISOR_H
#define CHIQUIOS_SUPERVISOR_H

#include "../core/types.h"
#include "drivers.h"

typedef enum {
    SERVICE_STOPPED = 0,
    SERVICE_STARTING,
    SERVICE_RUNNING,
    SERVICE_FAILED
} service_state_t;

typedef struct {
    const char *name;
    void (*initialize)(void);
    service_state_t state;
    u32 restarts;
    chiquios_status_t last_error;
} service_t;

void supervisor_initialize(void);
void supervisor_list(void);
int supervisor_restart(const char *name);

#endif
