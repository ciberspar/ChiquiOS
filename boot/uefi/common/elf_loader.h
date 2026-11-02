
#ifndef CBL_ELF_LOADER_H
#define CBL_ELF_LOADER_H

#include <stdint.h>
#include <stddef.h>

typedef int (*cbl_allocate_pages_at_fn)(
    void *context,
    uint64_t physical_address,
    uint64_t page_count
);

typedef struct {
    void *context;
    cbl_allocate_pages_at_fn allocate_pages_at;
} cbl_elf_memory_t;

typedef struct {
    uint64_t entry;
    uint32_t loaded_segments;
} cbl_elf_result_t;

int cbl_elf_load(
    const void *image,
    size_t image_size,
    unsigned int expected_class,
    unsigned int expected_machine,
    const cbl_elf_memory_t *memory,
    cbl_elf_result_t *result
);

#endif
