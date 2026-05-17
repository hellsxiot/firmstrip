#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "../src/magic_bytes.h"

static int tests_run = 0;
static int tests_passed = 0;

#define RUN_TEST(fn) do { tests_run++; fn(); tests_passed++; } while(0)

static void test_elf_detection(void) {
    uint8_t elf[] = {0x7f, 'E', 'L', 'F', 0x02, 0x01};
    MagicResult r = magic_bytes_identify(elf, sizeof(elf));
    assert(r.type == MAGIC_ELF);
    assert(r.confidence > 0);
    printf("  PASS: ELF detection\n");
}

static void test_gzip_detection(void) {
    uint8_t gz[] = {0x1f, 0x8b, 0x08, 0x00};
    MagicResult r = magic_bytes_identify(gz, sizeof(gz));
    assert(r.type == MAGIC_GZIP);
    printf("  PASS: GZIP detection\n");
}

static void test_squashfs_detection(void) {
    uint8_t sq[] = {'s', 'q', 's', 'h', 0x00, 0x00};
    MagicResult r = magic_bytes_identify(sq, sizeof(sq));
    assert(r.type == MAGIC_SQUASHFS);
    printf("  PASS: SquashFS detection\n");
}

static void test_png_detection(void) {
    uint8_t png[] = {0x89,'P','N','G',0x0d,0x0a,0x1a,0x0a,0x00};
    MagicResult r = magic_bytes_identify(png, sizeof(png));
    assert(r.type == MAGIC_PNG);
    printf("  PASS: PNG detection\n");
}

static void test_unknown_detection(void) {
    uint8_t unknown[] = {0x00, 0x01, 0x02, 0x03};
    MagicResult r = magic_bytes_identify(unknown, sizeof(unknown));
    assert(r.type == MAGIC_UNKNOWN);
    printf("  PASS: Unknown type detection\n");
}

static void test_empty_buffer(void) {
    MagicResult r = magic_bytes_identify(NULL, 0);
    assert(r.type == MAGIC_UNKNOWN);
    printf("  PASS: NULL/empty buffer handled\n");
}

static void test_too_short_buffer(void) {
    /* only 1 byte — should not match ELF (needs 4) */
    uint8_t buf[] = {0x7f};
    MagicResult r = magic_bytes_identify(buf, sizeof(buf));
    assert(r.type == MAGIC_UNKNOWN);
    printf("  PASS: Too-short buffer handled\n");
}

static void test_type_name(void) {
    assert(strcmp(magic_bytes_type_name(MAGIC_ELF), "ELF Binary") == 0);
    assert(strcmp(magic_bytes_type_name(MAGIC_UNKNOWN), "Unknown") == 0);
    printf("  PASS: type name lookup\n");
}

static void test_xz_detection(void) {
    uint8_t xz[] = {0xfd,'7','z','X','Z',0x00,0x00};
    MagicResult r = magic_bytes_identify(xz, sizeof(xz));
    assert(r.type == MAGIC_XZ);
    printf("  PASS: XZ detection\n");
}

int main(void) {
    printf("=== test_magic_bytes ===\n");
    magic_bytes_init();

    RUN_TEST(test_elf_detection);
    RUN_TEST(test_gzip_detection);
    RUN_TEST(test_squashfs_detection);
    RUN_TEST(test_png_detection);
    RUN_TEST(test_unknown_detection);
    RUN_TEST(test_empty_buffer);
    RUN_TEST(test_too_short_buffer);
    RUN_TEST(test_type_name);
    RUN_TEST(test_xz_detection);

    printf("\nResults: %d/%d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
