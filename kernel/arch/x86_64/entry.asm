BITS 64

section .text

global _start
extern kernel_main
extern _bss_start
extern _bss_end

_start:
    cli
    cld

    ; Guardar boot_info recibido en RDI.
    mov r12, rdi

    ; Limpiar BSS.
    lea rdi, [_bss_start]
    lea rcx, [_bss_end]
    sub rcx, rdi
    xor eax, eax
    rep stosb

    ; Recuperar el argumento.
    mov rdi, r12

    ; Llamar al kernel.
    call kernel_main

.halt:
    cli
    hlt
    jmp .halt