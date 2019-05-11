#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <check.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "elfhijacking.h"
#include "tinybin.h"
#define NEW_ENTRY (0x1234abcd)
#define ORIGINAL_ENTRY (0x401000)


START_TEST (test_given_regular_elf_file_then_hijack_entry_works)
{
    Elf64_Addr entry_point = hijack_entry(NEW_ENTRY, &tinybin_map_entry);
    ck_assert(entry_point == ORIGINAL_ENTRY);
    Elf64_Ehdr *ehdr = tinybin_map_entry.m_addr;
    ck_assert(ehdr->e_entry == NEW_ENTRY);

START_TEST (test_given_regular_elf_file_then_hijack_got_replaces_entry)
{
    Elf64_Addr orig_addr = hijack_got("get_val4",
                                      TEST_ADDR,
                                      &tinybin_usinglib_map_entry);
    ck_assert(orig_addr == 0x401026);
    Elf64_Addr *addr = (Elf64_Addr *) ((char *) tinybin_usinglib_map_entry.m_addr + 0x3020);
    ck_assert(*addr == TEST_ADDR);
}
END_TEST

Suite * test_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("elfhijacking");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core,
            test_given_regular_elf_file_then_hijack_entry_works);
            test_given_regular_elf_file_then_hijack_got_replaces_entry);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = test_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
