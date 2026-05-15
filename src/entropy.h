#ifndef ENTROPY_H
#define ENTROPY_H

#include <stddef.h>
#include <stdint.h>

/* Shannon entropy result for a file or buffer */
typedef struct {
    double entropy;       /* 0.0 (uniform) to 8.0 (max random) */
    size_t sample_size;   /* bytes actually analyzed */
    int is_compressed;    /* heuristic: entropy > ENTROPY_COMPRESS_THRESHOLD */
    int is_encrypted;     /* heuristic: entropy > ENTROPY_ENCRYPT_THRESHOLD */
} entropy_result_t;

/* Thresholds (bits per byte) */
#define ENTROPY_COMPRESS_THRESHOLD  6.8
#define ENTROPY_ENCRYPT_THRESHOLD   7.4

/* Max bytes read for large-file sampling */
#define ENTROPY_SAMPLE_MAX  65536

/**
 * Calculate Shannon entropy of a memory buffer.
 * Returns 0 on success, -1 on invalid args.
 */
int entropy_calc_buffer(const uint8_t *buf, size_t len, entropy_result_t *out);

/**
 * Calculate Shannon entropy of a file (samples up to ENTROPY_SAMPLE_MAX bytes).
 * Returns 0 on success, -1 on error.
 */
int entropy_calc_file(const char *path, entropy_result_t *out);

/**
 * Return a human-readable label for the entropy result.
 */
const char *entropy_label(const entropy_result_t *res);

#endif /* ENTROPY_H */
