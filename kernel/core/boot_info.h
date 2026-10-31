
#ifndef CHIQUIOS_CORE_BOOT_INFO_H
#define CHIQUIOS_CORE_BOOT_INFO_H

#include "types.h"
#include "memory.h"

#define CHIQUIOS_BOOT_MAGIC 0x43484951
#define CHIQUIOS_BOOT_VERSION 1

#define CHIQUIOS_BOOT_BIOS 1
#define CHIQUIOS_BOOT_UEFI 2

#define CHIQUIOS_ARCH_I386   1
#define CHIQUIOS_ARCH_X86_64 2

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

#endif
