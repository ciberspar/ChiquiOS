#include "memory.h"
#include "pmm.h"

static const memory_map_t *active_map;
static u64 total_usable;

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
        u64 start;
        u64 amount;

        if (region->length == 0)
            continue;

        if (region->type != MEMORY_TYPE_USABLE)
            continue;

        start = region->base;

        if (start >= MEMORY_MANAGED_LIMIT)
            continue;

        if (region->length > (~(u64)0) - start)
            end = MEMORY_MANAGED_LIMIT;
        else
            end = start + region->length;

        if (end > MEMORY_MANAGED_LIMIT)
            end = MEMORY_MANAGED_LIMIT;

        if (end <= start)
            continue;

        amount = end - start;

        if (total > (~(u64)0) - amount)
            return CHIQUIOS_ERROR;

        total += amount;
    }

    if (total == 0)
        return CHIQUIOS_ERROR;

    active_map = map;
    total_usable = total;

    return CHIQUIOS_OK;
}

u64 memory_get_total(void)
{
    return total_usable;
}

u64 memory_get_available(void)
{
    pmm_stats_t stats;

    if (!pmm_is_initialized())
        return total_usable;

    pmm_get_stats(&stats);

    return stats.free_pages * PMM_PAGE_SIZE;
}

const memory_map_t *memory_get_map(void)
{
    return active_map;
}