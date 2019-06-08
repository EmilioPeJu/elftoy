#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "debugutils.h"
#define READ_ERROR "Error while reading data"
#define WRITE_ERROR "Error while writing data"
#define EXPECT_DATA(expr, msg) if (expr == -1 && errno) { \
                              printf( msg ); \
                              return -errno; }

int ptrace_read_memory(pid_t pid, uint64_t addr, void *buffer, size_t len)
{
    long *lbuffer = buffer;
    char *cbuffer = buffer;
    long data;
    size_t rem = len % 8;
    len -= rem;
    for (size_t i=0, j=0; i < len; i+=8, j++) {
        errno = 0;
        data = ptrace(PTRACE_PEEKTEXT, pid, addr+i, NULL);
        EXPECT_DATA(data, READ_ERROR);
        lbuffer[j] = data;
    }
    if (rem) {
        errno = 0;
        data = ptrace(PTRACE_PEEKTEXT, pid, addr+len, NULL);
        EXPECT_DATA(data, READ_ERROR);
        for (size_t i=0; i<rem; i++) {
            cbuffer[len+i] = (data>>(8*i)) & 0xff;
        }
    }
    return EXIT_SUCCESS;
}

int ptrace_write_memory(pid_t pid, uint64_t addr, void *buffer, size_t len)
{
    long *lbuffer = buffer;
    char *cbuffer = buffer;
    long data;
    size_t rem = len % 8;
    len -= rem;
    for (size_t i=0, j=0; i < len; i+=8, j++) {
        errno = 0;
        long rc = ptrace(PTRACE_POKETEXT, pid, addr+i, lbuffer[j]);
        EXPECT_DATA(rc, WRITE_ERROR);
    }
    if (rem) {
        errno = 0;
        data = ptrace(PTRACE_PEEKTEXT, pid, addr+len, NULL);
        EXPECT_DATA(data, READ_ERROR);
        // clear first rem bytes
        data = (data >> (8*rem)) << (8*rem);
        for (size_t i=0; i < rem; i++) {
            data |= cbuffer[len+i] << (8*i);
        }
        errno = 0;
        long rc = ptrace(PTRACE_POKETEXT, pid, addr+len, data);
        EXPECT_DATA(rc, WRITE_ERROR);
    }
    return EXIT_SUCCESS;
}

int mem_read_memory(pid_t pid, uint64_t addr, void *buffer, size_t len) {
    char path[64];
    sprintf(path, "/proc/%ld/mem", (long) pid);
    int fd = open(path, O_RDONLY);
    errno = 0;
    size_t nread = pread(fd, buffer, len, (off_t) addr);
    EXPECT_DATA(nread, READ_ERROR);
    close(fd);
    return EXIT_SUCCESS;
}

int mem_write_memory(pid_t pid, uint64_t addr, void *buffer, size_t len)
{
    char path[64];
    sprintf(path, "/proc/%ld/mem", (long) pid);
    int fd = open(path, O_WRONLY);
    errno = 0;
    size_t nwrite = pwrite(fd, buffer, len, (off_t) addr);
    EXPECT_DATA(nwrite, WRITE_ERROR);
    close(fd);
    return EXIT_SUCCESS;
}
