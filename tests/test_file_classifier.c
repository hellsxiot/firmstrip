#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "../src/file_classifier.h"

static void test_extension_script(void) {
    assert(classify_by_extension("run.sh") == FILE_TYPE_SCRIPT);
    assert(classify_by_extension("helper.py") == FILE_TYPE_SCRIPT);
    assert(classify_by_extension("main.pl") == FILE_TYPE_SCRIPT);
    printf("PASS: extension script\n");
}

static void test_extension_config(void) {
    assert(classify_by_extension("app.conf") == FILE_TYPE_CONFIG);
    assert(classify_by_extension("settings.json") == FILE_TYPE_CONFIG);
    assert(classify_by_extension("device.xml") == FILE_TYPE_CONFIG);
    printf("PASS: extension config\n");
}

static void test_extension_archive(void) {
    assert(classify_by_extension("fw.tar") == FILE_TYPE_ARCHIVE);
    assert(classify_by_extension("update.gz") == FILE_TYPE_ARCHIVE);
    assert(classify_by_extension("pkg.zip") == FILE_TYPE_ARCHIVE);
    printf("PASS: extension archive\n");
}

static void test_extension_unknown(void) {
    assert(classify_by_extension("noext") == FILE_TYPE_UNKNOWN);
    assert(classify_by_extension(".weird") == FILE_TYPE_UNKNOWN);
    printf("PASS: extension unknown\n");
}

static void test_magic_elf(void) {
    uint8_t elf[] = {0x7f, 'E', 'L', 'F', 0x02, 0x01};
    assert(classify_by_magic(elf, sizeof(elf)) == FILE_TYPE_BINARY);
    printf("PASS: magic ELF\n");
}

static void test_magic_gzip(void) {
    uint8_t gz[] = {0x1f, 0x8b, 0x08, 0x00};
    assert(classify_by_magic(gz, sizeof(gz)) == FILE_TYPE_ARCHIVE);
    printf("PASS: magic gzip\n");
}

static void test_magic_shebang(void) {
    uint8_t sh[] = {'#', '!', '/', 'b', 'i', 'n'};
    assert(classify_by_magic(sh, sizeof(sh)) == FILE_TYPE_SCRIPT);
    printf("PASS: magic shebang\n");
}

static void test_classify_file_magic_wins(void) {
    /* .conf extension but ELF magic — magic should win */
    uint8_t elf[] = {0x7f, 'E', 'L', 'F', 0x00};
    FileType t = classify_file("weird.conf", elf, sizeof(elf));
    assert(t == FILE_TYPE_BINARY);
    printf("PASS: classify_file magic wins\n");
}

static void test_classify_file_fallback(void) {
    uint8_t plain[] = {0x00, 0x01, 0x02};
    FileType t = classify_file("config.json", plain, sizeof(plain));
    assert(t == FILE_TYPE_CONFIG);
    printf("PASS: classify_file fallback to extension\n");
}

static void test_type_name(void) {
    assert(strcmp(file_type_name(FILE_TYPE_BINARY), "binary") == 0);
    assert(strcmp(file_type_name(FILE_TYPE_SCRIPT), "script") == 0);
    assert(strcmp(file_type_name(FILE_TYPE_UNKNOWN), "unknown") == 0);
    printf("PASS: type names\n");
}

int main(void) {
    test_extension_script();
    test_extension_config();
    test_extension_archive();
    test_extension_unknown();
    test_magic_elf();
    test_magic_gzip();
    test_magic_shebang();
    test_classify_file_magic_wins();
    test_classify_file_fallback();
    test_type_name();
    printf("All file_classifier tests passed.\n");
    return 0;
}
