#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../src/owner_info.h"

static int tests_run = 0;
static int tests_passed = 0;

#define RUN_TEST(fn) do { \
    tests_run++; \
    if (fn()) { tests_passed++; printf("PASS: %s\n", #fn); } \
    else { printf("FAIL: %s\n", #fn); } \
} while (0)

static int test_create_free(void) {
    OwnerInfo *oi = owner_info_create();
    if (!oi) return 0;
    owner_info_free(oi);
    return 1;
}

static int test_add_and_count(void) {
    OwnerInfo *oi = owner_info_create();
    if (!oi) return 0;
    int r1 = owner_info_add(oi, "/etc/passwd", 0, 0);
    int r2 = owner_info_add(oi, "/home/user/file", 1000, 1000);
    int r3 = owner_info_add(oi, "/var/log/syslog", 0, 4);
    int ok = (r1 == 0 && r2 == 0 && r3 == 0 && oi->count == 3);
    owner_info_free(oi);
    return ok;
}

static int test_find_by_uid(void) {
    OwnerInfo *oi = owner_info_create();
    if (!oi) return 0;
    owner_info_add(oi, "/bin/sh",    0,    0);
    owner_info_add(oi, "/etc/hosts", 0,    0);
    owner_info_add(oi, "/tmp/x",     1001, 1001);

    size_t cnt = 0;
    OwnerEntry **found = owner_info_find_by_uid(oi, 0, &cnt);
    int ok = (found != NULL && cnt == 2);
    free(found);

    size_t cnt2 = 0;
    OwnerEntry **none = owner_info_find_by_uid(oi, 999, &cnt2);
    ok = ok && (none == NULL && cnt2 == 0);

    owner_info_free(oi);
    return ok;
}

static int test_find_by_gid(void) {
    OwnerInfo *oi = owner_info_create();
    if (!oi) return 0;
    owner_info_add(oi, "/dev/tty",   0, 5);
    owner_info_add(oi, "/dev/null",  0, 5);
    owner_info_add(oi, "/usr/local", 0, 0);

    size_t cnt = 0;
    OwnerEntry **found = owner_info_find_by_gid(oi, 5, &cnt);
    int ok = (found != NULL && cnt == 2);
    free(found);
    owner_info_free(oi);
    return ok;
}

static int test_path_stored_correctly(void) {
    OwnerInfo *oi = owner_info_create();
    if (!oi) return 0;
    owner_info_add(oi, "/etc/shadow", 0, 42);
    int ok = (strcmp(oi->entries[0].path, "/etc/shadow") == 0 &&
              oi->entries[0].uid == 0 &&
              oi->entries[0].gid == 42);
    owner_info_free(oi);
    return ok;
}

int main(void) {
    RUN_TEST(test_create_free);
    RUN_TEST(test_add_and_count);
    RUN_TEST(test_find_by_uid);
    RUN_TEST(test_find_by_gid);
    RUN_TEST(test_path_stored_correctly);

    printf("\n%d/%d tests passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
