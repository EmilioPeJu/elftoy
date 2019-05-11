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

START_TEST (test_get_phdr_finds_PT_LOAD)
{
    Elf64_Phdr *phdr = get_phdr(PT_LOAD, &tinybin_map_entry);
    ck_assert((uint64_t) phdr == (uint64_t) ((char *) tinybin_map_entry.m_addr + 0xe8));
}
END_TEST

START_TEST (test_get_dyn_val_finds_DT_PLTGOT)
{
    uint64_t got_addr = get_dyn_val(DT_PLTGOT, &tinybin_usinglib_map_entry);
    ck_assert(got_addr == 0x404000);
}
END_TEST

START_TEST (test_get_dynsym_gets_symbol)
{
    size_t index;
    Elf64_Sym *sym = get_dynsym("get_val",
                                &index,
                                &tinybin_usinglib_map_entry);
    ck_assert(index == 3);
    Elf64_Sym *expected_sym = (Elf64_Sym *) ((char *) tinybin_usinglib_map_entry.m_addr + 0x350);
    ck_assert(sym == expected_sym);
}
END_TEST

START_TEST(test_get_jmprel_for_gets_rela)
{
    Elf64_Rela *rela = get_jmprel_for("get_val", &tinybin_usinglib_map_entry);
    Elf64_Rela *expected_rela = (Elf64_Rela *) ((char *) tinybin_usinglib_map_entry.m_addr + 0x420);
    ck_assert(rela == expected_rela);
}
END_TEST

START_TEST(test_get_offset)
{
    Elf64_Off offset = get_offset(0x00403eef, &tinybin_usinglib_map_entry);
    ck_assert(offset == 0x00002eef);
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
    tcase_add_test(tc_core, test_get_dyn_val_finds_DT_PLTGOT);
    tcase_add_test(tc_core, test_get_dynsym_gets_symbol);
    tcase_add_test(tc_core, test_get_jmprel_for_gets_rela);
    tcase_add_test(tc_core, test_get_offset);
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
