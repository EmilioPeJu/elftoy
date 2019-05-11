#ifndef ELFHIJACKING_H
#define ELFHIJACKING_H
#include "elfutils.h"

Elf64_Addr hijack_entry(Elf64_Addr new_entry,
                        struct map_entry *target_file);

Elf64_Addr hijack_got(char *symbol_name,
                      Elf64_Addr new_addr,
                      struct map_entry *target_file);
#endif

