#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include "elfpatching.h"

// todo: find a header that provides this declaration
void *memmem(const void *haystack, size_t haystacklen,
             const void *needle, size_t needlelen);


int patch_jmp_in_segment(Elf64_Addr orig_entry,
                         struct map_entry *target_file,
                         struct segment_entry *target_segment)
{
    void *segment_addr = (char *) target_file->m_addr + target_segment->s_offset;
    void *jmp_addr = memmem(segment_addr, (size_t) target_segment->s_sz,
                            "\xe9\x00\x00\x00\x00", 5);
    if (!jmp_addr) {
        return EXIT_FAILURE;
    }
    // 1 byte for e9 and 4 for the address
    Elf64_Addr jmp_offset = (Elf64_Addr) jmp_addr - (Elf64_Addr) segment_addr;
    Elf64_Addr src_addr = target_segment->s_addr + jmp_offset + 1 + 4;
    int32_t relative_addr = orig_entry - src_addr;
    *((int32_t *) ((char *) jmp_addr + 1)) = relative_addr;
    return EXIT_SUCCESS;
}

int patch_jmp_after_address(Elf64_Addr orig_entry,
                            Elf64_Addr target_addr,
                            struct map_entry *target_file)
{
    struct segment_entry virtual_segment;
    Elf64_Off target_off = get_offset(target_addr, target_file);
    assert(target_off != -1);
    virtual_segment.s_addr = target_addr;
    virtual_segment.s_offset = target_off;
    virtual_segment.s_sz = target_file->m_sz - target_off;
    return patch_jmp_in_segment(orig_entry, target_file, &virtual_segment);
}

