#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <assert.h>
#include "elfutils.h"


int map_file(char *filepath, struct map_entry *map_entry)
{
    struct stat filestat;
    int fd = open(filepath, O_RDWR);
    assert(fd > 0);
    int rc = fstat(fd, &filestat);
    assert(rc == 0);
    void *addr =  mmap(NULL, filestat.st_size , PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    assert(addr != MAP_FAILED);
    map_entry->m_addr = addr;
    map_entry->m_sz = filestat.st_size;
    close(fd);
    return 0;
}

void *unmap_file(struct map_entry *map_entry)
{
    munmap(map_entry->m_addr, map_entry->m_sz);
}

/*
 * Get an available virtual address, a page after the end of the last segment
 */
Elf64_Addr get_addr_after_segments(struct map_entry *target_file)
{
    Elf64_Ehdr *ehdr = target_file->m_addr;
    Elf64_Addr target_addr = 0;
    for (size_t index=0; index < ehdr->e_phnum; index++) {
        Elf64_Phdr *phdr = (Elf64_Phdr *) ((char *) ehdr + ehdr->e_phoff
                           + ehdr->e_phentsize*index);
        if (phdr->p_type == PT_LOAD) {
            Elf64_Addr candidate = phdr->p_vaddr + phdr->p_memsz;
            target_addr = (candidate > target_addr ? candidate : target_addr);
        }
    }
    return target_addr + PAGE_SIZE ;
}

Elf64_Phdr *get_phdr(int type, struct map_entry *target_file)
{
    Elf64_Ehdr *ehdr = target_file->m_addr;
    assert(memcmp(ehdr, ELFMAG, SELFMAG) == 0);
    for(size_t ph_index=0; ph_index < ehdr->e_phnum; ph_index++) {
        Elf64_Phdr *phdr = (Elf64_Phdr *) ((char *) target_file->m_addr + ehdr->e_phoff
            + ph_index*ehdr->e_phentsize);
        if (phdr->p_type == type) {
            printf("Found phdr type %d index: %d\n", type, ph_index);
            return phdr;
        }
    }
    return NULL;
}
