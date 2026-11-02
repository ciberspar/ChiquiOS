
#include <efi.h>
#include <efilib.h>

#ifndef CBL_ARCH_NAME
#define CBL_ARCH_NAME L"Unknown"
#endif

EFI_STATUS EFIAPI efi_main(
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
) {
    (void)ImageHandle;

    if (SystemTable == NULL || SystemTable->ConOut == NULL) {
        return EFI_UNSUPPORTED;
    }

    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);

    SystemTable->ConOut->OutputString(
        SystemTable->ConOut,
        L"================================\r\n"
    );

    SystemTable->ConOut->OutputString(
        SystemTable->ConOut,
        L"       CHIQUIOS BOOTLOADER\r\n"
    );

    SystemTable->ConOut->OutputString(
        SystemTable->ConOut,
        L"              CBL\r\n"
    );

    SystemTable->ConOut->OutputString(
        SystemTable->ConOut,
        L"================================\r\n"
    );

    SystemTable->ConOut->OutputString(
        SystemTable->ConOut,
        L"UEFI firmware detected.\r\n"
    );

    SystemTable->ConOut->OutputString(
        SystemTable->ConOut,
        CBL_ARCH_NAME
    );

    SystemTable->ConOut->OutputString(
        SystemTable->ConOut,
        L"\r\nKernel loading: pending.\r\n"
    );

    return EFI_SUCCESS;
}
