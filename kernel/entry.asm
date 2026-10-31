
BITS 32

section .text

global _start
extern kernel_main
extern _bss_start
extern _bss_end

_start:
    cli

    mov esp, 0x90000
    xor ebp, ebp

    ; Inicializar BSS.
    mov edi, _bss_start
    mov ecx, _bss_end
    sub ecx, edi
    xor eax, eax
    cld
    rep stosb

    call kernel_main

.halt:
    cli
    hlt
    jmp .halt
