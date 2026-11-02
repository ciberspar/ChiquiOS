
#ifndef CBL_ELF_FORMAT_H
#define CBL_ELF_FORMAT_H

#include <stdint.h>

#define CBL_ELF_MAGIC_0 0x7F
#define CBL_ELF_MAGIC_1 'E'
#define CBL_ELF_MAGIC_2 'L'
#define CBL_ELF_MAGIC_3 'F'

#define CBL_ELFCLASS32 1
#define CBL_ELFCLASS64 2

#define CBL_ELFDATA2LSB 1
#define CBL_ELF_EXEC 2

#define CBL_EM_386 3
#define CBL_EM_X86_64 62

#define CBL_PT_LOAD 1

#define CBL_PF_X 1
#define CBL_PF_W 2
#define CBL_PF_R 4

typedef struct {
    unsigned char ident[16];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint32_t entry;
    uint32_t phoff;
    uint32_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
} __attribute__((packed)) cbl_elf32_header_t;

typedef struct {
    uint32_t type;
    uint32_t offset;
    uint32_t vaddr;
    uint32_t paddr;
    uint32_t filesz;
    uint32_t memsz;
    uint32_t flags;
    uint32_t align;
} __attribute__((packed)) cbl_elf32_program_header_t;

typedef struct {
    unsigned char ident[16];
    uint16_t type;
    uint16_t machine;
    uint32_t version;
    uint64_t entry;
    uint64_t phoff;
    uint64_t shoff;
    uint32_t flags;
    uint16_t ehsize;
    uint16_t phentsize;
    uint16_t phnum;
    uint16_t shentsize;
    uint16_t shnum;
    uint16_t shstrndx;
} __attribute__((packed)) cbl_elf64_header_t;

typedef struct {
    uint32_t type;
    uint32_t flags;
    uint64_t offset;
    uint64_t vaddr;
    uint64_t paddr;
    uint64_t filesz;
    uint64_t memsz;
    uint64_t align;
} __attribute__((packed)) cbl_elf64_program_header_t;

#endif
