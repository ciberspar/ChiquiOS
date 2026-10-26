
; ChiquiOS Bootloader 0.1
; NASM, sector de arranque BIOS x86

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
    sti

    mov [boot_drive], dl

    ; Leer 32 sectores (16 KiB) desde el sector 1.
    ; El kernel se carga en la dirección física 0x10000.
    mov si, disk_packet
    mov ah, 0x42
    mov dl, [boot_drive]
    int 0x13
    jc disk_error

    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp dword 0x08:protected_entry

disk_error:
    mov si, error_message
.print:
    lodsb
    test al, al
    jz .halt
    mov ah, 0x0E
    mov bx, 0x0007
    int 0x10
    jmp .print
.halt:
    cli
    hlt
    jmp .halt

boot_drive db 0
error_message db 'ChiquiOS: disk read error', 0

align 4
disk_packet:
    db 0x10
    db 0
    dw 32
    dw 0x0000
    dw 0x1000
    dq 1

align 8
gdt_start:
    dq 0
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

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

    ; Copiar los 16 KiB del kernel a 1 MiB.
    mov esi, 0x10000
    mov edi, 0x100000
    mov ecx, 4096
    rep movsd

    mov eax, 0x100000
    jmp eax

times 510-($-$$) db 0
dw 0xAA55
