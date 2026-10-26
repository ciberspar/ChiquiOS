
; ChiquiOS 0.4 - Bootloader BIOS para CD/DVD
; El BIOS carga el bootloader y los 32 sectores del kernel.

BITS 16
ORG 0x7C00

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    cld

    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp dword 0x08:protected_entry

BITS 32
protected_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x90000
    cld

    ; El BIOS carga el kernel justo después del sector de arranque.
    mov esi, 0x7E00
    mov edi, 0x100000
    mov ecx, 4096
    rep movsd

    mov eax, 0x100000
    jmp eax

align 8
gdt_start:
    dq 0
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

times 510-($-$$) db 0
dw 0xAA55
