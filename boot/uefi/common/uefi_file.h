
#ifndef CBL_UEFI_FILE_H
#define CBL_UEFI_FILE_H

#include <efi.h>
#include <efilib.h>

typedef struct {
    void *data;
    UINTN size;
} cbl_file_buffer_t;

EFI_STATUS cbl_read_file(
    EFI_HANDLE image_handle,
    EFI_SYSTEM_TABLE *system_table,
    CHAR16 *path,
    cbl_file_buffer_t *result
);

void cbl_free_file(
    EFI_SYSTEM_TABLE *system_table,
    cbl_file_buffer_t *file
);

#endif
