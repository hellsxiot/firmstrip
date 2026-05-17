#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>
#include "../src/inode_stat.h"

static void test_fill_regular_file(void) {
    const char *tmp = "/tmp/firmstrip_inode_test.txt";
    FILE *f = fopen(tmp, "w");
    assert(f);
    fputs("hello inode", f);
    fclose(f);

    InodeStat s;
    int rc = inode_stat_fill(tmp, &s);
    assert(rc == 0);
    assert(strcmp(s.path, tmp) == 0);
    assert(s.type == INODE_REG);
    assert(s.size == 11);
    assert(s.inode_num > 0);
    assert(s.nlink >= 1);
    unlink(tmp);
    printf("PASS: test_fill_regular_file\n");
}

static void test_fill_directory(void) {
    const char *tmp = "/tmp";
    InodeStat s;
    int rc = inode_stat_fill(tmp, &s);
    assert(rc == 0);
    assert(s.type == INODE_DIR);
    assert(s.inode_num > 0);
    printf("PASS: test_fill_directory\n");
}

static void test_fill_symlink(void) {
    const char *target = "/tmp/firmstrip_inode_target.txt";
    const char *link   = "/tmp/firmstrip_inode_link.txt";
    FILE *f = fopen(target, "w");
    assert(f); fclose(f);
    symlink(target, link);

    InodeStat s;
    int rc = inode_stat_fill(link, &s);
    assert(rc == 0);
    assert(s.type == INODE_LNK);
    unlink(link);
    unlink(target);
    printf("PASS: test_fill_symlink\n");
}

static void test_fill_null_args(void) {
    InodeStat s;
    assert(inode_stat_fill(NULL, &s)  == -1);
    assert(inode_stat_fill("/tmp", NULL) == -1);
    printf("PASS: test_fill_null_args\n");
}

static void test_fill_nonexistent(void) {
    InodeStat s;
    int rc = inode_stat_fill("/tmp/firmstrip_does_not_exist_xyz", &s);
    assert(rc == -1);
    printf("PASS: test_fill_nonexistent\n");
}

static void test_print_does_not_crash(void) {
    InodeStat s;
    inode_stat_fill("/tmp", &s);
    inode_stat_print(&s, stdout);
    inode_stat_print(NULL, stdout);
    printf("PASS: test_print_does_not_crash\n");
}

int main(void) {
    test_fill_regular_file();
    test_fill_directory();
    test_fill_symlink();
    test_fill_null_args();
    test_fill_nonexistent();
    test_print_does_not_crash();
    printf("All inode_stat tests passed.\n");
    return 0;
}
