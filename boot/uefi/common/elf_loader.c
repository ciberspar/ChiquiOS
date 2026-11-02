
#include "elf_loader.h"
#include "../../common/elf_format.h"

#define CBL_PAGE_SIZE 4096ULL
#define CBL_MAX_SEGMENTS 32

static int range_valid(
    uint64_t offset,
    uint64_t length,
    uint64_t total
) {
    if (offset > total) return 0;
    if (length > total - offset) return 0;
    return 1;
}

static void copy_bytes(
    void *destination,
    const void *source,
    size_t length
) {
    unsigned char *d = destination;
    const unsigned char *s = source;

    for (size_t i = 0; i < length; i++)
        d[i] = s[i];
}

static void zero_bytes(void *destination, size_t length) {
    unsigned char *d = destination;

    for (size_t i = 0; i < length; i++)
        d[i] = 0;
}

static int load_segment(
    const unsigned char *image,
    size_t image_size,
    uint64_t offset,
    uint64_t physical_address,
    uint64_t file_size,
    uint64_t memory_size,
    uint64_t alignment,
    const cbl_elf_memory_t *memory
) {
    if (file_size > memory_size)
        return 0;

    if (!memory_size)
        return 0;

    if (!range_valid(offset, file_size, image_size))
        return 0;

    if (physical_address > UINT64_MAX - memory_size)
        return 0;

    if (alignment > 1) {
        if ((alignment & (alignment - 1)) != 0)
            return 0;

        if ((physical_address % alignment) !=
            (offset % alignment))
            return 0;
    }

    uint64_t start =
        physical_address & ~(CBL_PAGE_SIZE - 1);

    uint64_t end = physical_address + memory_size;

    if (end > UINT64_MAX - (CBL_PAGE_SIZE - 1))
        return 0;

    uint64_t rounded_end =
        (end + CBL_PAGE_SIZE - 1) &
        ~(CBL_PAGE_SIZE - 1);

    uint64_t pages =
        (rounded_end - start) / CBL_PAGE_SIZE;

    if (!memory->allocate_pages_at(
            memory->context, start, pages))
        return 0;

    unsigned char *destination =
        (unsigned char *)(uintptr_t)physical_address;

    copy_bytes(destination, image + offset, (size_t)file_size);

    zero_bytes(
        destination + file_size,
        (size_t)(memory_size - file_size)
    );

    return 1;
}

static int load_elf32(
    const unsigned char *image,
    size_t image_size,
    const cbl_elf_memory_t *memory,
    cbl_elf_result_t *result
) {
    if (image_size < sizeof(cbl_elf32_header_t))
        return 0;

    const cbl_elf32_header_t *header =
        (const cbl_elf32_header_t *)image;

    if (header->machine != CBL_EM_386 ||
        header->type != CBL_ELF_EXEC)
        return 0;

    if (header->phentsize !=
        sizeof(cbl_elf32_program_header_t))
        return 0;

    if (header->phnum == 0 ||
        header->phnum > CBL_MAX_SEGMENTS)
        return 0;

    uint64_t table_size =
        (uint64_t)header->phnum * header->phentsize;

    if (!range_valid(header->phoff, table_size, image_size))
        return 0;

    const cbl_elf32_program_header_t *programs =
        (const cbl_elf32_program_header_t *)
        (image + header->phoff);

    uint32_t loaded = 0;

    for (uint16_t i = 0; i < header->phnum; i++) {
        const cbl_elf32_program_header_t *p = &programs[i];

        if (p->type != CBL_PT_LOAD)
            continue;

        if (!load_segment(
            image,
            image_size,
            p->offset,
            p->paddr,
            p->filesz,
            p->memsz,
            p->align,
            memory
        ))
            return 0;

        loaded++;
    }

    if (!loaded)
        return 0;

    result->entry = header->entry;
    result->loaded_segments = loaded;
    return 1;
}

static int load_elf64(
    const unsigned char *image,
    size_t image_size,
    const cbl_elf_memory_t *memory,
    cbl_elf_result_t *result
) {
    if (image_size < sizeof(cbl_elf64_header_t))
        return 0;

    const cbl_elf64_header_t *header =
        (const cbl_elf64_header_t *)image;

    if (header->machine != CBL_EM_X86_64 ||
        header->type != CBL_ELF_EXEC)
        return 0;

    if (header->phentsize !=
        sizeof(cbl_elf64_program_header_t))
        return 0;

    if (header->phnum == 0 ||
        header->phnum > CBL_MAX_SEGMENTS)
        return 0;

    uint64_t table_size =
        (uint64_t)header->phnum * header->phentsize;

    if (!range_valid(header->phoff, table_size, image_size))
        return 0;

    const cbl_elf64_program_header_t *programs =
        (const cbl_elf64_program_header_t *)
        (image + header->phoff);

    uint32_t loaded = 0;

    for (uint16_t i = 0; i < header->phnum; i++) {
        const cbl_elf64_program_header_t *p = &programs[i];

        if (p->type != CBL_PT_LOAD)
            continue;

        if (!load_segment(
            image,
            image_size,
            p->offset,
            p->paddr,
            p->filesz,
            p->memsz,
            p->align,
            memory
        ))
            return 0;

        loaded++;
    }

    if (!loaded)
        return 0;

    result->entry = header->entry;
    result->loaded_segments = loaded;
    return 1;
}

int cbl_elf_load(
    const void *image_data,
    size_t image_size,
    unsigned int expected_class,
    unsigned int expected_machine,
    const cbl_elf_memory_t *memory,
    cbl_elf_result_t *result
) {
    if (!image_data || !memory || !result ||
        !memory->allocate_pages_at)
        return 0;

    if (image_size < 16)
        return 0;

    const unsigned char *image =
        (const unsigned char *)image_data;

    if (image[0] != CBL_ELF_MAGIC_0 ||
        image[1] != CBL_ELF_MAGIC_1 ||
        image[2] != CBL_ELF_MAGIC_2 ||
        image[3] != CBL_ELF_MAGIC_3)
        return 0;

    if (image[5] != CBL_ELFDATA2LSB)
        return 0;

    if (expected_class == CBL_ELFCLASS32 &&
        expected_machine == CBL_EM_386)
        return load_elf32(image, image_size, memory, result);

    if (expected_class == CBL_ELFCLASS64 &&
        expected_machine == CBL_EM_X86_64)
        return load_elf64(image, image_size, memory, result);

    return 0;
}
