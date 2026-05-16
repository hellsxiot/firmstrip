#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <stdint.h>
#include <stddef.h>

/* Supported checksum algorithms */
typedef enum {
    CKSUM_MD5,
    CKSUM_SHA1,
    CKSUM_SHA256,
    CKSUM_CRC32
} checksum_algo_t;

/* Result buffer sizes */
#define MD5_HEX_LEN    33
#define SHA1_HEX_LEN   41
#define SHA256_HEX_LEN 65
#define CRC32_HEX_LEN  9

typedef struct {
    checksum_algo_t algo;
    char            hex[SHA256_HEX_LEN]; /* largest possible */
    uint8_t         raw[32];             /* largest raw (sha256) */
    size_t          raw_len;
} checksum_result_t;

/*
 * Compute checksum of a file at the given path.
 * Returns 0 on success, -1 on error.
 */
int checksum_file(const char *path, checksum_algo_t algo,
                  checksum_result_t *out);

/*
 * Compute checksum over a memory buffer.
 * Returns 0 on success, -1 on error.
 */
int checksum_buf(const uint8_t *data, size_t len, checksum_algo_t algo,
                 checksum_result_t *out);

/* Return a human-readable algorithm name string */
const char *checksum_algo_name(checksum_algo_t algo);

#endif /* CHECKSUM_H */
