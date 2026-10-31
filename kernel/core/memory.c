
#include "memory.h"

/*
 * ChiquiOS Memory Manager
 *
 * Initial implementation placeholder.
 * Physical memory allocation is not implemented yet.
 */

static const memory_map_t *system_memory_map = NULL;

chiquios_status_t memory_initialize(const memory_map_t *map)
{
    if (map == NULL || map->regions == NULL || map->count == 0) {
        return CHIQUIOS_ERROR;
    }

    system_memory_map = map;

    return CHIQUIOS_OK;
}

usize memory_get_total(void)
{
    (void)system_memory_map;
    return 0;
}

usize memory_get_available(void)
{
    (void)system_memory_map;
    return 0;
}
