
#include "exceptions.h"

#include "../../core/arch.h"
#include "../../include/drivers.h"

static const char *exception_names[32] = {
    "Division Error",
    "Debug",
    "Non-Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment",
    "Invalid TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating Point",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating Point",
    "Virtualization",
    "Control Protection",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Hypervisor Injection",
    "VMM Communication",
    "Security Exception",
    "Reserved"
};

void exception_dispatch(u64 vector, u64 error_code)
{
    arch_disable_interrupts();

    vga_write("\n");
    vga_write("================================\n");
    vga_write("       CHIQUIOS CPU EXCEPTION\n");
    vga_write("================================\n");

    if (vector < 32)
        vga_write(exception_names[vector]);
    else
        vga_write("Unknown exception");

    vga_write("\nError code: ");

    static const char hex[] = "0123456789ABCDEF";

    for (int shift = 60; shift >= 0; shift -= 4) {
        char c[2] = {
            hex[(error_code >> shift) & 0xF],
            '\0'
        };

        vga_write(c);
    }

    vga_write("\nChiquiOS detenido por seguridad.\n");

    for (;;)
        arch_halt();
}
