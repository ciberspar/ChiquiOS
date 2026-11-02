
#include "boot.h"

#define CBL_MAX_MEMORY_REGIONS 256

static uint32_t cbl_convert_type(UINT32 type) {
    switch (type) {
        case EfiLoaderCode:
        case EfiLoaderData:
        case EfiBootServicesCode:
        case EfiBootServicesData:
        case EfiConventionalMemory:
            return 1;

        case EfiUnusableMemory:
            return 2;

        case EfiACPIReclaimMemory:
            return 3;

        case EfiACPIMemoryNVS:
            return 4;

        case EfiMemoryMappedIO:
        case EfiMemoryMappedIOPortSpace:
            return 5;

        default:
            return 0;
    }
}

EFI_STATUS cbl_exit_boot_services(
    EFI_HANDLE image_handle,
    EFI_SYSTEM_TABLE *system_table,
    cbl_boot_info_t **boot_info
) {
    if (!image_handle || !system_table || !boot_info)
        return EFI_INVALID_PARAMETER;

    EFI_BOOT_SERVICES *bs = system_table->BootServices;

    EFI_PHYSICAL_ADDRESS info_address = 0;
    EFI_PHYSICAL_ADDRESS regions_address = 0;

    UINTN pages_info =
        EFI_SIZE_TO_PAGES(sizeof(cbl_boot_info_t));

    UINTN pages_regions =
        EFI_SIZE_TO_PAGES(
            CBL_MAX_MEMORY_REGIONS *
            sizeof(cbl_memory_region_t)
        );

    EFI_STATUS status = uefi_call_wrapper(
        bs->AllocatePages,
        4,
        AllocateAnyPages,
        EfiLoaderData,
        pages_info,
        &info_address
    );

    if (EFI_ERROR(status))
        return status;

    status = uefi_call_wrapper(
        bs->AllocatePages,
        4,
        AllocateAnyPages,
        EfiLoaderData,
        pages_regions,
        &regions_address
    );

    if (EFI_ERROR(status))
        return status;

    cbl_boot_info_t *info =
        (cbl_boot_info_t *)(uintptr_t)info_address;

    cbl_memory_region_t *regions =
        (cbl_memory_region_t *)(uintptr_t)regions_address;

    UINTN map_size = 0;
    UINTN map_key = 0;
    UINTN descriptor_size = 0;
    UINT32 descriptor_version = 0;

    EFI_MEMORY_DESCRIPTOR *map = NULL;

    for (unsigned int attempt = 0; attempt < 4; attempt++) {
        map_size = 0;

        status = uefi_call_wrapper(
            bs->GetMemoryMap,
            5,
            &map_size,
            NULL,
            &map_key,
            &descriptor_size,
            &descriptor_version
        );

        if (status != EFI_BUFFER_TOO_SMALL)
            return status;

        UINTN allocation_size =
            map_size + descriptor_size * 8;

        EFI_PHYSICAL_ADDRESS map_address = 0;

        status = uefi_call_wrapper(
            bs->AllocatePages,
            4,
            AllocateAnyPages,
            EfiLoaderData,
            EFI_SIZE_TO_PAGES(allocation_size),
            &map_address
        );

        if (EFI_ERROR(status))
            return status;

        map = (EFI_MEMORY_DESCRIPTOR *)(uintptr_t)map_address;
        map_size = allocation_size;

        status = uefi_call_wrapper(
            bs->GetMemoryMap,
            5,
            &map_size,
            map,
            &map_key,
            &descriptor_size,
            &descriptor_version
        );

        if (EFI_ERROR(status))
            continue;

        UINTN count = map_size / descriptor_size;

        if (count > CBL_MAX_MEMORY_REGIONS)
            return EFI_OUT_OF_RESOURCES;

        for (UINTN i = 0; i < count; i++) {
            EFI_MEMORY_DESCRIPTOR *d =
                (EFI_MEMORY_DESCRIPTOR *)
                ((UINT8 *)map + i * descriptor_size);

            regions[i].base = d->PhysicalStart;
            regions[i].length =
                d->NumberOfPages * EFI_PAGE_SIZE;
            regions[i].type = cbl_convert_type(d->Type);
            regions[i].attributes = (uint32_t)d->Attribute;
        }

        info->magic = CBL_BOOT_MAGIC;
        info->version = CBL_BOOT_VERSION;
        info->memory.regions_address = regions_address;
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

        status = uefi_call_wrapper(
            bs->ExitBootServices,
            2,
            image_handle,
            map_key
        );

        if (!EFI_ERROR(status)) {
            *boot_info = info;
            return EFI_SUCCESS;
        }

        if (status != EFI_INVALID_PARAMETER)
            return status;
    }

    return EFI_ABORTED;
}
