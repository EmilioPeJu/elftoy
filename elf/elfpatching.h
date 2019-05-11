#ifndef ELFPATCHING_H
#define ELFPATCHING_H
#include "elfutils.h"

int patch_jmp_in_segment(Elf64_Addr orig_entry,
                         struct map_entry *target_file,
                         struct segment_entry *target_segment);

int patch_jmp_after_address(Elf64_Addr orig_entry,
                            Elf64_Addr target_addr,
                            struct map_entry *target_file);

#endif
