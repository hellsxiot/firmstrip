#ifndef MAGIC_BYTES_H
#define MAGIC_BYTES_H

#include <stdint.h>
#include <stddef.h>

/* Maximum number of magic byte patterns we support */
#define MAGIC_MAX_PATTERNS 64

/* Maximum length of a magic byte sequence */
#define MAGIC_MAX_SEQ_LEN 16

typedef enum {
    MAGIC_UNKNOWN = 0,
    MAGIC_ELF,
    MAGIC_SQUASHFS,
    MAGIC_GZIP,
    MAGIC_BZIP2,
    MAGIC_XZ,
    MAGIC_LZMA,
    MAGIC_ZIP,
    MAGIC_TAR,
    MAGIC_JFFS2,
    MAGIC_CRAMFS,
    MAGIC_UBIFS,
    MAGIC_PDF,
    MAGIC_PNG,
    MAGIC_JPEG,
    MAGIC_TYPE_COUNT
} MagicType;

typedef struct {
    MagicType   type;
    const char *name;
    uint8_t     bytes[MAGIC_MAX_SEQ_LEN];
    size_t      len;
    size_t      offset; /* byte offset in file to check */
} MagicPattern;

typedef struct {
    MagicType   type;
    const char *name;
    int         confidence; /* 0-100 */
} MagicResult;

/* Initialize the magic pattern table */
void magic_bytes_init(void);

/* Identify file type from raw bytes */
MagicResult magic_bytes_identify(const uint8_t *data, size_t len);

/* Identify file type from a file path */
MagicResult magic_bytes_identify_file(const char *path);

/* Return human-readable name for a MagicType */
const char *magic_bytes_type_name(MagicType type);

#endif /* MAGIC_BYTES_H */
