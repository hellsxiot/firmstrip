#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/symlink_tracker.h"

static int tests_run = 0;
static int tests_passed = 0;

#define RUN_TEST(fn) do { \
    printf("  running %s ... ", #fn); \
    fn(); \
    tests_run++; \
    tests_passed++; \
    printf("ok\n"); \
} while (0)

static void test_init(void) {
    symlink_tracker_t t;
    symlink_tracker_init(&t);
    assert(t.count == 0);
    assert(t.dangling == 0);
    assert(t.loops == 0);
    assert(t.external == 0);
    assert(t.suspicious == 0);
}

static void test_classify_external(void) {
    char resolved[SYMLINK_PATH_MAX];
    symlink_status_t s = symlink_tracker_classify(
        "/etc/passwd", "../../../etc/shadow",
        "/tmp/fakeroot", resolved, sizeof(resolved));
    /* target escapes root — should be EXTERNAL */
    assert(s == SYMLINK_EXTERNAL);
}

static void test_classify_suspicious(void) {
    char resolved[SYMLINK_PATH_MAX];
    symlink_status_t s = symlink_tracker_classify(
        "/usr/bin/sh", "/etc/shadow",
        "/tmp/fakeroot", resolved, sizeof(resolved));
    assert(s == SYMLINK_SUSPICIOUS);
}

static void test_classify_ok(void) {
    char resolved[SYMLINK_PATH_MAX];
    symlink_status_t s = symlink_tracker_classify(
        "/usr/bin/python", "/usr/bin/python3",
        "/tmp/fakeroot", resolved, sizeof(resolved));
    /* no loop, no escape — should be OK or DANGLING (target may not exist) */
    assert(s == SYMLINK_OK || s == SYMLINK_DANGLING);
}

static void test_add_and_count(void) {
    symlink_tracker_t t;
    symlink_tracker_init(&t);
    int r = symlink_tracker_add(&t, "/bin/sh", "/bin/bash", "/tmp/fakeroot");
    assert(r == 0);
    assert(t.count == 1);
}

static void test_add_external_increments_counter(void) {
    symlink_tracker_t t;
    symlink_tracker_init(&t);
    symlink_tracker_add(&t, "/etc/resolv.conf",
                        "../../../../etc/resolv.conf", "/tmp/fakeroot");
    assert(t.external == 1);
}

static void test_add_suspicious_increments_counter(void) {
    symlink_tracker_t t;
    symlink_tracker_init(&t);
    symlink_tracker_add(&t, "/tmp/x", "/etc/shadow", "/tmp/fakeroot");
    assert(t.suspicious == 1);
}

int main(void) {
    printf("=== symlink_tracker tests ===\n");
    RUN_TEST(test_init);
    RUN_TEST(test_classify_external);
    RUN_TEST(test_classify_suspicious);
    RUN_TEST(test_classify_ok);
    RUN_TEST(test_add_and_count);
    RUN_TEST(test_add_external_increments_counter);
    RUN_TEST(test_add_suspicious_increments_counter);
    printf("\n%d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
