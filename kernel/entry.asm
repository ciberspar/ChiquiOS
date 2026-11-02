
BITS 32

section .text

global _start

extern kernel_main
extern _bss_start
extern _bss_end

_start:
    cli
    cld

    ; ESI debe contener la dirección de boot_info.
    mov ebp, esi

    ; Preparar la pila del kernel.
    mov esp, 0x90000
    and esp, 0xFFFFFFF0
    xor ebp, ebp

    ; Limpiar BSS.
    mov edi, _bss_start
    mov ecx, _bss_end
    sub ecx, edi
    xor eax, eax
    rep stosb

    ; kernel_main(boot_info)
    push esi
    call kernel_main
    add esp, 4

.halt:
    cli
    hlt
    jmp .halt
