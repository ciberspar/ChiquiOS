
#ifndef CBL_BOOT_INFO_H
#define CBL_BOOT_INFO_H

#include <stdint.h>

#define CBL_BOOT_MAGIC   0x43484951U
#define CBL_BOOT_VERSION 1U

#define CBL_BOOT_UEFI 2U

#define CBL_ARCH_I386   1U
#define CBL_ARCH_X86_64 2U

typedef struct {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t attributes;
} __attribute__((packed)) cbl_memory_region_t;

typedef struct {
    uint64_t regions_address;
    uint32_t count;
} cbl_memory_map_t;

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
} cbl_boot_info_x64_t;

_Static_assert(sizeof(cbl_memory_map_t) == 16,
               "Invalid memory map size");

_Static_assert(__builtin_offsetof(
    cbl_boot_info_x64_t, boot_method) == 24,
    "Invalid boot_method offset");

_Static_assert(__builtin_offsetof(
    cbl_boot_info_x64_t, architecture) == 28,
    "Invalid architecture offset");

#endif
