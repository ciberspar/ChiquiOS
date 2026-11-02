#ifndef CBL_BOOT_INFO_H
#define CBL_BOOT_INFO_H

#include <stdint.h>
#include <stddef.h>

#define CBL_BOOT_MAGIC   0x43484951U
#define CBL_BOOT_VERSION 1U

#define CBL_BOOT_BIOS 1U
#define CBL_BOOT_UEFI 2U

#define CBL_ARCH_I386   1U
#define CBL_ARCH_X86_64 2U

typedef struct {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t attributes;
} __attribute__((packed)) cbl_memory_region_t;

/*
 * El campo regions_address debe tener el tamaño
 * de un puntero de la arquitectura del bootloader.
 */
typedef struct {
    uintptr_t regions_address;
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
} cbl_boot_info_t;

#if UINTPTR_MAX == 0xFFFFFFFFU

_Static_assert(sizeof(cbl_memory_map_t) == 8,
               "IA32: memory map size incorrect");

_Static_assert(sizeof(cbl_boot_info_t) == 44,
               "IA32: boot info size incorrect");

_Static_assert(offsetof(cbl_boot_info_t, boot_method) == 16,
               "IA32: boot_method offset incorrect");

_Static_assert(offsetof(cbl_boot_info_t, architecture) == 20,
               "IA32: architecture offset incorrect");

#elif UINTPTR_MAX == 0xFFFFFFFFFFFFFFFFULL

_Static_assert(sizeof(cbl_memory_map_t) == 16,
               "x86_64: memory map size incorrect");

_Static_assert(sizeof(cbl_boot_info_t) == 56,
               "x86_64: boot info size incorrect");

_Static_assert(offsetof(cbl_boot_info_t, boot_method) == 24,
               "x86_64: boot_method offset incorrect");

_Static_assert(offsetof(cbl_boot_info_t, architecture) == 28,
               "x86_64: architecture offset incorrect");

#else
#error "Unsupported pointer size"
#endif

_Static_assert(sizeof(cbl_memory_region_t) == 24,
               "Memory region size incorrect");

#endif