#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/symlink_tracker.h"

static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) do { tests_run++; printf("  test: %s\n", name); } while(0)
#define PASS() do { tests_passed++; } while(0)
#define FAIL(msg) do { printf("  FAIL: %s\n", msg); } while(0)

void test_create_free(void) {
    TEST("create and free");
    SymlinkTracker *t = symlink_tracker_create();
    assert(t != NULL);
    assert(symlink_tracker_count(t) == 0);
    symlink_tracker_free(t);
    PASS();
}

void test_add_and_resolve(void) {
    TEST("add and resolve symlink");
    SymlinkTracker *t = symlink_tracker_create();
    assert(t);
    int r = symlink_tracker_add(t, "/usr/bin/sh", "/bin/bash");
    assert(r == 0);
    assert(symlink_tracker_count(t) == 1);
    const char *target = symlink_tracker_resolve(t, "/usr/bin/sh");
    assert(target != NULL);
    assert(strcmp(target, "/bin/bash") == 0);
    symlink_tracker_free(t);
    PASS();
}

void test_resolve_missing(void) {
    TEST("resolve non-existent path returns NULL");
    SymlinkTracker *t = symlink_tracker_create();
    assert(t);
    const char *res = symlink_tracker_resolve(t, "/no/such/link");
    assert(res == NULL);
    symlink_tracker_free(t);
    PASS();
}

void test_multiple_entries(void) {
    TEST("multiple symlinks");
    SymlinkTracker *t = symlink_tracker_create();
    assert(t);
    symlink_tracker_add(t, "/a", "/b");
    symlink_tracker_add(t, "/b", "/c");
    symlink_tracker_add(t, "/c", "/d");
    assert(symlink_tracker_count(t) == 3);
    assert(strcmp(symlink_tracker_resolve(t, "/b"), "/c") == 0);
    symlink_tracker_free(t);
    PASS();
}

void test_cycle_detection(void) {
    TEST("cycle detection");
    SymlinkTracker *t = symlink_tracker_create();
    assert(t);
    symlink_tracker_add(t, "/loop1", "/loop2");
    symlink_tracker_add(t, "/loop2", "/loop1");
    int has_cycle = symlink_tracker_has_cycle(t, "/loop1");
    assert(has_cycle == 1);
    symlink_tracker_free(t);
    PASS();
}

void test_no_cycle(void) {
    TEST("no false positive cycle");
    SymlinkTracker *t = symlink_tracker_create();
    assert(t);
    symlink_tracker_add(t, "/x", "/y");
    symlink_tracker_add(t, "/y", "/z");
    int has_cycle = symlink_tracker_has_cycle(t, "/x");
    assert(has_cycle == 0);
    symlink_tracker_free(t);
    PASS();
}

int main(void) {
    printf("=== symlink_tracker tests ===\n");
    test_create_free();
    test_add_and_resolve();
    test_resolve_missing();
    test_multiple_entries();
    test_cycle_detection();
    test_no_cycle();
    printf("\n%d/%d tests passed\n", tests_passed, tests_run);
    return tests_passed == tests_run ? 0 : 1;
}
