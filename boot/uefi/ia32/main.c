#include <efi.h>
#include <efilib.h>

EFI_STATUS EFIAPI efi_main(
    EFI_HANDLE image,
    EFI_SYSTEM_TABLE *system_table
) {
    InitializeLib(image, system_table);

    ST->ConOut->ClearScreen(ST->ConOut);

    Print(L"\r\n");
    Print(L"====================================\r\n");
    Print(L"          CHIQUIOS BOOT\r\n");
    Print(L"====================================\r\n");
    Print(L"CBL version: 0.1\r\n");
    Print(L"Firmware: UEFI IA32\r\n");
    Print(L"Architecture: i386\r\n");
    Print(L"\r\n");

    Print(L"ChiquiOS Boot Loader iniciado.\r\n");
    Print(L"Kernel: pendiente de integracion.\r\n");

    return EFI_SUCCESS;
}