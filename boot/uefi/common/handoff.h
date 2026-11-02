
#ifndef CBL_HANDOFF_H
#define CBL_HANDOFF_H

#include <stdint.h>

#if defined(__i386__)

__attribute__((noreturn))
void cbl_ia32_handoff(
    void *boot_info,
    uint32_t kernel_entry
);

#elif defined(__x86_64__)

__attribute__((noreturn))
void cbl_x64_handoff(
    void *boot_info,
    uint64_t kernel_entry
);

#else
#error "Unsupported architecture"
#endif

#endif
