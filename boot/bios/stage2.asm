
BITS 16
ORG 0x8000

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov si, banner
    call print_string

    mov si, waiting
    call print_string

halt:
    cli
    hlt
    jmp halt

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

banner:
    db "ChiquiOS Bootloader", 13, 10, 0

waiting:
    db "BIOS Stage 2 loaded.", 13, 10
    db "Kernel loading is the next step.", 13, 10, 0

times 4096-($-$$) db 0
