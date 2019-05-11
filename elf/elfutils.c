#include <stdio.h>
#include <string.h>
#include <stdbool.h>
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
        // TODO: memory range check
        Elf64_Phdr *phdr = (Elf64_Phdr *) ((char *) ehdr + ehdr->e_phoff
                           + ehdr->e_phentsize*index);
        if (phdr->p_type == PT_LOAD) {
            Elf64_Addr candidate = phdr->p_vaddr + phdr->p_memsz;
            target_addr = (candidate > target_addr ? candidate : target_addr);
        }
    }
    return target_addr + PAGE_SIZE;
}

Elf64_Phdr *get_phdr(int type, struct map_entry *target_file)
{
    Elf64_Ehdr *ehdr = target_file->m_addr;
    assert(memcmp(ehdr, ELFMAG, SELFMAG) == 0);
    for(size_t ph_index=0; ph_index < ehdr->e_phnum; ph_index++) {
        // TODO: memory range check
        Elf64_Phdr *phdr = (Elf64_Phdr *) ((char *) target_file->m_addr + ehdr->e_phoff
            + ph_index*ehdr->e_phentsize);
        if (phdr->p_type == type) {
            return phdr;
        }
    }
    return NULL;
}

uint64_t get_dyn_val(int tag, struct map_entry *target_file)
{
    Elf64_Phdr *dyn_phdr = get_phdr(PT_DYNAMIC, target_file);
    assert(dyn_phdr != NULL);
    // TODO: memory range check
    Elf64_Dyn *dyn = (Elf64_Dyn *) ((char *) target_file->m_addr + dyn_phdr->p_offset);
    while (dyn->d_tag != DT_NULL) {
        if (dyn->d_tag == tag)
            return dyn->d_un.d_val;
        dyn+=1;
    }
    return -1; // not sure if this value make sense in any type of dyn
}

void *get_dyn_ptr(int type, struct map_entry *target_file)
{
    Elf64_Addr addr = get_dyn_val(type, target_file);
    assert(addr != -1);
    Elf64_Off off = get_offset(addr, target_file);
    assert(off != -1);
    char *ptr = (char *) target_file->m_addr + off;
    return ptr;
}

size_t get_dyn_sym_num(struct map_entry *target_file)
{
    // this is wrong, but at least it gives a limit
    // usually GNU_HASH is used for that
    char *strtab = get_dyn_ptr(DT_STRTAB, target_file);
    strtab++;
    size_t count = 0;
    while (*strtab != 0) {
        count++;
        strtab += strlen(strtab) + 1;
    }
    return count;
}

Elf64_Sym *get_dynsym(char *name, size_t *index, struct map_entry *target_file)
{
    char *strtab = get_dyn_ptr(DT_STRTAB, target_file);
    assert(strtab != NULL);
    Elf64_Sym *symtab = get_dyn_ptr(DT_SYMTAB, target_file);
    assert(symtab != NULL);
    size_t sym_num = get_dyn_sym_num(target_file);
    // TODO: check table limits
    size_t i = 0;
    bool found = false;
    while (i < sym_num) {
        if (strcmp(name, strtab + symtab[i].st_name) == 0) {
            if (index != NULL)
                *index = i;
            found = true;
            break;
        }
        i++;
    }

    if (!found)
        return NULL;

    return &symtab[i];
}

Elf64_Rela *get_jmprel_for(char *name, struct map_entry *target_file)
{
    size_t sym_index;
    Elf64_Sym *sym = get_dynsym(name, &sym_index, target_file);
    assert(sym != NULL);
    Elf64_Addr rela_addr = get_dyn_val(DT_JMPREL, target_file);
    assert(rela_addr != -1);
    Elf64_Off rela_off = get_offset(rela_addr, target_file);
    assert(rela_addr != -1);
    // TODO: again, check limits
    Elf64_Rela *rela = (Elf64_Rela *) ((char *) target_file->m_addr + rela_off);
    while (ELF64_R_TYPE(rela->r_info) != R_X86_64_NONE) {
        if (ELF64_R_SYM(rela->r_info) == sym_index && ELF64_R_TYPE(rela->r_info) == R_X86_64_JUMP_SLOT ) {
            return rela;
        }
        rela += 1;
    }
    return NULL;
}

Elf64_Off get_offset(Elf64_Addr addr, struct map_entry *target_file)
{
    Elf64_Ehdr *ehdr = target_file->m_addr;
    for (size_t ph_i=0; ph_i<ehdr->e_phnum; ph_i++) {
        Elf64_Phdr *phdr = (Elf64_Phdr *) ((char *) target_file->m_addr 
                + ehdr->e_phoff + ehdr->e_phentsize*ph_i);
        if (phdr->p_vaddr <= addr && addr <= phdr->p_vaddr + phdr->p_filesz) {
            return phdr->p_offset + (addr - phdr->p_vaddr);
        }
    }
    // that address is not mapped from the binary file
    return -1;
}
