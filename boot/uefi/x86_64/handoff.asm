
BITS 64

section .text

global cbl_x64_handoff

; RDI = boot_info
; RSI = kernel entry

cbl_x64_handoff:
    cli
    cld

    mov rax, rsi
    mov rdi, rdi

    and rsp, -16
    xor rbp, rbp

    jmp rax

.halt:
    cli
    hlt
    jmp .halt
