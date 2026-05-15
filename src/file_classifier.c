#include "file_classifier.h"
#include <string.h>
#include <stdio.h>

static const struct {
    const char *ext;
    FileType type;
} ext_map[] = {
    { ".sh",   FILE_TYPE_SCRIPT },
    { ".py",   FILE_TYPE_SCRIPT },
    { ".pl",   FILE_TYPE_SCRIPT },
    { ".conf", FILE_TYPE_CONFIG },
    { ".cfg",  FILE_TYPE_CONFIG },
    { ".ini",  FILE_TYPE_CONFIG },
    { ".xml",  FILE_TYPE_CONFIG },
    { ".json", FILE_TYPE_CONFIG },
    { ".so",   FILE_TYPE_LIBRARY },
    { ".a",    FILE_TYPE_LIBRARY },
    { ".ko",   FILE_TYPE_KERNEL_MOD },
    { ".gz",   FILE_TYPE_ARCHIVE },
    { ".tar",  FILE_TYPE_ARCHIVE },
    { ".zip",  FILE_TYPE_ARCHIVE },
    { ".bin",  FILE_TYPE_BINARY },
    { ".elf",  FILE_TYPE_BINARY },
    { NULL,    FILE_TYPE_UNKNOWN }
};

static const char *type_names[] = {
    [FILE_TYPE_UNKNOWN]    = "unknown",
    [FILE_TYPE_BINARY]     = "binary",
    [FILE_TYPE_SCRIPT]     = "script",
    [FILE_TYPE_CONFIG]     = "config",
    [FILE_TYPE_LIBRARY]    = "library",
    [FILE_TYPE_KERNEL_MOD] = "kernel_module",
    [FILE_TYPE_ARCHIVE]    = "archive",
    [FILE_TYPE_TEXT]       = "text",
};

FileType classify_by_extension(const char *filename) {
    if (!filename) return FILE_TYPE_UNKNOWN;
    const char *dot = strrchr(filename, '.');
    if (!dot) return FILE_TYPE_UNKNOWN;
    for (int i = 0; ext_map[i].ext != NULL; i++) {
        if (strcasecmp(dot, ext_map[i].ext) == 0)
            return ext_map[i].type;
    }
    return FILE_TYPE_UNKNOWN;
}

FileType classify_by_magic(const uint8_t *buf, size_t len) {
    if (!buf || len < 4) return FILE_TYPE_UNKNOWN;
    if (buf[0] == 0x7f && buf[1] == 'E' && buf[2] == 'L' && buf[3] == 'F')
        return FILE_TYPE_BINARY;
    if (buf[0] == 0x1f && buf[1] == 0x8b)
        return FILE_TYPE_ARCHIVE;
    if (buf[0] == 'P' && buf[1] == 'K')
        return FILE_TYPE_ARCHIVE;
    if (buf[0] == '#' && buf[1] == '!')
        return FILE_TYPE_SCRIPT;
    return FILE_TYPE_UNKNOWN;
}

FileType classify_file(const char *filename, const uint8_t *buf, size_t len) {
    FileType t = classify_by_magic(buf, len);
    if (t != FILE_TYPE_UNKNOWN) return t;
    return classify_by_extension(filename);
}

const char *file_type_name(FileType type) {
    if (type >= FILE_TYPE_COUNT) return "unknown";
    return type_names[type];
}
