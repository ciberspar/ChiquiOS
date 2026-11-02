
#include "memory_map.h"

EFI_STATUS cbl_get_memory_map(
    EFI_BOOT_SERVICES *bs,
    EFI_MEMORY_DESCRIPTOR **map,
    UINTN *map_size,
    UINTN *map_key,
    UINTN *descriptor_size
) {
    if (!bs || !map || !map_size ||
        !map_key || !descriptor_size)
        return EFI_INVALID_PARAMETER;

    *map = NULL;
    *map_size = 0;

    EFI_STATUS status = uefi_call_wrapper(
        bs->GetMemoryMap,
        5,
        map_size,
        NULL,
        map_key,
        descriptor_size,
        NULL
    );

    if (status != EFI_BUFFER_TOO_SMALL)
        return status;

    *map_size += 2 * (*descriptor_size);

    status = uefi_call_wrapper(
        bs->AllocatePool,
        3,
        EfiLoaderData,
        *map_size,
        (void **)map
    );

    if (EFI_ERROR(status))
        return status;

    status = uefi_call_wrapper(
        bs->GetMemoryMap,
        5,
        map_size,
        *map,
        map_key,
        descriptor_size,
        NULL
    );

    if (EFI_ERROR(status)) {
        uefi_call_wrapper(bs->FreePool, 1, *map);
        *map = NULL;
    }

    return status;
}
