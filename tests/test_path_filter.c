#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "../src/path_filter.h"

static int passed = 0;
static int failed = 0;

#define TEST(name) printf("  test: %s\n", name)
#define ASSERT(cond) do { if (cond) { passed++; } else { failed++; printf("    FAIL at line %d\n", __LINE__); } } while(0)

void test_create_free() {
    TEST("create and free include filter");
    PathFilter *pf = path_filter_create(FILTER_INCLUDE);
    ASSERT(pf != NULL);
    ASSERT(path_filter_count(pf) == 0);
    path_filter_free(pf);
}

void test_add_patterns() {
    TEST("add patterns to filter");
    PathFilter *pf = path_filter_create(FILTER_INCLUDE);
    ASSERT(path_filter_add(pf, "*.conf") == 0);
    ASSERT(path_filter_add(pf, "/etc/*") == 0);
    ASSERT(path_filter_count(pf) == 2);
    path_filter_free(pf);
}

void test_include_match() {
    TEST("include filter matches correctly");
    PathFilter *pf = path_filter_create(FILTER_INCLUDE);
    path_filter_add(pf, "*.conf");
    path_filter_add(pf, "*.cfg");
    ASSERT(path_filter_match(pf, "system.conf") == true);
    ASSERT(path_filter_match(pf, "app.cfg") == true);
    ASSERT(path_filter_match(pf, "readme.txt") == false);
    path_filter_free(pf);
}

void test_exclude_match() {
    TEST("exclude filter matches correctly");
    PathFilter *pf = path_filter_create(FILTER_EXCLUDE);
    path_filter_add(pf, "*.log");
    path_filter_add(pf, "*.tmp");
    ASSERT(path_filter_match(pf, "system.log") == false);
    ASSERT(path_filter_match(pf, "cache.tmp") == false);
    ASSERT(path_filter_match(pf, "binary.bin") == true);
    path_filter_free(pf);
}

void test_empty_include_filter() {
    TEST("empty include filter allows all");
    PathFilter *pf = path_filter_create(FILTER_INCLUDE);
    ASSERT(path_filter_match(pf, "anything") == true);
    path_filter_free(pf);
}

void test_empty_exclude_filter() {
    TEST("empty exclude filter blocks nothing");
    PathFilter *pf = path_filter_create(FILTER_EXCLUDE);
    ASSERT(path_filter_match(pf, "anything") == false);
    path_filter_free(pf);
}

void test_null_safety() {
    TEST("null pointer safety");
    ASSERT(path_filter_match(NULL, "path") == false);
    ASSERT(path_filter_add(NULL, "*.c") == -1);
    ASSERT(path_filter_count(NULL) == 0);
    path_filter_free(NULL);
}

int main() {
    printf("path_filter tests\n");
    test_create_free();
    test_add_patterns();
    test_include_match();
    test_exclude_match();
    test_empty_include_filter();
    test_empty_exclude_filter();
    test_null_safety();
    printf("results: %d passed, %d failed\n", passed, failed);
    return failed > 0 ? 1 : 0;
}
