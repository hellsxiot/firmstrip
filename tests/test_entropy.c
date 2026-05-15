#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "../src/entropy.h"

static int tests_run = 0;
static int tests_passed = 0;

#define CHECK(cond, msg) do { \
    tests_run++; \
    if (cond) { tests_passed++; printf("  PASS: %s\n", msg); } \
    else { printf("  FAIL: %s\n", msg); } \
} while(0)

static void test_zero_entropy(void)
{
    uint8_t buf[256];
    memset(buf, 0xAA, sizeof(buf));
    entropy_result_t r;
    int rc = entropy_calc_buffer(buf, sizeof(buf), &r);
    CHECK(rc == 0, "zero-entropy: return code");
    CHECK(fabs(r.entropy) < 0.001, "zero-entropy: value near 0");
    CHECK(r.is_compressed == 0, "zero-entropy: not compressed");
    CHECK(r.is_encrypted  == 0, "zero-entropy: not encrypted");
}

static void test_max_entropy(void)
{
    uint8_t buf[256];
    for (int i = 0; i < 256; i++)
        buf[i] = (uint8_t)i;
    entropy_result_t r;
    int rc = entropy_calc_buffer(buf, sizeof(buf), &r);
    CHECK(rc == 0, "max-entropy: return code");
    CHECK(fabs(r.entropy - 8.0) < 0.001, "max-entropy: value near 8.0");
    CHECK(r.is_encrypted == 1, "max-entropy: flagged encrypted");
}

static void test_invalid_args(void)
{
    entropy_result_t r;
    CHECK(entropy_calc_buffer(NULL, 10, &r) == -1, "null buf returns -1");
    CHECK(entropy_calc_buffer((uint8_t *)"x", 0, &r) == -1, "zero len returns -1");
    CHECK(entropy_calc_file(NULL, &r) == -1, "null path returns -1");
    CHECK(entropy_calc_file("/nonexistent_file_xyz", &r) == -1, "missing file returns -1");
}

static void test_label(void)
{
    entropy_result_t r = {0};
    r.entropy = 0.5; r.is_compressed = 0; r.is_encrypted = 0;
    CHECK(strcmp(entropy_label(&r), "plain/sparse") == 0, "label: plain/sparse");
    r.entropy = 7.0; r.is_compressed = 1; r.is_encrypted = 0;
    CHECK(strcmp(entropy_label(&r), "compressed") == 0, "label: compressed");
    r.entropy = 7.9; r.is_compressed = 1; r.is_encrypted = 1;
    CHECK(strcmp(entropy_label(&r), "encrypted/random") == 0, "label: encrypted/random");
    CHECK(entropy_label(NULL) != NULL, "label: null safe");
}

int main(void)
{
    printf("=== entropy tests ===\n");
    test_zero_entropy();
    test_max_entropy();
    test_invalid_args();
    test_label();
    printf("\n%d/%d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
