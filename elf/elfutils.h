#ifndef ELFUTILS_H
#define ELFUTILS_H
#include <elf.h>
#define PAGE_SIZE (0x1000)

struct map_entry {
    void *m_addr;
    size_t m_sz;
};

struct segment_entry {
    Elf64_Off s_offset;
    Elf64_Addr   s_addr;
    size_t       s_sz;
};

int map_file(char *filepath, struct map_entry *map_entry);

void *unmap_file(struct map_entry *map_entry);

Elf64_Addr get_addr_after_segments(struct map_entry *target_file);

#endif
