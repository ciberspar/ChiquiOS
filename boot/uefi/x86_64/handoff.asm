
BITS 64

section .text

global cbl_x64_handoff

; System V AMD64 calling convention:
; RDI = boot_info
; RSI = kernel_entry

cbl_x64_handoff:
    cli
    cld

    mov rax, rsi
    xor rbp, rbp

    ; RDI conserva el puntero boot_info.
    ; El kernel prepara su propia pila.
    jmp rax

.halt:
    cli
    hlt
    jmp .halt
