#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../src/hardlink_map.h"

static int callback_count = 0;

static void count_cb(const hardlink_entry_t *entry, void *ctx) {
    (void)ctx;
    (void)entry;
    callback_count++;
}

static void test_create_free(void) {
    hardlink_map_t *m = hardlink_map_create();
    assert(m != NULL);
    assert(hardlink_map_dup_count(m) == 0);
    hardlink_map_free(m);
    printf("  [PASS] create/free\n");
}

static void test_first_registration_returns_null(void) {
    hardlink_map_t *m = hardlink_map_create();
    const char *prev = hardlink_map_register(m, 100, 1, "/a/file");
    assert(prev == NULL);
    hardlink_map_free(m);
    printf("  [PASS] first registration returns NULL\n");
}

static void test_second_registration_returns_first_path(void) {
    hardlink_map_t *m = hardlink_map_create();
    hardlink_map_register(m, 200, 1, "/a/original");
    const char *prev = hardlink_map_register(m, 200, 1, "/b/link");
    assert(prev != NULL);
    assert(strcmp(prev, "/a/original") == 0);
    assert(hardlink_map_dup_count(m) == 1);
    hardlink_map_free(m);
    printf("  [PASS] second registration returns first path\n");
}

static void test_different_devices_not_confused(void) {
    hardlink_map_t *m = hardlink_map_create();
    hardlink_map_register(m, 300, 1, "/dev1/file");
    const char *prev = hardlink_map_register(m, 300, 2, "/dev2/file");
    assert(prev == NULL);  /* different device, not a hardlink */
    assert(hardlink_map_dup_count(m) == 0);
    hardlink_map_free(m);
    printf("  [PASS] different devices not confused\n");
}

static void test_lookup(void) {
    hardlink_map_t *m = hardlink_map_create();
    hardlink_map_register(m, 400, 1, "/some/path");
    const hardlink_entry_t *e = hardlink_map_lookup(m, 400, 1);
    assert(e != NULL);
    assert(e->inode == 400);
    assert(strcmp(e->first_path, "/some/path") == 0);
    assert(hardlink_map_lookup(m, 999, 1) == NULL);
    hardlink_map_free(m);
    printf("  [PASS] lookup\n");
}

static void test_foreach_dup(void) {
    hardlink_map_t *m = hardlink_map_create();
    hardlink_map_register(m, 500, 1, "/p1");
    hardlink_map_register(m, 500, 1, "/p2");
    hardlink_map_register(m, 501, 1, "/p3");  /* unique inode */
    callback_count = 0;
    hardlink_map_foreach_dup(m, count_cb, NULL);
    assert(callback_count == 1);
    hardlink_map_free(m);
    printf("  [PASS] foreach_dup only visits duplicates\n");
}

int main(void) {
    printf("Running hardlink_map tests...\n");
    test_create_free();
    test_first_registration_returns_null();
    test_second_registration_returns_first_path();
    test_different_devices_not_confused();
    test_lookup();
    test_foreach_dup();
    printf("All hardlink_map tests passed.\n");
    return 0;
}
