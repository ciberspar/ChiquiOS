BITS 16
ORG 0x7C00

%define STAGE2_SEGMENT 0x0800
%define STAGE2_SECTORS 32

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov [boot_drive], dl

    ; Comprobar extensiones LBA del BIOS.
    mov ah, 0x41
    mov bx, 0x55AA
    int 0x13
    jc disk_error

    cmp bx, 0xAA55
    jne disk_error

    test cx, 1
    jz disk_error

    ; Cargar stage2 desde el sector LBA 1.
    mov si, disk_packet
    mov ah, 0x42
    mov dl, [boot_drive]
    int 0x13
    jc disk_error

    jmp STAGE2_SEGMENT:0x0000

disk_error:
    mov si, error_message

.print:
    lodsb
    test al, al
    jz .halt
    mov ah, 0x0E
    int 0x10
    jmp .print

.halt:
    cli
    hlt
    jmp .halt

boot_drive db 0

disk_packet:
    db 0x10
    db 0
    dw STAGE2_SECTORS
    dw 0x0000
    dw STAGE2_SEGMENT
    dq 1

error_message db 'ChiquiOS: BIOS disk error', 0

times 510-($-$$) db 0
dw 0xAA55