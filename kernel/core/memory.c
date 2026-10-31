
#include "memory.h"

#define MEMORY_MAP_ADDRESS_LIMIT 0x100000000ULL

static const memory_map_t *active_map;
static u64 total_usable;
static u64 available_usable;

chiquios_status_t memory_initialize(const memory_map_t *map)
{
    u32 i;
    const memory_region_t *regions;
    u64 total = 0;

    if (map == NULL ||
        map->regions_address == 0 ||
        map->count == 0 ||
        map->count > MEMORY_MAX_REGIONS) {
        return CHIQUIOS_ERROR;
    }

    regions = (const memory_region_t *)(uptr)map->regions_address;

    for (i = 0; i < map->count; i++) {
        const memory_region_t *region = &regions[i];
        u64 end;

        if (region->length == 0)
            continue;

        end = region->base + region->length;

        if (end < region->base)
            return CHIQUIOS_ERROR;

        if (region->type == MEMORY_TYPE_USABLE) {
            u64 start = region->base;
            u64 finish = end;

            if (start >= MEMORY_MAP_ADDRESS_LIMIT)
                continue;

            if (finish > MEMORY_MAP_ADDRESS_LIMIT)
                finish = MEMORY_MAP_ADDRESS_LIMIT;

            if (finish > start) {
                u64 amount = finish - start;

                if (total > (~(u64)0) - amount)
                    return CHIQUIOS_ERROR;

                total += amount;
            }
        }
    }

    if (total == 0)
        return CHIQUIOS_ERROR;

    active_map = map;
    total_usable = total;
    available_usable = total;

    return CHIQUIOS_OK;
}

u64 memory_get_total(void)
{
    return total_usable;
}

u64 memory_get_available(void)
{
    return available_usable;
}

const memory_map_t *memory_get_map(void)
{
    return active_map;
}
