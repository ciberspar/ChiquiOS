
#include "uefi_file.h"

EFI_STATUS cbl_read_file(
    EFI_HANDLE image_handle,
    EFI_SYSTEM_TABLE *system_table,
    CHAR16 *path,
    cbl_file_buffer_t *result
) {
    if (!image_handle || !system_table || !path || !result)
        return EFI_INVALID_PARAMETER;

    result->data = NULL;
    result->size = 0;

    EFI_BOOT_SERVICES *bs = system_table->BootServices;

    EFI_LOADED_IMAGE_PROTOCOL *loaded_image = NULL;
    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *filesystem = NULL;
    EFI_FILE_PROTOCOL *root = NULL;
    EFI_FILE_PROTOCOL *file = NULL;
    EFI_FILE_INFO *info = NULL;
    void *buffer = NULL;

    EFI_STATUS status;

    status = uefi_call_wrapper(
        bs->HandleProtocol,
        3,
        image_handle,
        &LoadedImageProtocol,
        (void **)&loaded_image
    );

    if (EFI_ERROR(status))
        return status;

    status = uefi_call_wrapper(
        bs->HandleProtocol,
        3,
        loaded_image->DeviceHandle,
        &FileSystemProtocol,
        (void **)&filesystem
    );

    if (EFI_ERROR(status))
        return status;

    status = uefi_call_wrapper(
        filesystem->OpenVolume,
        2,
        filesystem,
        &root
    );

    if (EFI_ERROR(status))
        return status;

    status = uefi_call_wrapper(
        root->Open,
        5,
        root,
        &file,
        path,
        EFI_FILE_MODE_READ,
        0
    );

    if (EFI_ERROR(status))
        goto cleanup;

    UINTN info_size = 0;

    status = uefi_call_wrapper(
        file->GetInfo,
        4,
        file,
        &gEfiFileInfoGuid,
        &info_size,
        NULL
    );

    if (status != EFI_BUFFER_TOO_SMALL)
        goto cleanup;

    status = uefi_call_wrapper(
        bs->AllocatePool,
        3,
        EfiLoaderData,
        info_size,
        (void **)&info
    );

    if (EFI_ERROR(status))
        goto cleanup;

    status = uefi_call_wrapper(
        file->GetInfo,
        4,
        file,
        &gEfiFileInfoGuid,
        &info_size,
        info
    );

    if (EFI_ERROR(status))
        goto cleanup;

    if (info->FileSize == 0 ||
        info->FileSize > (UINTN)-1) {
        status = EFI_BAD_BUFFER_SIZE;
        goto cleanup;
    }

    status = uefi_call_wrapper(
        bs->AllocatePool,
        3,
        EfiLoaderData,
        (UINTN)info->FileSize,
        &buffer
    );

    if (EFI_ERROR(status))
        goto cleanup;

    UINTN read_size = (UINTN)info->FileSize;

    status = uefi_call_wrapper(
        file->Read,
        3,
        file,
        &read_size,
        buffer
    );

    if (EFI_ERROR(status))
        goto cleanup;

    if (read_size != (UINTN)info->FileSize) {
        status = EFI_DEVICE_ERROR;
        goto cleanup;
    }

    result->data = buffer;
    result->size = read_size;
    buffer = NULL;
    status = EFI_SUCCESS;

cleanup:
    if (buffer)
        uefi_call_wrapper(bs->FreePool, 1, buffer);

    if (info)
        uefi_call_wrapper(bs->FreePool, 1, info);

    if (file)
        uefi_call_wrapper(file->Close, 1, file);

    if (root)
        uefi_call_wrapper(root->Close, 1, root);

    return status;
}

void cbl_free_file(
    EFI_SYSTEM_TABLE *system_table,
    cbl_file_buffer_t *file
) {
    if (!system_table || !file || !file->data)
        return;

    uefi_call_wrapper(
        system_table->BootServices->FreePool,
        1,
        file->data
    );

    file->data = NULL;
    file->size = 0;
}
