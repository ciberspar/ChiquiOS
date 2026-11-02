
#ifndef CBL_BOOT_H
#define CBL_BOOT_H

#include <efi.h>
#include "memory_map.h"
#include "elf_loader.h"

EFI_STATUS cbl_exit_boot_services(
    EFI_HANDLE image_handle,
    EFI_SYSTEM_TABLE *system_table,
    cbl_boot_info_t **boot_info
);

#endif
