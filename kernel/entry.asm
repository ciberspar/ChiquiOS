
BITS 32

section .text.entry
global _start

extern kernel_main
extern _bss_start
extern _bss_end
extern _stack_top

_start:
    cli
    cld

    ; ESI contiene el puntero boot_info recibido del CBL.
    ; No lo guardamos en BSS porque vamos a limpiarla.

    ; Limpiar BSS.
    mov edi, _bss_start
    mov ecx, _bss_end
    sub ecx, edi
    xor eax, eax
    rep stosb

    ; Usar la pila reservada por el linker.
    mov esp, _stack_top
    and esp, 0xFFFFFFF0

    ; kernel_main(boot_info)
    push esi
    call kernel_main
    add esp, 4

.halt:
    cli
    hlt
    jmp .halt
