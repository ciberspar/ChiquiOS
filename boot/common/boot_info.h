
#ifndef CBL_COMMON_BOOT_INFO_H
#define CBL_COMMON_BOOT_INFO_H

#include <stdint.h>

#define CHIQUIOS_BOOT_MAGIC   0x43484951U
#define CHIQUIOS_BOOT_VERSION 1U

#define CHIQUIOS_BOOT_BIOS 1U
#define CHIQUIOS_BOOT_UEFI 2U

#define CHIQUIOS_ARCH_I386   1U
#define CHIQUIOS_ARCH_X86_64 2U

typedef struct {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t attributes;
} __attribute__((packed)) cbl_memory_region_t;

#if defined(__x86_64__)

typedef struct {
    uint64_t regions_address;
    uint32_t count;
} cbl_memory_map_t;

#else

typedef struct {
    uint32_t regions_address;
    uint32_t count;
} cbl_memory_map_t;

#endif

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

#endif
