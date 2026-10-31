
#ifndef CHIQUIOS_ARCH_CONFIG_H
#define CHIQUIOS_ARCH_CONFIG_H

/* Arquitecturas compatibles */

#if defined(__x86_64__)

#define CHIQUIOS_ARCH_X86_64 1
#define CHIQUIOS_ARCH_I386   0
#define CHIQUIOS_ARCH_NAME   "x86_64"

typedef unsigned long long arch_uintptr_t;
typedef long long arch_intptr_t;

#elif defined(__i386__)

#define CHIQUIOS_ARCH_X86_64 0
#define CHIQUIOS_ARCH_I386   1
#define CHIQUIOS_ARCH_NAME   "i386"

typedef unsigned int arch_uintptr_t;
typedef int arch_intptr_t;

#else

#error "Arquitectura no compatible con ChiquiOS"

#endif

#define CHIQUIOS_PAGE_SIZE 4096ULL

#endif
