#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "elfhijacking.h"

Elf64_Addr hijack_entry(Elf64_Addr new_entry,
                        struct map_entry *target_file)
{
    Elf64_Addr orig_entry;
    Elf64_Ehdr *ehdr = target_file->m_addr;
    // are we really in the ELF header?
    assert(memcmp(ehdr, ELFMAG, SELFMAG) == 0);
    orig_entry = ehdr->e_entry;
    ehdr->e_entry = new_entry;
    printf("Changed entry point from 0x%lx to 0x%lx\n", orig_entry, new_entry);
    return orig_entry;
}

