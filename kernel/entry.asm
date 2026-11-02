
BITS 32

section .text

global _start

extern kernel_main
extern _bss_start
extern _bss_end


_start:
    cli
    cld

    ; Guardar boot_info en una zona temporal.
    mov [boot_info_saved], esi

    mov esp, 0x90000
    and esp, 0xFFFFFFF0

    ; Limpiar BSS.
    mov edi, _bss_start
    mov ecx, _bss_end
    sub ecx, edi
    xor eax, eax
    rep stosb

    ; Recuperar boot_info.
    mov esi, [boot_info_saved]

    push esi
    call kernel_main
    add esp, 4

.halt:
    cli
    hlt
    jmp .halt

section .bss
align 4
boot_info_saved:
    resd 1
