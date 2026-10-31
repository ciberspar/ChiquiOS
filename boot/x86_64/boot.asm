BITS 16
ORG 0x7C00

STAGE2_SECTORS equ 32
STAGE2_SEGMENT equ 0x0800
STAGE2_OFFSET  equ 0x0000

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

    ; Comprobar extensiones de disco BIOS.
    mov ah, 0x41
    mov bx, 0x55AA
    int 0x13
    jc disk_error

    cmp bx, 0xAA55
    jne disk_error

    test cx, 1
    jz disk_error

    ; Cargar segunda etapa desde LBA 1.
    mov word [dap_count], STAGE2_SECTORS
    mov word [dap_offset], STAGE2_OFFSET
    mov word [dap_segment], STAGE2_SEGMENT
    mov dword [dap_lba_low], 1
    mov dword [dap_lba_high], 0

    mov si, disk_packet
    mov dl, [boot_drive]
    mov ah, 0x42
    int 0x13
    jc disk_error

    mov dl, [boot_drive]
    jmp STAGE2_SEGMENT:STAGE2_OFFSET

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
error_message db 'ChiquiOS x64: error de disco', 0

align 4

disk_packet:
    db 0x10
    db 0
dap_count:
    dw 1
dap_offset:
    dw 0
dap_segment:
    dw 0
dap_lba_low:
    dd 0
dap_lba_high:
    dd 0

times 510-($-$$) db 0
dw 0xAA55