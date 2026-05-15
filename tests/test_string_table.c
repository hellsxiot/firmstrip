#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "../src/string_table.h"

static void test_init_and_free(void) {
    StringTable tbl;
    assert(string_table_init(&tbl) == 0);
    assert(string_table_count(&tbl) == 0);
    string_table_free(&tbl);
    printf("  [PASS] init_and_free\n");
}

static void test_intern_new_string(void) {
    StringTable tbl;
    string_table_init(&tbl);

    const char *s = string_table_intern(&tbl, "/etc/passwd");
    assert(s != NULL);
    assert(strcmp(s, "/etc/passwd") == 0);
    assert(string_table_count(&tbl) == 1);

    string_table_free(&tbl);
    printf("  [PASS] intern_new_string\n");
}

static void test_intern_deduplicates(void) {
    StringTable tbl;
    string_table_init(&tbl);

    const char *a = string_table_intern(&tbl, "/bin/sh");
    const char *b = string_table_intern(&tbl, "/bin/sh");
    assert(a != NULL && b != NULL);
    assert(a == b); /* same pointer */
    assert(string_table_count(&tbl) == 1);

    string_table_free(&tbl);
    printf("  [PASS] intern_deduplicates\n");
}

static void test_lookup_missing(void) {
    StringTable tbl;
    string_table_init(&tbl);

    const char *r = string_table_lookup(&tbl, "/not/there");
    assert(r == NULL);

    string_table_free(&tbl);
    printf("  [PASS] lookup_missing\n");
}

static void test_multiple_entries(void) {
    StringTable tbl;
    string_table_init(&tbl);

    const char *paths[] = {"/usr/bin", "/usr/lib", "/etc", "/var/log"};
    size_t n = sizeof(paths) / sizeof(paths[0]);

    for (size_t i = 0; i < n; i++)
        assert(string_table_intern(&tbl, paths[i]) != NULL);

    assert(string_table_count(&tbl) == n);
    assert(string_table_lookup(&tbl, "/etc") != NULL);
    assert(string_table_lookup(&tbl, "/tmp") == NULL);

    string_table_free(&tbl);
    printf("  [PASS] multiple_entries\n");
}

int main(void) {
    printf("Running string_table tests...\n");
    test_init_and_free();
    test_intern_new_string();
    test_intern_deduplicates();
    test_lookup_missing();
    test_multiple_entries();
    printf("All string_table tests passed.\n");
    return 0;
}
