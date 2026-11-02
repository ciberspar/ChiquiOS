
BITS 64

section .text

global cbl_x64_handoff

cbl_x64_handoff:
    cli
    cld

    ; RDI = boot_info
    ; RSI = kernel entry

    mov rax, rsi
    and rsp, -16
    xor rbp, rbp

    ; kernel_main recibirá boot_info a través de _start.
    jmp rax

.halt:
    cli
    hlt
    jmp .halt
