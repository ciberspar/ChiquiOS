BITS 16
ORG 0x8000

stage2_start:
    cli

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    sti

    mov si, welcome_message
    call print_string

    mov si, next_message
    call print_string

halt:
    cli
.hang:
    hlt
    jmp .hang

print_string:
    lodsb
    test al, al
    jz .done

    mov ah, 0x0E
    mov bx, 0x0007
    int 0x10

    jmp print_string

.done:
    ret

welcome_message:
    db 13, 10
    db '================================', 13, 10
    db '       CHIQUIOS BOOTLOADER', 13, 10
    db '              CBL', 13, 10
    db '================================', 13, 10, 0

next_message:
    db 'CBL Stage 2 loaded successfully.', 13, 10
    db 'Kernel loading: not implemented yet.', 13, 10, 0