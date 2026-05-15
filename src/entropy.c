#include "entropy.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>

int entropy_calc_buffer(const uint8_t *buf, size_t len, entropy_result_t *out)
{
    if (!buf || len == 0 || !out)
        return -1;

    unsigned long freq[256] = {0};
    for (size_t i = 0; i < len; i++)
        freq[buf[i]]++;

    double entropy = 0.0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] == 0)
            continue;
        double p = (double)freq[i] / (double)len;
        entropy -= p * log2(p);
    }

    out->entropy      = entropy;
    out->sample_size  = len;
    out->is_compressed = (entropy > ENTROPY_COMPRESS_THRESHOLD);
    out->is_encrypted  = (entropy > ENTROPY_ENCRYPT_THRESHOLD);
    return 0;
}

int entropy_calc_file(const char *path, entropy_result_t *out)
{
    if (!path || !out)
        return -1;

    FILE *fp = fopen(path, "rb");
    if (!fp) {
        fprintf(stderr, "entropy: cannot open %s: %s\n", path, strerror(errno));
        return -1;
    }

    uint8_t buf[ENTROPY_SAMPLE_MAX];
    size_t n = fread(buf, 1, sizeof(buf), fp);
    fclose(fp);

    if (n == 0) {
        /* empty file — zero entropy */
        out->entropy      = 0.0;
        out->sample_size  = 0;
        out->is_compressed = 0;
        out->is_encrypted  = 0;
        return 0;
    }

    return entropy_calc_buffer(buf, n, out);
}

const char *entropy_label(const entropy_result_t *res)
{
    if (!res)
        return "unknown";
    if (res->is_encrypted)
        return "encrypted/random";
    if (res->is_compressed)
        return "compressed";
    if (res->entropy < 1.0)
        return "plain/sparse";
    return "normal";
}
