#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include "../src/suid_scanner.h"

static void test_init_free(void) {
    SuidResult r;
    assert(suid_result_init(&r) == 0);
    assert(r.entries != NULL);
    assert(r.count == 0);
    suid_result_free(&r);
    assert(r.entries == NULL);
    printf("[PASS] test_init_free\n");
}

static void test_no_special_bits(void) {
    SuidResult r;
    suid_result_init(&r);
    struct stat st = {0};
    st.st_mode = S_IFREG | 0755;
    assert(suid_scan_file(&r, "/bin/ls", &st) == 0);
    assert(r.count == 0);
    suid_result_free(&r);
    printf("[PASS] test_no_special_bits\n");
}

static void test_suid_detected(void) {
    SuidResult r;
    suid_result_init(&r);
    struct stat st = {0};
    st.st_mode = S_IFREG | S_ISUID | 0755;
    st.st_uid = 0;
    st.st_gid = 0;
    assert(suid_scan_file(&r, "/usr/bin/sudo", &st) == 0);
    assert(r.count == 1);
    assert(r.entries[0].flags & SUID_FLAG_SUID);
    assert(strcmp(r.entries[0].path, "/usr/bin/sudo") == 0);
    suid_result_free(&r);
    printf("[PASS] test_suid_detected\n");
}

static void test_sgid_detected(void) {
    SuidResult r;
    suid_result_init(&r);
    struct stat st = {0};
    st.st_mode = S_IFREG | S_ISGID | 0755;
    assert(suid_scan_file(&r, "/usr/bin/wall", &st) == 0);
    assert(r.count == 1);
    assert(r.entries[0].flags & SUID_FLAG_SGID);
    suid_result_free(&r);
    printf("[PASS] test_sgid_detected\n");
}

static void test_sticky_detected(void) {
    SuidResult r;
    suid_result_init(&r);
    struct stat st = {0};
    st.st_mode = S_IFDIR | S_ISVTX | 0777;
    assert(suid_scan_file(&r, "/tmp", &st) == 0);
    assert(r.count == 1);
    assert(r.entries[0].flags & SUID_FLAG_STICKY);
    suid_result_free(&r);
    printf("[PASS] test_sticky_detected\n");
}

static void test_flags_str(void) {
    assert(strcmp(suid_flags_str(SUID_FLAG_SUID), "suid") == 0);
    assert(strcmp(suid_flags_str(SUID_FLAG_SGID), "sgid") == 0);
    assert(strcmp(suid_flags_str(SUID_FLAG_STICKY), "sticky") == 0);
    assert(strcmp(suid_flags_str(SUID_FLAG_SUID | SUID_FLAG_SGID), "suid+sgid") == 0);
    assert(strcmp(suid_flags_str(0), "none") == 0);
    printf("[PASS] test_flags_str\n");
}

static void test_null_inputs(void) {
    SuidResult r;
    suid_result_init(&r);
    struct stat st = {0};
    assert(suid_scan_file(NULL, "/foo", &st) == -1);
    assert(suid_scan_file(&r, NULL, &st) == -1);
    assert(suid_scan_file(&r, "/foo", NULL) == -1);
    suid_result_free(&r);
    printf("[PASS] test_null_inputs\n");
}

int main(void) {
    test_init_free();
    test_no_special_bits();
    test_suid_detected();
    test_sgid_detected();
    test_sticky_detected();
    test_flags_str();
    test_null_inputs();
    printf("All suid_scanner tests passed.\n");
    return 0;
}
