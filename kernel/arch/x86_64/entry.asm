BITS 64

section .text
global _start
extern kernel_main
extern _bss_start
extern _bss_end

_start:
    cli
    cld

    ; El cargador debe entregar:
    ; RDI = dirección de boot_info
    ; RSP = pila válida de 16 bytes de alineación

    xor rbp, rbp

    ; Limpiar BSS
    lea rdi, [_bss_start]
    lea rcx, [_bss_end]
    sub rcx, rdi
    xor eax, eax
    rep stosb

    ; Recuperar boot_info y llamar al kernel
    ; RDI conserva el primer argumento de la ABI x86-64.
    call kernel_main

.halt:
    cli
    hlt
    jmp .halt