/*
 * test_inode_stat.c - Unit tests for inode_stat module
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>

#include "../src/inode_stat.h"

static void test_init_and_free(void) {
    InodeStatTable table;
    int rc = inode_stat_table_init(&table, 4);
    assert(rc == 0);
    assert(table.entries != NULL);
    assert(table.count == 0);
    assert(table.capacity == 4);
    inode_stat_table_free(&table);
    printf("PASS test_init_and_free\n");
}

static void test_collect_existing_file(void) {
    /* Use /etc/hostname or /proc/version as a known existing path */
    const char *test_path = "/etc/hostname";
    struct stat st;
    if (stat(test_path, &st) != 0) {
        printf("SKIP test_collect_existing_file (no /etc/hostname)\n");
        return;
    }

    InodeStatTable table;
    assert(inode_stat_table_init(&table, 4) == 0);
    int rc = inode_stat_collect(&table, test_path);
    assert(rc == 0);
    assert(table.count == 1);
    assert(strcmp(table.entries[0].path, test_path) == 0);
    assert(table.entries[0].inode == st.st_ino);
    assert(table.entries[0].size == st.st_size);
    assert(table.entries[0].uid == st.st_uid);
    inode_stat_table_free(&table);
    printf("PASS test_collect_existing_file\n");
}

static void test_collect_missing_file(void) {
    InodeStatTable table;
    assert(inode_stat_table_init(&table, 4) == 0);
    int rc = inode_stat_collect(&table, "/nonexistent/path/xyz");
    assert(rc == -1);
    assert(table.count == 0);
    inode_stat_table_free(&table);
    printf("PASS test_collect_missing_file\n");
}

static void test_collect_multiple_and_grow(void) {
    const char *paths[] = {"/etc/hostname", "/etc/passwd", "/etc/group", "/etc/shells", "/etc/os-release"};
    int n = 5;
    InodeStatTable table;
    assert(inode_stat_table_init(&table, 2) == 0); /* start small to force grow */

    int collected = 0;
    for (int i = 0; i < n; i++) {
        struct stat st;
        if (stat(paths[i], &st) != 0) continue;
        int rc = inode_stat_collect(&table, paths[i]);
        assert(rc == 0);
        collected++;
    }
    assert((int)table.count == collected);
    inode_stat_table_free(&table);
    printf("PASS test_collect_multiple_and_grow (collected %d)\n", collected);
}

static void test_report_does_not_crash(void) {
    InodeStatTable table;
    assert(inode_stat_table_init(&table, 4) == 0);
    inode_stat_collect(&table, "/etc/hostname");
    inode_stat_report(&table); /* just shouldn't crash */
    inode_stat_table_free(&table);
    printf("PASS test_report_does_not_crash\n");
}

int main(void) {
    test_init_and_free();
    test_collect_existing_file();
    test_collect_missing_file();
    test_collect_multiple_and_grow();
    test_report_does_not_crash();
    printf("All inode_stat tests done.\n");
    return 0;
}
