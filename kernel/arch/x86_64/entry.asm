
BITS 64

section .text

global _start

extern kernel_main
extern _bss_start
extern _bss_end
extern _stack_top

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

    ; Preparar la pila propia.
    lea rsp, [_stack_top]
    and rsp, -16

    ; Recuperar boot_info.
    mov rdi, r12

    ; Entrar al kernel C.
    call kernel_main

.halt:
    cli
    hlt
    jmp .halt
