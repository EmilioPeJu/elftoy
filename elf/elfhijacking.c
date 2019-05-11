#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "elfhijacking.h"

Elf64_Addr hijack_entry(Elf64_Addr new_addr,
                        struct map_entry *target_file)
{
    Elf64_Addr orig_entry;
    Elf64_Ehdr *ehdr = target_file->m_addr;
    // are we really in the ELF header?
    assert(memcmp(ehdr, ELFMAG, SELFMAG) == 0);
    orig_entry = ehdr->e_entry;
    ehdr->e_entry = new_addr;
    return orig_entry;
}

Elf64_Addr hijack_got(char *symbol_name,
                      Elf64_Addr new_addr,
                      struct map_entry *target_file)
{
    // there are many ways of doing this
    // I'll do it using the jmp relocation table
    Elf64_Rela *rela = get_jmprel_for(symbol_name, target_file);
    assert(rela != NULL);
    Elf64_Addr got_entry_addr = rela->r_offset;
    Elf64_Off got_entry_off = get_offset(got_entry_addr, target_file);
    Elf64_Addr *got_entry = (Elf64_Addr *) ((char *) target_file->m_addr + got_entry_off);
    Elf64_Addr orig_addr = *got_entry;
    *got_entry = new_addr;
    return orig_addr;
}
