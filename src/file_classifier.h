#ifndef FILE_CLASSIFIER_H
#define FILE_CLASSIFIER_H

#include <stdint.h>
#include <stddef.h>

typedef enum {
    FILE_TYPE_UNKNOWN = 0,
    FILE_TYPE_BINARY,
    FILE_TYPE_SCRIPT,
    FILE_TYPE_CONFIG,
    FILE_TYPE_LIBRARY,
    FILE_TYPE_KERNEL_MOD,
    FILE_TYPE_ARCHIVE,
    FILE_TYPE_TEXT,
    FILE_TYPE_COUNT
} FileType;

/* Classify based on file extension alone */
FileType classify_by_extension(const char *filename);

/* Classify based on magic bytes in buffer */
FileType classify_by_magic(const uint8_t *buf, size_t len);

/* Classify using magic first, fallback to extension */
FileType classify_file(const char *filename, const uint8_t *buf, size_t len);

/* Return human-readable name for a FileType */
const char *file_type_name(FileType type);

#endif /* FILE_CLASSIFIER_H */
