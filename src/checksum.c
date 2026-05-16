#include "checksum.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <openssl/md5.h>
#include <openssl/sha.h>

/* CRC32 lookup table (IEEE polynomial) */
static uint32_t crc32_table[256];
static int      crc32_table_ready = 0;

static void crc32_init(void) {
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t c = i;
        for (int j = 0; j < 8; j++)
            c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
        crc32_table[i] = c;
    }
    crc32_table_ready = 1;
}

static uint32_t crc32_compute(const uint8_t *data, size_t len) {
    if (!crc32_table_ready) crc32_init();
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < len; i++)
        crc = crc32_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    return crc ^ 0xFFFFFFFFu;
}

static void bytes_to_hex(const uint8_t *src, size_t len, char *dst) {
    static const char hex[] = "0123456789abcdef";
    for (size_t i = 0; i < len; i++) {
        dst[i * 2]     = hex[(src[i] >> 4) & 0xF];
        dst[i * 2 + 1] = hex[src[i] & 0xF];
    }
    dst[len * 2] = '\0';
}

int checksum_buf(const uint8_t *data, size_t len, checksum_algo_t algo,
                 checksum_result_t *out) {
    if (!data || !out) return -1;
    out->algo = algo;
    switch (algo) {
        case CKSUM_MD5:
            MD5(data, len, out->raw);
            out->raw_len = 16;
            break;
        case CKSUM_SHA1:
            SHA1(data, len, out->raw);
            out->raw_len = 20;
            break;
        case CKSUM_SHA256:
            SHA256(data, len, out->raw);
            out->raw_len = 32;
            break;
        case CKSUM_CRC32: {
            uint32_t crc = crc32_compute(data, len);
            memcpy(out->raw, &crc, 4);
            out->raw_len = 4;
            break;
        }
        default:
            return -1;
    }
    bytes_to_hex(out->raw, out->raw_len, out->hex);
    return 0;
}

int checksum_file(const char *path, checksum_algo_t algo,
                  checksum_result_t *out) {
    FILE *f = fopen(path, "rb");
    if (!f) return -1;

    uint8_t buf[4096];
    size_t  n;

    /* Use streaming OpenSSL contexts for large files */
    MD5_CTX    md5_ctx;
    SHA_CTX    sha1_ctx;
    SHA256_CTX sha256_ctx;
    uint32_t   crc = 0xFFFFFFFFu;

    if (!crc32_table_ready) crc32_init();

    switch (algo) {
        case CKSUM_MD5:    MD5_Init(&md5_ctx);    break;
        case CKSUM_SHA1:   SHA1_Init(&sha1_ctx);  break;
        case CKSUM_SHA256: SHA256_Init(&sha256_ctx); break;
        default: break;
    }

    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        switch (algo) {
            case CKSUM_MD5:    MD5_Update(&md5_ctx, buf, n);    break;
            case CKSUM_SHA1:   SHA1_Update(&sha1_ctx, buf, n);  break;
            case CKSUM_SHA256: SHA256_Update(&sha256_ctx, buf, n); break;
            case CKSUM_CRC32:
                for (size_t i = 0; i < n; i++)
                    crc = crc32_table[(crc ^ buf[i]) & 0xFF] ^ (crc >> 8);
                break;
        }
    }
    fclose(f);

    out->algo = algo;
    switch (algo) {
        case CKSUM_MD5:    MD5_Final(out->raw, &md5_ctx);    out->raw_len = 16; break;
        case CKSUM_SHA1:   SHA1_Final(out->raw, &sha1_ctx);  out->raw_len = 20; break;
        case CKSUM_SHA256: SHA256_Final(out->raw, &sha256_ctx); out->raw_len = 32; break;
        case CKSUM_CRC32: {
            uint32_t final_crc = crc ^ 0xFFFFFFFFu;
            memcpy(out->raw, &final_crc, 4);
            out->raw_len = 4;
            break;
        }
    }
    bytes_to_hex(out->raw, out->raw_len, out->hex);
    return 0;
}

const char *checksum_algo_name(checksum_algo_t algo) {
    switch (algo) {
        case CKSUM_MD5:    return "md5";
        case CKSUM_SHA1:   return "sha1";
        case CKSUM_SHA256: return "sha256";
        case CKSUM_CRC32:  return "crc32";
        default:           return "unknown";
    }
}
