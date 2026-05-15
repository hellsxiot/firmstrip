#include "entropy_report.h"
#include "entropy.h"

#include <stdio.h>
#include <string.h>

void entropy_report_file(const char *path, FILE *out)
{
    if (!path || !out)
        return;

    entropy_result_t res;
    if (entropy_calc_file(path, &res) != 0) {
        fprintf(out, "  [entropy] %-50s  ERROR\n", path);
        return;
    }

    fprintf(out,
        "  [entropy] %-50s  %.4f bits/byte  (%s, sampled %zu bytes)\n",
        path,
        res.entropy,
        entropy_label(&res),
        res.sample_size);
}

void entropy_report_batch(const char **paths, size_t count, FILE *out)
{
    if (!paths || !out)
        return;

    fprintf(out, "\n=== Entropy Analysis ===\n");
    size_t compressed = 0, encrypted = 0;

    for (size_t i = 0; i < count; i++) {
        entropy_result_t res;
        if (entropy_calc_file(paths[i], &res) != 0)
            continue;
        entropy_report_file(paths[i], out);
        if (res.is_encrypted) encrypted++;
        else if (res.is_compressed) compressed++;
    }

    fprintf(out, "\nSummary: %zu files  |  compressed: %zu  |  encrypted: %zu\n",
            count, compressed, encrypted);
}
