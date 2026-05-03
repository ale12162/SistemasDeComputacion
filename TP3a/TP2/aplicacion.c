#include <efi.h>
#include <efilib.h>

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\n");
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"=====================================\r\n");
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"        Claude's Inters              \r\n");
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"=====================================\r\n");
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\n        >>> grupo: Claude's Inters <<<\r\n\r\n");
    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"Iniciando analisis de seguridad...\r\n");

    unsigned char code[] = {0xCC};
    if (code[0] == 0xCC)
    {
        SystemTable->ConOut->OutputString(SystemTable->ConOut, L"[OK] Breakpoint estatico alcanzado (INT3)\r\n");
    }

    SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\nEjecucion finalizada.\r\n");
    return EFI_SUCCESS;
}
