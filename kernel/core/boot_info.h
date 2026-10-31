
#ifndef CHIQUIOS_CORE_BOOT_INFO_H
#define CHIQUIOS_CORE_BOOT_INFO_H

#include "types.h"
#include "memory.h"
#include "arch_config.h"

#define CHIQUIOS_BOOT_MAGIC   0x43484951U
#define CHIQUIOS_BOOT_VERSION 1U

#define CHIQUIOS_BOOT_BIOS 1U
#define CHIQUIOS_BOOT_UEFI 2U

#define CHIQUIOS_ARCH_I386   1U
#define CHIQUIOS_ARCH_X86_64 2U

typedef struct {
    u32 magic;
    u32 version;

    memory_map_t memory;

    u32 boot_method;
    u32 architecture;

    u64 framebuffer_address;
    u32 framebuffer_width;
    u32 framebuffer_height;
    u32 framebuffer_pitch;
} chiquios_boot_info_t;

#if CHIQUIOS_BUILD_I386

typedef char boot_memory_map_size_check[
    (sizeof(memory_map_t) == 8) ? 1 : -1
];

typedef char boot_info_size_check[
    (sizeof(chiquios_boot_info_t) == 44) ? 1 : -1
];

typedef char boot_info_method_offset_check[
    (__builtin_offsetof(chiquios_boot_info_t, boot_method) == 16)
    ? 1 : -1
];

typedef char boot_info_arch_offset_check[
    (__builtin_offsetof(chiquios_boot_info_t, architecture) == 20)
    ? 1 : -1
];

#elif CHIQUIOS_BUILD_X86_64

typedef char boot_memory_map_size_check[
    (sizeof(memory_map_t) == 16) ? 1 : -1
];

typedef char boot_info_arch_offset_check[
    (__builtin_offsetof(chiquios_boot_info_t, architecture) == 24)
    ? 1 : -1
];

#endif

#endif
