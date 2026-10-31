BITS 16
ORG 0x8000

%ifndef KERNEL_BYTES
    %error "Falta KERNEL_BYTES"
%endif

%ifndef KERNEL_SECTORS
    %error "Falta KERNEL_SECTORS"
%endif

%if KERNEL_SECTORS < 1 || KERNEL_SECTORS > 1024
    %error "Kernel fuera del limite inicial de 512 KiB"
%endif

STAGE2_SECTORS equ 32
KERNEL_LBA equ 1 + STAGE2_SECTORS

KERNEL_LOAD_SEGMENT equ 0x1000
KERNEL_LOAD_OFFSET  equ 0x0000

PML4_ADDRESS equ 0x70000
PDPT_ADDRESS equ 0x71000
PD_ADDRESS   equ 0x72000

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7A00
    cld
    sti

    mov [boot_drive], dl

    call collect_memory
    jc memory_error

    call load_kernel
    jc disk_error

    ; Comprobar soporte de modo largo.
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb cpu_error

    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz cpu_error

    cli
    lgdt [gdt_descriptor]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp dword 0x08:protected_entry

; --------------------------------------------------
; Memoria BIOS E820
; --------------------------------------------------

collect_memory:
    xor ax, ax
    mov es, ax
    mov di, 0x6000

    xor ebx, ebx
    xor bp, bp

.e820_loop:
    cmp bp, 64
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
    inc bp

.e820_next:
    test ebx, ebx
    jnz .e820_loop

.e820_done:
    test bp, bp
    jz .e820_error

    ; boot_info x86-64.
    mov dword [0x5000], 0x43484951
    mov dword [0x5004], 1

    mov dword [0x5008], 0x6000
    mov dword [0x500C], 0

    movzx eax, bp
    mov dword [0x5010], eax

    mov dword [0x5018], 1
    mov dword [0x501C], 2

    mov dword [0x5020], 0
    mov dword [0x5024], 0
    mov dword [0x5028], 0
    mov dword [0x502C], 0
    mov dword [0x5030], 0

    clc
    ret

.e820_error:
    stc
    ret

; --------------------------------------------------
; Carga del kernel desde disco
; --------------------------------------------------

load_kernel:
    mov word [remaining], KERNEL_SECTORS
    mov word [destination_segment], KERNEL_LOAD_SEGMENT
    mov dword [current_lba], KERNEL_LBA

.read_loop:
    cmp word [remaining], 0
    je .done

    mov ax, [remaining]
    cmp ax, 32
    jbe .read_chunk
    mov ax, 32

.read_chunk:
    mov [dap_count], ax
    mov word [dap_offset], 0

    mov ax, [destination_segment]
    mov [dap_segment], ax

    mov eax, [current_lba]
    mov [dap_lba_low], eax
    mov dword [dap_lba_high], 0

    mov si, disk_packet
    mov dl, [boot_drive]
    mov ah, 0x42
    int 0x13
    jc .error

    mov ax, [dap_count]
    sub [remaining], ax

    mov bx, ax
    shl bx, 5
    add [destination_segment], bx

    movzx eax, ax
    add [current_lba], eax

    jmp .read_loop

.done:
    clc
    ret

.error:
    stc
    ret

; --------------------------------------------------
; Errores
; --------------------------------------------------

memory_error:
    mov si, msg_memory
    jmp print_error

disk_error:
    mov si, msg_disk
    jmp print_error

cpu_error:
    mov si, msg_cpu

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
; Datos
; --------------------------------------------------

boot_drive db 0
remaining dw 0
destination_segment dw 0
current_lba dd 0

msg_memory db 'ChiquiOS x64: error E820', 0
msg_disk db 'ChiquiOS x64: error leyendo kernel', 0
msg_cpu db 'ChiquiOS x64: CPU incompatible', 0

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
; Modo protegido de 32 bits
; --------------------------------------------------

BITS 32

protected_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x9F000
    cld

    ; Copiar kernel a 1 MiB.
    mov esi, 0x10000
    mov edi, 0x100000
    mov ecx, (KERNEL_BYTES + 3) / 4
    rep movsd

    ; Limpiar las tablas de páginas.
    mov edi, PML4_ADDRESS
    xor eax, eax
    mov ecx, 3072
    rep stosd

    ; PML4[0] -> PDPT.
    mov dword [PML4_ADDRESS], PDPT_ADDRESS | 3

    ; PDPT[0] -> PD.
    mov dword [PDPT_ADDRESS], PD_ADDRESS | 3

    ; Mapear los primeros 1 GiB con páginas de 2 MiB.
    mov edi, PD_ADDRESS
    mov eax, 0x83
    mov ecx, 512

.map_pages:
    mov [edi], eax
    mov dword [edi + 4], 0
    add eax, 0x200000
    add edi, 8
    loop .map_pages

    ; Activar PAE.
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; Dirección de PML4.
    mov eax, PML4_ADDRESS
    mov cr3, eax

    ; Activar Long Mode.
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; Activar paginación.
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    jmp dword 0x18:long_mode_entry

; --------------------------------------------------
; Modo largo de 64 bits
; --------------------------------------------------

BITS 64

long_mode_entry:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax

    mov rsp, 0x9F000
    and rsp, -16

    ; boot_info x86-64.
    mov rdi, 0x5000

    ; Kernel cargado en 1 MiB.
    mov rax, 0x100000
    jmp rax

times 16384-($-$$) db 0