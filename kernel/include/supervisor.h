
#ifndef CHIQUIOS_SUPERVISOR_H
#define CHIQUIOS_SUPERVISOR_H

#include "drivers.h"

typedef enum {
    SERVICE_STOPPED = 0,
    SERVICE_RUNNING,
    SERVICE_FAILED
} service_state_t;

typedef struct {
    const char *name;
    void (*initialize)(void);
    service_state_t state;
    u32 restarts;
} service_t;

void supervisor_initialize(void);
void supervisor_list(void);
int supervisor_restart(const char *name);

#endif
