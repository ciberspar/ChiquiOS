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
    Print(L"Firmware: UEFI\r\n");
    Print(L"Architecture: x86_64\r\n");
    Print(L"\r\n");

    Print(L"ChiquiOS Boot Loader iniciado.\r\n");
    Print(L"Kernel: pendiente de integracion.\r\n");
    Print(L"\r\n");
    Print(L"Presiona una tecla para apagar el sistema.\r\n");

    ST->ConIn->Reset(ST->ConIn, FALSE);

    EFI_INPUT_KEY key;
    while (ST->ConIn->ReadKeyStroke(ST->ConIn, &key) == EFI_NOT_READY) {
    }

    return EFI_SUCCESS;
}