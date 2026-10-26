
BITS 32

section .text
global _start
extern kernel_main

_start:
    cli
    mov esp, 0x90000
    xor ebp, ebp

    call kernel_main

.halt:
    cli
    hlt
    jmp .halt
