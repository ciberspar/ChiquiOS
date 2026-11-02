#ifndef CBL_MEMORY_MAP_H
#define CBL_MEMORY_MAP_H

#include <efi.h>
#include "boot_info.h"

EFI_STATUS cbl_get_memory_map(
    EFI_BOOT_SERVICES *bs,
    EFI_MEMORY_DESCRIPTOR **map,
    UINTN *map_size,
    UINTN *map_key,
    UINTN *descriptor_size
);

#endif