
BITS 32

section .text

global cbl_ia32_handoff

; EAX = kernel entry
; ESI = boot_info

cbl_ia32_handoff:
    cli
    cld

    mov edx, eax
    mov esp, 0x90000
    and esp, 0xFFFFFFF0

    xor ebp, ebp

    jmp edx

.halt:
    cli
    hlt
    jmp .halt
