
BITS 32

section .text

global cbl_ia32_handoff

; C calling convention:
; [ESP+4] = boot_info
; [ESP+8] = kernel_entry

cbl_ia32_handoff:
    cli
    cld

    mov esi, [esp + 4]
    mov eax, [esp + 8]

    xor ebp, ebp

    ; El punto de entrada del kernel prepara su propia pila.
    jmp eax

.halt:
    cli
    hlt
    jmp .halt
