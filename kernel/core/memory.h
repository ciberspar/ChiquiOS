
#ifndef CHIQUIOS_CORE_MEMORY_H
#define CHIQUIOS_CORE_MEMORY_H

#include "types.h"

#define MEMORY_MAX_REGIONS 64

#define MEMORY_TYPE_USABLE       1
#define MEMORY_TYPE_RESERVED     2
#define MEMORY_TYPE_ACPI_RECLAIM 3
#define MEMORY_TYPE_ACPI_NVS     4
#define MEMORY_TYPE_BAD          5

typedef struct {
    u64 base;
    u64 length;
    u32 type;
    u32 attributes;
} __attribute__((packed)) memory_region_t;

typedef struct {
    u32 regions_address;
    u32 count;
} memory_map_t;

chiquios_status_t memory_initialize(const memory_map_t *map);
u64 memory_get_total(void);
u64 memory_get_available(void);
const memory_map_t *memory_get_map(void);

#endif
