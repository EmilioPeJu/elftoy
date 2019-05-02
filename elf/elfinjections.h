#ifndef ELFINJECTIONS_H
#define ELFINJECTIONS_H
#include "elfutils.h"

int inject_PT_NOTE(Elf64_Addr target_addr,
                   size_t target_sz,
                   struct map_entry *target_file,
                   struct segment_entry *target_segment);

int append_payload(char *target_filepath,
                   char *payload_filepath);

#endif
