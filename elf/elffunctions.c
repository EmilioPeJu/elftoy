#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include "elffunctions.h"
#include "elfinjections.h"
#include "elfhijacking.h"
#include "elfpatching.h"

static size_t get_file_size(const char *filepath) {
    struct stat filestat;
    int rc = stat(filepath, &filestat);
    assert(rc == 0);
    return filestat.st_size;
}

int _inject_PT_NOTE_hijack_entry(char *bin_path, char *payload_path)
{
    struct segment_entry payload_segment;
    struct map_entry target_file;
    Elf64_Addr target_addr = 0xa000000;
    size_t payload_size = get_file_size(payload_path);
    printf("Payload size: %x\n", payload_size);
    int rc;
    rc = map_file(bin_path, &target_file);
    assert(rc == 0);
    // the address will be corrected to be congruent with offset mod page_size
    rc = inject_PT_NOTE(target_addr,
                        payload_size,
                        &target_file,
                        &payload_segment);
    assert(rc == 0);
    printf("Got target_addr %lx\n", payload_segment.s_addr);
    Elf64_Addr orig_entry = hijack_entry(payload_segment.s_addr, &target_file);
    unmap_file(&target_file);
    // we are appending content to the file, so, we need to unmap to commit
    // changes and after adding the payload we can map again
    append_payload(bin_path, payload_path);
    rc = map_file(bin_path, &target_file);
    assert(rc == 0);
    patch_jmp_in_segment(orig_entry, &target_file, &payload_segment);
    unmap_file(&target_file);
    return 0;
}

int test_main(int argc, char **argv)
{
    _inject_PT_NOTE_hijack_entry(argv[1], argv[2]);
    return 0;
}
