#include "magic_bytes.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static MagicPattern patterns[] = {
    { MAGIC_ELF,      "ELF Binary",   {0x7f,'E','L','F'},                    4,  0 },
    { MAGIC_SQUASHFS, "SquashFS",     {'s','q','s','h'},                      4,  0 },
    { MAGIC_SQUASHFS, "SquashFS-LE",  {'h','s','q','s'},                      4,  0 },
    { MAGIC_GZIP,     "GZIP",         {0x1f, 0x8b},                           2,  0 },
    { MAGIC_BZIP2,    "BZIP2",        {'B','Z','h'},                          3,  0 },
    { MAGIC_XZ,       "XZ",           {0xfd,'7','z','X','Z',0x00},            6,  0 },
    { MAGIC_LZMA,     "LZMA",         {0x5d, 0x00, 0x00},                     3,  0 },
    { MAGIC_ZIP,      "ZIP",          {'P','K', 0x03, 0x04},                  4,  0 },
    { MAGIC_JFFS2,    "JFFS2",        {0x19, 0x85},                           2,  0 },
    { MAGIC_CRAMFS,   "CramFS",       {0x45, 0x3d, 0xcd, 0x28},              4,  0 },
    { MAGIC_PDF,      "PDF",          {'%','P','D','F','-'},                  5,  0 },
    { MAGIC_PNG,      "PNG",          {0x89,'P','N','G',0x0d,0x0a,0x1a,0x0a},8,  0 },
    { MAGIC_JPEG,     "JPEG",         {0xff, 0xd8, 0xff},                     3,  0 },
};

static const size_t pattern_count = sizeof(patterns) / sizeof(patterns[0]);

void magic_bytes_init(void) {
    /* nothing dynamic to init right now */
}

MagicResult magic_bytes_identify(const uint8_t *data, size_t len) {
    MagicResult result = { MAGIC_UNKNOWN, "Unknown", 0 };

    if (!data || len == 0)
        return result;

    for (size_t i = 0; i < pattern_count; i++) {
        const MagicPattern *p = &patterns[i];
        size_t end = p->offset + p->len;

        if (len < end)
            continue;

        if (memcmp(data + p->offset, p->bytes, p->len) == 0) {
            result.type       = p->type;
            result.name       = p->name;
            result.confidence = 95;
            return result;
        }
    }

    return result;
}

MagicResult magic_bytes_identify_file(const char *path) {
    MagicResult result = { MAGIC_UNKNOWN, "Unknown", 0 };
    uint8_t buf[MAGIC_MAX_SEQ_LEN + 8];

    FILE *f = fopen(path, "rb");
    if (!f)
        return result;

    size_t n = fread(buf, 1, sizeof(buf), f);
    fclose(f);

    return magic_bytes_identify(buf, n);
}

const char *magic_bytes_type_name(MagicType type) {
    for (size_t i = 0; i < pattern_count; i++) {
        if (patterns[i].type == type)
            return patterns[i].name;
    }
    return "Unknown";
}
