
#include "boot.h"
#include <stdint.h>

#define CBL_MAX_MEMORY_REGIONS 1024
#define CBL_MEMORY_MAP_BUFFER_SIZE (128 * 1024)

static uint32_t cbl_convert_type(UINT32 type)
{
    switch (type) {
        case EfiLoaderCode:
        case EfiLoaderData:
        case EfiBootServicesCode:
        case EfiBootServicesData:
        case EfiConventionalMemory:
        case EfiPersistentMemory:
            return 1;

        case EfiUnusableMemory:
            return 5;

        case EfiACPIReclaimMemory:
            return 3;

        case EfiACPIMemoryNVS:
            return 4;

        default:
            return 2;
    }
}

EFI_STATUS cbl_exit_boot_services(
    EFI_HANDLE image_handle,
    EFI_SYSTEM_TABLE *system_table,
    cbl_boot_info_t **boot_info
)
{
    if (!image_handle || !system_table || !boot_info)
        return EFI_INVALID_PARAMETER;

    EFI_BOOT_SERVICES *bs = system_table->BootServices;

    cbl_boot_info_t *info = NULL;
    cbl_memory_region_t *regions = NULL;
    EFI_MEMORY_DESCRIPTOR *map = NULL;

    UINTN map_capacity = CBL_MEMORY_MAP_BUFFER_SIZE;
    UINTN regions_capacity =
        CBL_MAX_MEMORY_REGIONS * sizeof(cbl_memory_region_t);

    EFI_STATUS status;

    status = uefi_call_wrapper(
        bs->AllocatePool, 3,
        EfiLoaderData,
        sizeof(cbl_boot_info_t),
        (void **)&info
    );

    if (EFI_ERROR(status))
        return status;

    status = uefi_call_wrapper(
        bs->AllocatePool, 3,
        EfiLoaderData,
        regions_capacity,
        (void **)&regions
    );

    if (EFI_ERROR(status))
        goto fail;

    status = uefi_call_wrapper(
        bs->AllocatePool, 3,
        EfiLoaderData,
        map_capacity,
        (void **)&map
    );

    if (EFI_ERROR(status))
        goto fail;

    UINTN map_size = map_capacity;
    UINTN map_key = 0;
    UINTN descriptor_size = 0;
    UINT32 descriptor_version = 0;

    for (UINTN attempt = 0; attempt < 8; attempt++) {

        map_size = map_capacity;

        status = uefi_call_wrapper(
            bs->GetMemoryMap, 5,
            &map_size,
            map,
            &map_key,
            &descriptor_size,
            &descriptor_version
        );

        if (status == EFI_BUFFER_TOO_SMALL) {
            status = EFI_OUT_OF_RESOURCES;
            goto fail;
        }

        if (EFI_ERROR(status))
            goto fail;

        if (descriptor_size < sizeof(EFI_MEMORY_DESCRIPTOR) ||
            descriptor_size == 0 ||
            map_size % descriptor_size != 0) {
            status = EFI_COMPROMISED_DATA;
            goto fail;
        }

        UINTN count = map_size / descriptor_size;

        if (count > CBL_MAX_MEMORY_REGIONS) {
            status = EFI_OUT_OF_RESOURCES;
            goto fail;
        }

        for (UINTN i = 0; i < count; i++) {
            EFI_MEMORY_DESCRIPTOR *d =
                (EFI_MEMORY_DESCRIPTOR *)(
                    (UINT8 *)map + i * descriptor_size
                );

            regions[i].base = d->PhysicalStart;
            regions[i].length =
                d->NumberOfPages * EFI_PAGE_SIZE;
            regions[i].type = cbl_convert_type(d->Type);
            regions[i].attributes = (uint32_t)d->Attribute;
        }

        info->magic = CBL_BOOT_MAGIC;
        info->version = CBL_BOOT_VERSION;

        info->memory.regions_address =
            (uintptr_t)regions;
        info->memory.count = (uint32_t)count;

        info->boot_method = CBL_BOOT_UEFI;

#if defined(__x86_64__)
        info->architecture = CBL_ARCH_X86_64;
#else
        info->architecture = CBL_ARCH_I386;
#endif

        info->framebuffer_address = 0;
        info->framebuffer_width = 0;
        info->framebuffer_height = 0;
        info->framebuffer_pitch = 0;

        /*
         * No se deben realizar asignaciones ni llamadas
         * innecesarias entre GetMemoryMap y ExitBootServices.
         */
        status = uefi_call_wrapper(
            bs->ExitBootServices, 2,
            image_handle,
            map_key
        );

        if (!EFI_ERROR(status)) {
            *boot_info = info;
            return EFI_SUCCESS;
        }

        if (status != EFI_INVALID_PARAMETER)
            goto fail;
    }

    status = EFI_ABORTED;

fail:
    if (map)
        uefi_call_wrapper(bs->FreePool, 1, map);

    if (regions)
        uefi_call_wrapper(bs->FreePool, 1, regions);

    if (info)
        uefi_call_wrapper(bs->FreePool, 1, info);

    return status;
}
