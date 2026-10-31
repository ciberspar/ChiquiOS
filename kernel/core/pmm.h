
#ifndef CHIQUIOS_CORE_PMM_H
#define CHIQUIOS_CORE_PMM_H

#include "types.h"
#include "memory.h"

#define PMM_PAGE_SIZE 4096ULL

typedef struct {
    u64 total_pages;
    u64 free_pages;
    u64 used_pages;
    u64 reserved_pages;
} pmm_stats_t;

chiquios_status_t pmm_initialize(const memory_map_t *map);

u64 pmm_allocate_page(void);
chiquios_status_t pmm_free_page(u64 address);

void pmm_get_stats(pmm_stats_t *stats);
int pmm_is_initialized(void);

#endif
