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
    printf("file start: %lx file end: %lx payload start: %lx jmp_addr: %lx\n",
           target_file->m_addr,
           (char *) target_file->m_addr + target_file->m_sz,
           segment_addr, jmp_addr);
    if (!jmp_addr) {
        printf("jmp to patch not found");
        return EXIT_FAILURE;
    }
    printf("Found jmp in %lx\n", jmp_addr);
    // 1 byte for e9 and 4 for the address
    Elf64_Addr jmp_offset = (Elf64_Addr) jmp_addr - (Elf64_Addr) segment_addr;
    printf("Jmp offset: %lx\n", jmp_offset);
    Elf64_Addr src_addr = target_segment->s_addr + jmp_offset + 1 + 4;
    int32_t relative_addr = orig_entry - src_addr;
    printf("jmp argument %x\n", relative_addr);
    *((int32_t *) ((char *) jmp_addr + 1)) = relative_addr;
    return EXIT_SUCCESS;
}

