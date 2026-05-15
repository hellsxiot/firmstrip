#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "../src/file_classifier.h"

static int tests_run = 0;
static int tests_passed = 0;

#define CHECK(cond, msg) do { \
    tests_run++; \
    if (cond) { tests_passed++; printf("  PASS: %s\n", msg); } \
    else { printf("  FAIL: %s\n", msg); } \
} while(0)

static void test_elf_detection(void) {
    printf("[elf detection]\n");
    uint8_t elf_magic[] = { 0x7f, 'E', 'L', 'F', 0x02, 0x01 };
    FileClassification c = classify_file("/bin/busybox", elf_magic, sizeof(elf_magic));
    CHECK(c.type == FTYPE_ELF_BINARY, "ELF magic recognized");
    CHECK(c.is_executable == 1, "ELF marked executable");
    CHECK(c.is_sensitive == 0, "ELF not sensitive");
}

static void test_private_key_detection(void) {
    printf("[private key detection]\n");
    const uint8_t pem[] = "-----BEGIN RSA PRIVATE KEY-----\nMIIE...";
    FileClassification c = classify_file("/etc/ssl/private/server.key", pem, sizeof(pem));
    CHECK(c.type == FTYPE_PRIVATE_KEY, "PEM private key recognized");
    CHECK(c.is_sensitive == 1, "private key marked sensitive");
}

static void test_shell_script_by_path(void) {
    printf("[shell script by path]\n");
    const uint8_t shebang[] = "#!/bin/sh\necho hello\n";
    FileClassification c = classify_file("/etc/init.d/rcS", shebang, sizeof(shebang));
    CHECK(c.type == FTYPE_SHELL_SCRIPT, "shell script recognized by shebang");
    CHECK(c.is_executable == 1, "shell script marked executable");
}

static void test_config_by_extension(void) {
    printf("[config by extension]\n");
    FileClassification c = classify_file("/etc/network/interfaces", NULL, 0);
    CHECK(c.type == FTYPE_CONFIG || c.type == FTYPE_TEXT || c.type == FTYPE_UNKNOWN,
          "config file returns reasonable type");
}

static void test_unknown_fallback(void) {
    printf("[unknown fallback]\n");
    uint8_t random_bytes[] = { 0xDE, 0xAD, 0xBE, 0xEF };
    FileClassification c = classify_file("/var/data.bin", random_bytes, sizeof(random_bytes));
    CHECK(c.type == FTYPE_DATA || c.type == FTYPE_UNKNOWN, "binary data gets data/unknown type");
    CHECK(c.is_sensitive == 0, "random data not flagged sensitive");
}

static void test_type_name(void) {
    printf("[type name strings]\n");
    CHECK(strcmp(file_type_name(FTYPE_ELF_BINARY), "elf-binary") == 0, "elf-binary name");
    CHECK(strcmp(file_type_name(FTYPE_PRIVATE_KEY), "private-key") == 0, "private-key name");
    CHECK(strcmp(file_type_name(FTYPE_UNKNOWN), "unknown") == 0, "unknown name");
}

int main(void) {
    printf("=== test_file_classifier ===\n");
    test_elf_detection();
    test_private_key_detection();
    test_shell_script_by_path();
    test_config_by_extension();
    test_unknown_fallback();
    test_type_name();
    printf("\nResults: %d/%d passed\n", tests_passed, tests_run);
    return (tests_passed == tests_run) ? 0 : 1;
}
