
BITS 16
ORG 0x7C00

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov [boot_drive], dl

    mov si, message
    call print_string

    mov dl, [boot_drive]
    mov si, disk_packet
    mov ah, 0x42
    int 0x13
    jc disk_error

    jmp 0x0000:0x8000

disk_error:
    mov si, error_message
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

boot_drive:
    db 0

message:
    db "CBL BIOS Stage 1", 13, 10, 0

error_message:
    db "Disk read error!", 13, 10, 0

align 4

disk_packet:
    db 0x10
    db 0
    dw 8
    dw 0x8000
    dw 0
    dq 1

times 510-($-$$) db 0
dw 0xAA55
