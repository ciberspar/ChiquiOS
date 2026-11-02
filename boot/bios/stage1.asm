BITS 16
ORG 0x7C00

start:
    jmp 0x0000:boot_start
    nop

boot_start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov [boot_drive], dl

    mov si, message
    call print

    ; Leer Stage 2 desde el sector LBA 1.
    mov byte [retries], 3

read_stage2:
    mov si, disk_packet
    mov dl, [boot_drive]
    mov ah, 0x42
    int 0x13
    jnc stage2_loaded

    ; Reiniciar el dispositivo y reintentar.
    xor ah, ah
    mov dl, [boot_drive]
    int 0x13

    dec byte [retries]
    jnz read_stage2

    mov si, error_message
    call print
    jmp halt

stage2_loaded:
    jmp 0x0000:0x8000

print:
    lodsb
    test al, al
    jz print_done

    mov ah, 0x0E
    mov bx, 0x0007
    int 0x10
    jmp print

print_done:
    ret

halt:
    cli
.hang:
    hlt
    jmp .hang

boot_drive db 0
retries db 0

message db 'CBL Stage 1: BIOS boot', 13, 10, 0
error_message db 'CBL: Stage 2 loading failed', 13, 10, 0

align 4
disk_packet:
    db 0x10
    db 0
    dw 8
    dw 0x8000
    dw 0x0000
    dq 1

times 510 - ($ - $$) db 0
dw 0xAA55