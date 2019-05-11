#ifndef ELFFUNCTIONS_H
#define ELFFUNCTIONS_H

#include "elfutils.h"

int _inject_note_segment_hijack_entry(char *bin_path, char *payload_path);

Elf64_Addr _hijack_entry(char *bin_path, Elf64_Addr addr);

Elf64_Addr _hijack_got(char *bin_path, char *symbol_name, Elf64_Addr addr);

Elf64_Addr _inject_note_segment(char *bin_path, char *payload_path);

int _patch_jmp(char *bin_path, Elf64_Addr orig_entry, Elf64_Addr addr);

#endif
