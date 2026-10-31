
#ifndef CHIQUIOS_CORE_TYPES_H
#define CHIQUIOS_CORE_TYPES_H

/* Enteros de tamaño fijo */

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;

typedef signed char        i8;
typedef signed short       i16;
typedef signed int         i32;
typedef signed long long   i64;

/* Tipos que se adaptan a la arquitectura */

typedef __SIZE_TYPE__ usize;
typedef __UINTPTR_TYPE__ uptr;

/* Resultados */

typedef i32 chiquios_status_t;

#define CHIQUIOS_OK       0
#define CHIQUIOS_ERROR   -1

#ifndef NULL
#define NULL ((void *)0)
#endif

#endif
