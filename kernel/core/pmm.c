#include "pmm.h"

#define PMM_MAX_PAGES (MEMORY_MANAGED_LIMIT / PMM_PAGE_SIZE)
#define PMM_STATE_BYTES (PMM_MAX_PAGES / 4ULL)

#define PAGE_FREE      0
#define PAGE_USED      1
#define PAGE_RESERVED  2
#define PAGE_UNMANAGED 3

/*
 * Cada página utiliza 2 bits.
 *
 * i386:
 *   4 GiB administrables.
 *
 * x86-64:
 *   16 GiB administrables.
 */

static u8 page_states[PMM_STATE_BYTES];
static pmm_stats_t statistics;
static int initialized;

extern u8 _kernel_start;
extern u8 _kernel_end;

static u8 get_state(u64 page)
{
    u32 shift = (u32)((page & 3ULL) * 2ULL);

    return (page_states[page >> 2] >> shift) & 3U;
}

static void set_state(u64 page, u8 state)
{
    u32 shift = (u32)((page & 3ULL) * 2ULL);
    u8 mask = (u8)(3U << shift);

    page_states[page >> 2] &= (u8)~mask;
    page_states[page >> 2] |= (u8)((state & 3U) << shift);
}

static void reserve_range(u64 start, u64 end)
{
    u64 first;
    u64 last;
    u64 page;

    if (start >= MEMORY_MANAGED_LIMIT)
        return;

    if (end > MEMORY_MANAGED_LIMIT)
        end = MEMORY_MANAGED_LIMIT;

    if (end <= start)
        return;

    first = start / PMM_PAGE_SIZE;

    last = end / PMM_PAGE_SIZE;

    if ((end % PMM_PAGE_SIZE) != 0)
        last++;

    if (last > PMM_MAX_PAGES)
        last = PMM_MAX_PAGES;

    for (page = first; page < last; page++) {
        if (get_state(page) == PAGE_FREE) {
            set_state(page, PAGE_RESERVED);

            statistics.free_pages--;
            statistics.reserved_pages++;
        }
    }
}

chiquios_status_t pmm_initialize(const memory_map_t *map)
{
    const memory_region_t *regions;
    u32 i;
    u64 page;
    u64 byte;

    if (initialized ||
        map == NULL ||
        map->regions_address == 0 ||
        map->count == 0 ||
        map->count > MEMORY_MAX_REGIONS) {
        return CHIQUIOS_ERROR;
    }

    for (byte = 0; byte < PMM_STATE_BYTES; byte++)
        page_states[byte] = 0xFF;

    statistics.total_pages = 0;
    statistics.free_pages = 0;
    statistics.used_pages = 0;
    statistics.reserved_pages = 0;

    regions = (const memory_region_t *)(uptr)map->regions_address;

    for (i = 0; i < map->count; i++) {
        u64 start;
        u64 end;
        u64 first;
        u64 last;

        if (regions[i].type != MEMORY_TYPE_USABLE ||
            regions[i].length == 0)
            continue;

        start = regions[i].base;

        if (start >= MEMORY_MANAGED_LIMIT)
            continue;

        if (regions[i].length > (~(u64)0) - start)
            end = MEMORY_MANAGED_LIMIT;
        else
            end = start + regions[i].length;

        if (end > MEMORY_MANAGED_LIMIT)
            end = MEMORY_MANAGED_LIMIT;

        if (end <= start)
            continue;

        first = start / PMM_PAGE_SIZE;

        if ((start % PMM_PAGE_SIZE) != 0)
            first++;

        last = end / PMM_PAGE_SIZE;

        if (last > PMM_MAX_PAGES)
            last = PMM_MAX_PAGES;

        for (page = first; page < last; page++) {
            if (get_state(page) == PAGE_UNMANAGED) {
                set_state(page, PAGE_FREE);

                statistics.total_pages++;
                statistics.free_pages++;
            }
        }
    }

    if (statistics.free_pages == 0)
        return CHIQUIOS_ERROR;

    /* Reservar los primeros 1 MiB. */
    reserve_range(0, 0x100000ULL);

    /* Reservar el kernel y su sección BSS. */
    reserve_range(
        (u64)(uptr)&_kernel_start,
        (u64)(uptr)&_kernel_end
    );

    if (statistics.free_pages == 0)
        return CHIQUIOS_ERROR;

    initialized = 1;

    return CHIQUIOS_OK;
}

u64 pmm_allocate_page(void)
{
    u64 page;

    if (!initialized)
        return 0;

    for (page = 0; page < PMM_MAX_PAGES; page++) {
        if (get_state(page) == PAGE_FREE) {
            set_state(page, PAGE_USED);

            statistics.free_pages--;
            statistics.used_pages++;

            return page * PMM_PAGE_SIZE;
        }
    }

    return 0;
}

chiquios_status_t pmm_free_page(u64 address)
{
    u64 page;

    if (!initialized ||
        address == 0 ||
        (address & (PMM_PAGE_SIZE - 1)) != 0) {
        return CHIQUIOS_ERROR;
    }

    page = address / PMM_PAGE_SIZE;

    if (page >= PMM_MAX_PAGES ||
        get_state(page) != PAGE_USED) {
        return CHIQUIOS_ERROR;
    }

    set_state(page, PAGE_FREE);

    statistics.used_pages--;
    statistics.free_pages++;

    return CHIQUIOS_OK;
}

void pmm_get_stats(pmm_stats_t *stats)
{
    if (stats == NULL)
        return;

    *stats = statistics;
}

int pmm_is_initialized(void)
{
    return initialized;
}