#ifndef FILE_CLASSIFIER_H
#define FILE_CLASSIFIER_H

#include <stdint.h>
#include <stddef.h>

/* File type categories for IoT firmware content */
typedef enum {
    FTYPE_UNKNOWN = 0,
    FTYPE_ELF_BINARY,
    FTYPE_SHELL_SCRIPT,
    FTYPE_CONFIG,
    FTYPE_CERTIFICATE,
    FTYPE_PRIVATE_KEY,
    FTYPE_ARCHIVE,
    FTYPE_IMAGE,
    FTYPE_TEXT,
    FTYPE_DATA,
} FileType;

typedef struct {
    FileType type;
    const char *type_name;
    int is_sensitive;   /* 1 if file may contain credentials/keys */
    int is_executable;  /* 1 if file appears to be executable code */
} FileClassification;

/*
 * Classify a file based on its path and first few bytes of content.
 * buf may be NULL if no content is available (path-only classification).
 */
FileClassification classify_file(const char *path, const uint8_t *buf, size_t buf_len);

/* Return a human-readable name for a FileType */
const char *file_type_name(FileType type);

#endif /* FILE_CLASSIFIER_H */
