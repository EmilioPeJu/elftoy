#include <stdlib.h>
#include <string.h>
#include <check.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "elfutils.h"
#include "tinybin.h"
#define TEST_FILE "test_content.txt"
#define TEST_CONTENT "test_content"
#define TEST_CONTENT_LEN 12
#define TEST_REPLACE_CONTENT "test_replace"
#define AFTER_SEGMENTS_ADDR (0x402010)


void create_test_file()
{
    int fd = open(TEST_FILE, O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU);
    write(fd, TEST_CONTENT, TEST_CONTENT_LEN);
    close(fd);
}

START_TEST (test_given_known_file_then_map_is_correct)
{
    struct map_entry map_entry;
    create_test_file();
    map_file(TEST_FILE, &map_entry);
    ck_assert(memcmp(TEST_CONTENT, map_entry.m_addr, TEST_CONTENT_LEN) == 0);
    unmap_file(&map_entry);
}
END_TEST

START_TEST (test_given_known_file_then_map_can_modify)
{
    create_test_file();
    struct map_entry write_map_entry;
    struct map_entry read_map_entry;
    map_file(TEST_FILE, &write_map_entry);
    memcpy(write_map_entry.m_addr, TEST_REPLACE_CONTENT, TEST_CONTENT_LEN);
    unmap_file(&write_map_entry);
    map_file(TEST_FILE, &read_map_entry);
    ck_assert(memcmp(TEST_REPLACE_CONTENT,
                     read_map_entry.m_addr,
                     TEST_CONTENT_LEN) == 0);
    unmap_file(&read_map_entry);
}
END_TEST

START_TEST (test_given_regular_binary_then_get_addr_is_correct)
{
    Elf64_Addr addr = get_addr_after_segments(&tinybin_map_entry);
    ck_assert(addr == AFTER_SEGMENTS_ADDR);
}
END_TEST

Suite * test_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("elfutils");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_given_known_file_then_map_is_correct);
    tcase_add_test(tc_core, test_given_known_file_then_map_can_modify);
    tcase_add_test(tc_core, test_given_regular_binary_then_get_addr_is_correct);
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
