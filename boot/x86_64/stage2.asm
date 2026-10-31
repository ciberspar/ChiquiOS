
BITS 16
ORG 0x8000

%ifndef KERNEL_BYTES
    %error "Falta definir KERNEL_BYTES"
%endif

%ifndef KERNEL_SECTORS
    %error "Falta definir KERNEL_SECTORS"
%endif

%if KERNEL_SECTORS < 1 || KERNEL_SECTORS > 1024
    %error "Kernel demasiado grande para este cargador"
%endif

%if KERNEL_BYTES < 1 || KERNEL_BYTES > 524288
    %error "Tamano de kernel no admitido"
%endif

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    cld

    mov [boot_drive], dl

    ; Obtener mapa de memoria BIOS E820.
    call collect_memory
    jc memory_error

    ; Leer kernel desde LBA 33.
    mov word [remaining], KERNEL_SECTORS
    mov word [destination_segment], 0x1000
    mov dword [current_lba], 33

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

    ; Avanzar destino: 32 parrafos por sector.
    shl ax, 5
    add [destination_segment], ax

    ; Avanzar LBA.
    movzx eax, word [dap_count]
    add [current_lba], eax

    jmp read_kernel

kernel_loaded:
    ; Preparar estructuras de paginacion.
    call setup_paging

    ; Entrar en modo protegido de 32 bits.
    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp dword 0x08:protected_entry

; --------------------------------------------------
; BIOS E820
; Mapa de memoria en 0x6000
; --------------------------------------------------

collect_memory:
    xor ax, ax
    mov es, ax
    mov di, 0x6000
    xor ebx, ebx
    mov word [e820_count], 0

.e820_loop:
    cmp word [e820_count], 64
    jae .e820_done

    mov eax, 0xE820
    mov edx, 0x534D4150
    mov ecx, 24
    mov dword [es:di + 20], 1

    int 0x15
    jc .e820_done

    cmp eax, 0x534D4150
    jne .e820_error

    cmp ecx, 20
    jb .e820_next

    add di, 24
    inc word [e820_count]

.e820_next:
    test ebx, ebx
    jnz .e820_loop

.e820_done:
    cmp word [e820_count], 0
    je .e820_error

    ; chiquios_boot_info_t para x86-64.
    mov dword [0x5000], 0x43484951
    mov dword [0x5004], 1

    ; memory.regions_address (64 bits).
    mov dword [0x5008], 0x6000
    mov dword [0x500C], 0

    ; memory.count.
    movzx eax, word [e820_count]
    mov dword [0x5010], eax

    ; Padding de estructura.
    mov dword [0x5014], 0

    ; boot_method = BIOS.
    mov dword [0x5018], 1

    ; architecture = x86_64.
    mov dword [0x501C], 2

    ; Framebuffer no disponible todavía.
    mov dword [0x5020], 0
    mov dword [0x5024], 0
    mov dword [0x5028], 0
    mov dword [0x502C], 0

    clc
    ret

.e820_error:
    stc
    ret

; --------------------------------------------------
; Tablas de paginas: identidad de los primeros 1 GiB
; --------------------------------------------------

setup_paging:
    ; Limpiar tres paginas.
    mov edi, 0x70000
    xor eax, eax
    mov ecx, 3072
    rep stosd

    ; PML4[0] -> PDPT.
    mov dword [0x70000], 0x71003

    ; PDPT[0] -> PD.
    mov dword [0x71000], 0x72003

    ; 512 entradas de 2 MiB = 1 GiB.
    mov edi, 0x72000
    mov eax, 0x00000083
    mov ecx, 512

.fill_pd:
    mov [edi], eax
    mov dword [edi + 4], 0
    add eax, 0x200000
    add edi, 8
    loop .fill_pd

    ret

; --------------------------------------------------
; Errores BIOS
; --------------------------------------------------

memory_error:
    mov si, memory_message
    jmp print_error

disk_error:
    mov si, disk_message

print_error:
    lodsb
    test al, al
    jz halt_error

    mov ah, 0x0E
    mov bx, 0x0007
    int 0x10
    jmp print_error

halt_error:
    cli
    hlt
    jmp halt_error

; --------------------------------------------------
; Variables
; --------------------------------------------------

boot_drive db 0
remaining dw 0
destination_segment dw 0
e820_count dw 0
current_lba dd 0

memory_message db 'ChiquiOS: E820 error', 0
disk_message db 'ChiquiOS: kernel read error', 0

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
    dq 0

; --------------------------------------------------
; GDT
; --------------------------------------------------

align 8

gdt_start:
    dq 0
    dq 0x00CF9A000000FFFF
    dq 0x00CF92000000FFFF
    dq 0x00AF9A000000FFFF
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; --------------------------------------------------
; Modo protegido 32 bits
; --------------------------------------------------

BITS 32

protected_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Copiar kernel a su direccion de enlace: 1 MiB.
    mov esi, 0x10000
    mov edi, 0x100000
    mov ecx, (KERNEL_BYTES + 3) / 4
    rep movsd

    ; Activar PAE.
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Directorio PML4.
    mov eax, 0x70000
    mov cr3, eax

    ; Activar Long Mode en EFER.
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Activar paginacion y proteccion.
    mov eax, cr0
    or eax, 0x80000001
    mov cr0, eax

    ; Entrar en modo de 64 bits.
    jmp 0x18:long_mode_entry

; --------------------------------------------------
; Modo largo 64 bits
; --------------------------------------------------

BITS 64

long_mode_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax

    xor ax, ax
    mov fs, ax
    mov gs, ax

    ; Pila inicial del kernel.
    mov rsp, 0x90000
    and rsp, -16

    ; Argumento 1 de kernel_main:
    ; const chiquios_boot_info_t *boot_info
    mov rdi, 0x5000

    ; Kernel enlazado en 0x100000.
    mov rax, 0x100000
    jmp rax

halt64:
    cli
    hlt
    jmp halt64

; --------------------------------------------------
; Reservar exactamente 16 KiB para stage2.
; --------------------------------------------------

times 16384-($-$$) db 0
