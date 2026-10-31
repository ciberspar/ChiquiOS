
#include "exceptions.h"
#include "idt.h"
#include "../../include/drivers.h"
#include "../../core/arch.h"

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
    "Security",
    "Reserved"
};

void exception_handler(u64 vector, u64 error_code)
{
    arch_disable_interrupts();

    vga_write("\n");
    vga_write("================================\n");
    vga_write("       CHIQUIOS CPU EXCEPTION\n");
    vga_write("================================\n");

    if (vector < 32) {
        vga_write(exception_names[vector]);
    } else {
        vga_write("Unknown exception");
    }

    vga_write("\nKernel halted for safety.\n");

    (void)error_code;

    for (;;) {
        arch_halt();
    }
}

void exceptions_initialize(void)
{
    /*
     * Los handlers individuales se conectarán
     * cuando esté lista la tabla de stubs ASM.
     */
}
