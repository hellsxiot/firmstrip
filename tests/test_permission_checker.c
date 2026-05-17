#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>

#include "../src/permission_checker.h"

static int tests_run = 0;
static int tests_passed = 0;

#define RUN_TEST(fn) do { \
    tests_run++; \
    if (fn()) { tests_passed++; printf("PASS: %s\n", #fn); } \
    else { printf("FAIL: %s\n", #fn); } \
} while (0)

static int test_world_writable(void) {
    uint32_t flags = perm_check_file("/etc/passwd", 0666);
    return (flags & PERM_WORLD_WRITABLE) != 0;
}

static int test_suid_detected(void) {
    uint32_t flags = perm_check_file("/bin/su", S_ISUID | 0755);
    return (flags & PERM_SUID_SET) != 0;
}

static int test_sgid_detected(void) {
    uint32_t flags = perm_check_file("/usr/bin/wall", S_ISGID | 0755);
    return (flags & PERM_SGID_SET) != 0;
}

static int test_clean_file(void) {
    uint32_t flags = perm_check_file("/etc/hosts", 0644);
    return flags == 0;
}

static int test_no_owner_read(void) {
    uint32_t flags = perm_check_file("/some/file", 0044);
    return (flags & PERM_NO_OWNER_READ) != 0;
}

static int test_report_init_and_add(void) {
    perm_report_t report;
    perm_report_init(&report);
    assert(report.count == 0);

    int r = perm_report_add(&report, "/bin/busybox", S_ISUID | 0755, PERM_SUID_SET);
    assert(r == 0);
    assert(report.count == 1);
    assert(strcmp(report.entries[0].path, "/bin/busybox") == 0);
    assert(report.entries[0].flags & PERM_SUID_SET);

    perm_report_free(&report);
    return 1;
}

static int test_report_multiple_entries(void) {
    perm_report_t report;
    perm_report_init(&report);

    for (int i = 0; i < 10; i++) {
        char path[64];
        snprintf(path, sizeof(path), "/tmp/file%d", i);
        perm_report_add(&report, path, 0666, PERM_WORLD_WRITABLE);
    }
    assert(report.count == 10);
    perm_report_free(&report);
    return 1;
}

static int test_combined_flags(void) {
    /* world-writable + suid is particularly bad */
    uint32_t flags = perm_check_file("/danger", S_ISUID | 0777);
    return (flags & PERM_WORLD_WRITABLE) && (flags & PERM_SUID_SET);
}

int main(void) {
    RUN_TEST(test_world_writable);
    RUN_TEST(test_suid_detected);
    RUN_TEST(test_sgid_detected);
    RUN_TEST(test_clean_file);
    RUN_TEST(test_no_owner_read);
    RUN_TEST(test_report_init_and_add);
    RUN_TEST(test_report_multiple_entries);
    RUN_TEST(test_combined_flags);

    printf("\nResults: %d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
