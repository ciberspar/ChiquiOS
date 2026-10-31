#ifndef CHIQUIOS_CORE_ARCH_H
#define CHIQUIOS_CORE_ARCH_H

void arch_disable_interrupts(void);
void arch_enable_interrupts(void);
void arch_halt(void);

#endif