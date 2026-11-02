
#include <efi.h>
#include <efilib.h>

#include "uefi_file.h"
#include "elf_loader.h"
#include "../../common/elf_format.h"

typedef struct {
    EFI_BOOT_SERVICES *boot_services;
} cbl_efi_context_t;

static int cbl_allocate_at(
    void *context,
    uint64_t physical_address,
    uint64_t page_count
) {
    cbl_efi_context_t *efi =
        (cbl_efi_context_t *)context;

    if (!efi || !efi->boot_services || page_count == 0)
        return 0;

    if (page_count > (uint64_t)(UINTN)-1)
        return 0;

#if defined(__i386__)
    if (physical_address > 0xFFFFFFFFULL)
        return 0;
#endif

    EFI_PHYSICAL_ADDRESS address =
        (EFI_PHYSICAL_ADDRESS)physical_address;

    EFI_STATUS status = uefi_call_wrapper(
        efi->boot_services->AllocatePages,
        4,
        AllocateAddress,
        EfiLoaderData,
        (UINTN)page_count,
        &address
    );

    if (EFI_ERROR(status))
        return 0;

    return address == physical_address;
}

EFI_STATUS EFIAPI efi_main(
    EFI_HANDLE image_handle,
    EFI_SYSTEM_TABLE *system_table
) {
    InitializeLib(image_handle, system_table);

    Print(L"\r\n");
    Print(L"ChiquiOS Bootloader (CBL)\r\n");
    Print(L"Loading kernel...\r\n");

    cbl_file_buffer_t kernel_file = { 0 };
    cbl_elf_result_t kernel_result = { 0 };

    cbl_efi_context_t context = {
        .boot_services = system_table->BootServices
    };

    cbl_elf_memory_t memory = {
        .context = &context,
        .allocate_pages_at = cbl_allocate_at
    };

#if defined(__x86_64__)
    CHAR16 *kernel_path = L"\\ChiquiOS\\kernel-x64.elf";
    unsigned int elf_class = CBL_ELFCLASS64;
    unsigned int elf_machine = CBL_EM_X86_64;
    Print(L"Architecture: x86_64\r\n");
#elif defined(__i386__)
    CHAR16 *kernel_path = L"\\ChiquiOS\\kernel-x32.elf";
    unsigned int elf_class = CBL_ELFCLASS32;
    unsigned int elf_machine = CBL_EM_386;
    Print(L"Architecture: IA32\r\n");
#else
    Print(L"Unsupported UEFI architecture.\r\n");
    return EFI_UNSUPPORTED;
#endif

    EFI_STATUS status = cbl_read_file(
        image_handle,
        system_table,
        kernel_path,
        &kernel_file
    );

    if (EFI_ERROR(status)) {
        Print(L"ERROR: Cannot open kernel ELF.\r\n");
        return status;
    }

    Print(L"Kernel file read successfully.\r\n");

    int loaded = cbl_elf_load(
        kernel_file.data,
        kernel_file.size,
        elf_class,
        elf_machine,
        &memory,
        &kernel_result
    );

    cbl_free_file(system_table, &kernel_file);

    if (!loaded) {
        Print(L"ERROR: ELF loading failed.\r\n");
        return EFI_LOAD_ERROR;
    }

    Print(L"Kernel segments loaded: %u\r\n",
          kernel_result.loaded_segments);

    Print(L"Kernel entry: 0x%lx\r\n",
          kernel_result.entry);

    Print(L"ELF loaded. Kernel handoff is not implemented yet.\r\n");

    return EFI_SUCCESS;
}
