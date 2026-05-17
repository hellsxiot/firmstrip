#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "../src/owner_info.h"

static int tests_run = 0;
static int tests_passed = 0;

#define ASSERT(cond, msg) do { \
    tests_run++; \
    if (cond) { tests_passed++; printf("  PASS: %s\n", msg); } \
    else { printf("  FAIL: %s\n", msg); } \
} while(0)

static void test_basic_owner(void) {
    printf("test_basic_owner:\n");
    const char *path = "/tmp";
    OwnerInfo info;
    int ret = owner_info_get(path, &info);
    ASSERT(ret == 0, "owner_info_get returns 0 for /tmp");
    ASSERT(info.uid >= 0, "uid is non-negative");
    ASSERT(strlen(info.username) > 0, "username is non-empty");
    ASSERT(strlen(info.groupname) > 0, "groupname is non-empty");
}

static void test_invalid_path(void) {
    printf("test_invalid_path:\n");
    OwnerInfo info;
    int ret = owner_info_get("/nonexistent_firmstrip_path_xyz", &info);
    ASSERT(ret != 0, "returns error for nonexistent path");
}

static void test_null_args(void) {
    printf("test_null_args:\n");
    OwnerInfo info;
    ASSERT(owner_info_get(NULL, &info) != 0, "NULL path returns error");
    ASSERT(owner_info_get("/tmp", NULL) != 0, "NULL out returns error");
}

static void test_world_writable(void) {
    printf("test_world_writable:\n");
    const char *tmpfile = "/tmp/firmstrip_owner_test.txt";
    int fd = open(tmpfile, O_CREAT | O_WRONLY, 0666);
    if (fd < 0) { printf("  SKIP: cannot create temp file\n"); return; }
    close(fd);
    chmod(tmpfile, 0666);

    OwnerInfo info;
    int ret = owner_info_get(tmpfile, &info);
    ASSERT(ret == 0, "owner_info_get succeeds on temp file");
    ASSERT(info.is_world_writable == 1, "world-writable flag set for 0666 file");
    ASSERT(owner_info_is_suspicious(&info) == 1, "suspicious for world-writable");
    unlink(tmpfile);
}

static void test_not_suspicious(void) {
    printf("test_not_suspicious:\n");
    const char *tmpfile = "/tmp/firmstrip_owner_test2.txt";
    int fd = open(tmpfile, O_CREAT | O_WRONLY, 0644);
    if (fd < 0) { printf("  SKIP: cannot create temp file\n"); return; }
    close(fd);
    chmod(tmpfile, 0644);

    OwnerInfo info;
    owner_info_get(tmpfile, &info);
    ASSERT(info.is_world_writable == 0, "not world-writable for 0644");
    ASSERT(info.is_setuid == 0, "no setuid for regular file");
    ASSERT(info.is_setgid == 0, "no setgid for regular file");
    unlink(tmpfile);
}

int main(void) {
    printf("Running owner_info tests...\n\n");
    test_basic_owner();
    test_invalid_path();
    test_null_args();
    test_world_writable();
    test_not_suspicious();
    printf("\nResults: %d/%d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
