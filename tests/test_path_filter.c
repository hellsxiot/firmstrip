#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../src/path_filter.h"

static int passed = 0;
static int failed = 0;

#define CHECK(cond, desc) do { \
    if (cond) { printf("  PASS: %s\n", desc); passed++; } \
    else { printf("  FAIL: %s\n", desc); failed++; } \
} while (0)

static void test_init(void) {
    printf("test_init\n");
    path_filter_t f;
    int r = path_filter_init(&f, FILTER_MODE_INCLUDE);
    CHECK(r == 0, "init returns 0");
    CHECK(f.count == 0, "count starts at 0");
    CHECK(f.mode == FILTER_MODE_INCLUDE, "mode set correctly");
    path_filter_free(&f);
}

static void test_add_and_match_include(void) {
    printf("test_add_and_match_include\n");
    path_filter_t f;
    path_filter_init(&f, FILTER_MODE_INCLUDE);

    int r = path_filter_add(&f, "*.conf");
    CHECK(r == 0, "add pattern succeeds");
    CHECK(f.count == 1, "count is 1 after add");

    CHECK(path_filter_match(&f, "/etc/network.conf") == 1, "*.conf matches .conf file");
    CHECK(path_filter_match(&f, "/bin/busybox") == 0, "*.conf does not match binary");
    CHECK(path_filter_match(&f, "/etc/passwd") == 0, "*.conf does not match passwd");

    path_filter_free(&f);
}

static void test_add_and_match_exclude(void) {
    printf("test_add_and_match_exclude\n");
    path_filter_t f;
    path_filter_init(&f, FILTER_MODE_EXCLUDE);

    path_filter_add(&f, "/proc/*");
    path_filter_add(&f, "/sys/*");

    CHECK(path_filter_match(&f, "/proc/cpuinfo") == 0, "/proc/* excluded");
    CHECK(path_filter_match(&f, "/sys/kernel/mm") == 0, "/sys/* excluded");
    CHECK(path_filter_match(&f, "/etc/passwd") == 1, "/etc/passwd not excluded");

    path_filter_free(&f);
}

static void test_empty_include_filter(void) {
    printf("test_empty_include_filter\n");
    path_filter_t f;
    path_filter_init(&f, FILTER_MODE_INCLUDE);
    /* no patterns: include mode with no patterns passes everything */
    CHECK(path_filter_match(&f, "/any/path") == 1, "empty include passes all");
    path_filter_free(&f);
}

static void test_multiple_patterns(void) {
    printf("test_multiple_patterns\n");
    path_filter_t f;
    path_filter_init(&f, FILTER_MODE_INCLUDE);
    path_filter_add(&f, "*.sh");
    path_filter_add(&f, "*.py");
    path_filter_add(&f, "*.conf");

    CHECK(path_filter_match(&f, "/usr/bin/setup.sh") == 1, "*.sh matches");
    CHECK(path_filter_match(&f, "/opt/app/run.py") == 1, "*.py matches");
    CHECK(path_filter_match(&f, "/etc/app.conf") == 1, "*.conf matches");
    CHECK(path_filter_match(&f, "/bin/ls") == 0, "binary does not match");

    path_filter_free(&f);
}

int main(void) {
    printf("=== path_filter tests ===\n");
    test_init();
    test_add_and_match_include();
    test_add_and_match_exclude();
    test_empty_include_filter();
    test_multiple_patterns();
    printf("\nResults: %d passed, %d failed\n", passed, failed);
    return failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
