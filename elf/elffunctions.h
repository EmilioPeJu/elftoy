#ifndef ELFFUNCTIONS_H
#define ELFFUNCTIONS_H

#include "elfutils.h"

int _inject_PT_NOTE_hijack_entry(char *bin_path, char *payload_path);

#endif
