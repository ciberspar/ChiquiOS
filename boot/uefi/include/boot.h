#ifndef CBL_UEFI_BOOT_H
#define CBL_UEFI_BOOT_H

#include <efi.h>
#include <efilib.h>

#define CBL_UEFI_VERSION L"CBL 0.1"

EFI_STATUS cbl_initialize(
    EFI_HANDLE image,
    EFI_SYSTEM_TABLE *system_table
);

void cbl_print_header(void);

#endif