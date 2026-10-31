
BITS 32

section .text

global _start
extern kernel_main
extern _bss_start
extern _bss_end

_start:
    cli

    ; ESI contiene la dirección de boot_info.
    mov esi, 0x5000

    mov esp, 0x90000
    xor ebp, ebp

    ; Limpiar BSS sin modificar ESI.
    mov edi, _bss_start
    mov ecx, _bss_end
    sub ecx, edi
    xor eax, eax
    cld
    rep stosb

    ; kernel_main(boot_info)
    push esi
    call kernel_main
    add esp, 4

.halt:
    cli
    hlt
    jmp .halt
