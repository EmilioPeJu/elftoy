#include <stdio.h>
#include <assert.h>
#include "debugutils.h"

void read_memory(pid_t pid, uint64_t addr, void *buffer, size_t len)
{
    assert(len % 4 == 0);
    int *cbuffer = buffer;
    long data;
    for (size_t i=0; i < len; i++) {
        data = ptrace(PTRACE_PEEKTEXT, pid, addr+i*4, NULL);
        printf("Got %ld from ptrace\n", data);
        cbuffer[i] = data;
    }
}
