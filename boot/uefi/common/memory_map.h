
#ifndef CBL_MEMORY_MAP_H
#define CBL_MEMORY_MAP_H

#include <efi.h>
#include <stdint.h>

typedef struct {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t attributes;
} __attribute__((packed)) cbl_memory_region_t;

typedef struct {
    uint64_t regions_address;
    uint32_t count;
} __attribute__((packed)) cbl_memory_map_t;

typedef struct {
    uint32_t magic;
    uint32_t version;
    cbl_memory_map_t memory;
    uint32_t boot_method;
    uint32_t architecture;
    uint64_t framebuffer_address;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint32_t framebuffer_pitch;
} __attribute__((packed)) cbl_boot_info_t;

EFI_STATUS cbl_get_memory_map(
    EFI_BOOT_SERVICES *bs,
    EFI_MEMORY_DESCRIPTOR **map,
    UINTN *map_size,
    UINTN *map_key,
    UINTN *descriptor_size
);

#endif
