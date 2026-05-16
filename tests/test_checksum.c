#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../src/checksum.h"

static void test_md5_known(void) {
    /* MD5 of empty string: d41d8cd98f00b204e9800998ecf8427e */
    checksum_result_t r;
    int rc = checksum_buf((const uint8_t *)"", 0, CKSUM_MD5, &r);
    assert(rc == 0);
    assert(strcmp(r.hex, "d41d8cd98f00b204e9800998ecf8427e") == 0);
    assert(r.raw_len == 16);
    printf("[PASS] md5 empty string\n");
}

static void test_sha256_known(void) {
    /* SHA256 of empty string */
    const char *expected =
        "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
    checksum_result_t r;
    int rc = checksum_buf((const uint8_t *)"", 0, CKSUM_SHA256, &r);
    assert(rc == 0);
    assert(strcmp(r.hex, expected) == 0);
    assert(r.raw_len == 32);
    printf("[PASS] sha256 empty string\n");
}

static void test_crc32_known(void) {
    /* CRC32 of "123456789" = 0xcbf43926 */
    const uint8_t data[] = "123456789";
    checksum_result_t r;
    int rc = checksum_buf(data, 9, CKSUM_CRC32, &r);
    assert(rc == 0);
    assert(strcmp(r.hex, "cbf43926") == 0);
    assert(r.raw_len == 4);
    printf("[PASS] crc32 '123456789'\n");
}

static void test_sha1_known(void) {
    /* SHA1 of empty string: da39a3ee5e6b4b0d3255bfef95601890afd80709 */
    checksum_result_t r;
    int rc = checksum_buf((const uint8_t *)"", 0, CKSUM_SHA1, &r);
    assert(rc == 0);
    assert(strcmp(r.hex, "da39a3ee5e6b4b0d3255bfef95601890afd80709") == 0);
    assert(r.raw_len == 20);
    printf("[PASS] sha1 empty string\n");
}

static void test_algo_name(void) {
    assert(strcmp(checksum_algo_name(CKSUM_MD5),    "md5")    == 0);
    assert(strcmp(checksum_algo_name(CKSUM_SHA1),   "sha1")   == 0);
    assert(strcmp(checksum_algo_name(CKSUM_SHA256), "sha256") == 0);
    assert(strcmp(checksum_algo_name(CKSUM_CRC32),  "crc32")  == 0);
    printf("[PASS] algo name strings\n");
}

static void test_null_buf(void) {
    checksum_result_t r;
    int rc = checksum_buf(NULL, 0, CKSUM_MD5, &r);
    assert(rc == -1);
    printf("[PASS] null buf returns error\n");
}

static void test_file_checksum(void) {
    /* Write a temp file and verify checksum matches buf version */
    const char *path = "/tmp/firmstrip_cksum_test.bin";
    const uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x01, 0x02, 0x03};

    FILE *f = fopen(path, "wb");
    assert(f != NULL);
    fwrite(data, 1, sizeof(data), f);
    fclose(f);

    checksum_result_t from_buf, from_file;
    assert(checksum_buf(data, sizeof(data), CKSUM_SHA256, &from_buf)  == 0);
    assert(checksum_file(path, CKSUM_SHA256, &from_file) == 0);
    assert(strcmp(from_buf.hex, from_file.hex) == 0);
    printf("[PASS] file checksum matches buf checksum\n");
}

int main(void) {
    printf("=== checksum tests ===\n");
    test_md5_known();
    test_sha1_known();
    test_sha256_known();
    test_crc32_known();
    test_algo_name();
    test_null_buf();
    test_file_checksum();
    printf("all checksum tests passed\n");
    return 0;
}
