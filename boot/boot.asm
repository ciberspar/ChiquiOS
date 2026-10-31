
BITS 16
ORG 0x7C00

%ifndef KERNEL_BYTES
    %error "Falta definir KERNEL_BYTES"
%endif

%ifndef KERNEL_SECTORS
    %error "Falta definir KERNEL_SECTORS"
%endif

%if KERNEL_SECTORS < 1 || KERNEL_SECTORS > 1024
    %error "El kernel supera la capacidad del cargador BIOS actual"
%endif

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

    mov word [remaining], KERNEL_SECTORS
    mov word [destination_segment], 0x1000
    mov word [dap_lba], 1

read_kernel:
    cmp word [remaining], 0
    je kernel_loaded

    mov ax, [remaining]
    cmp ax, 127
    jbe read_chunk
    mov ax, 127

read_chunk:
    mov [dap_count], ax

    mov ax, [destination_segment]
    mov [dap_segment], ax

    mov si, disk_packet
    mov dl, [boot_drive]
    mov ah, 0x42
    int 0x13
    jc disk_error

    mov ax, [dap_count]
    sub [remaining], ax

    mov bx, ax
    shl bx, 5
    add [destination_segment], bx

    add [dap_lba], ax

    jmp read_kernel

kernel_loaded:
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
remaining dw 0
destination_segment dw 0

error_message db 'ChiquiOS: error leyendo el kernel', 0

align 4

disk_packet:
    db 0x10
    db 0
dap_count:
    dw 1
    dw 0
dap_segment:
    dw 0x1000
dap_lba:
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

    mov esi, 0x10000
    mov edi, 0x100000
    mov ecx, (KERNEL_BYTES + 3) / 4
    rep movsd

    mov eax, 0x100000
    jmp eax

times 510-($-$$) db 0
dw 0xAA55
