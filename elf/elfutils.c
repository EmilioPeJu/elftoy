#include <stdio.h>
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

