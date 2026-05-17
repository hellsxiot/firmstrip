#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include "../src/permission_checker.h"

static int tests_run = 0;
static int tests_passed = 0;

#define RUN_TEST(fn) do { tests_run++; fn(); tests_passed++; } while (0)

static void test_normal_file(void) {
    PermissionResult r = permission_check("/etc/passwd", 0644);
    assert(strcmp(r.path, "/etc/passwd") == 0);
    assert(r.flags == PERM_OK);
    assert(!permission_is_suspicious(&r));
    printf("  [ok] normal file\n");
}

static void test_suid_detected(void) {
    PermissionResult r = permission_check("/bin/su", S_ISUID | 0755);
    assert(r.flags & PERM_SUID);
    assert(permission_is_suspicious(&r));
    assert(strstr(r.description, "setuid") != NULL);
    printf("  [ok] suid detected\n");
}

static void test_sgid_detected(void) {
    PermissionResult r = permission_check("/usr/bin/wall", S_ISGID | 0755);
    assert(r.flags & PERM_SGID);
    assert(permission_is_suspicious(&r));
    printf("  [ok] sgid detected\n");
}

static void test_world_writable(void) {
    PermissionResult r = permission_check("/tmp/bad", 0777);
    assert(r.flags & PERM_WORLD_WRITE);
    assert(permission_is_suspicious(&r));
    assert(strstr(r.description, "world-writable") != NULL);
    printf("  [ok] world-writable detected\n");
}

static void test_sticky_bit(void) {
    PermissionResult r = permission_check("/tmp", S_ISVTX | 0755);
    assert(r.flags & PERM_STICKY);
    assert(!permission_is_suspicious(&r));
    printf("  [ok] sticky bit flagged but not suspicious\n");
}

static void test_multiple_flags(void) {
    PermissionResult r = permission_check("/weird", S_ISUID | 0002);
    assert(r.flags & PERM_SUID);
    assert(r.flags & PERM_WORLD_WRITE);
    assert(permission_is_suspicious(&r));
    assert(strstr(r.description, "setuid") != NULL);
    assert(strstr(r.description, "world-writable") != NULL);
    printf("  [ok] multiple flags combined\n");
}

static void test_null_path(void) {
    PermissionResult r = permission_check(NULL, 0644);
    assert(r.flags == PERM_ERROR);
    printf("  [ok] null path returns error\n");
}

static void test_print_does_not_crash(void) {
    PermissionResult r = permission_check("/bin/sh", S_ISUID | 0755);
    permission_result_print(&r, stdout);
    permission_result_print(NULL, stdout);
    printf("  [ok] print handles null gracefully\n");
}

int main(void) {
    printf("=== permission_checker tests ===\n");
    RUN_TEST(test_normal_file);
    RUN_TEST(test_suid_detected);
    RUN_TEST(test_sgid_detected);
    RUN_TEST(test_world_writable);
    RUN_TEST(test_sticky_bit);
    RUN_TEST(test_multiple_flags);
    RUN_TEST(test_null_path);
    RUN_TEST(test_print_does_not_crash);
    printf("=== %d/%d passed ===\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
