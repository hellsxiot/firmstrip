#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "path_filter.h"
#include "fs_walker.h"

#define MAX_RESULTS 256

static void usage(const char *prog) {
    fprintf(stderr, "usage: %s <root_dir> [--exclude|--include] <pattern> ...\n", prog);
    fprintf(stderr, "  --include  only show matching paths (default)\n");
    fprintf(stderr, "  --exclude  hide matching paths\n");
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 3) usage(argv[0]);

    const char *root = argv[1];
    PathFilterMode mode = FILTER_INCLUDE;
    int pat_start = 2;

    if (strcmp(argv[2], "--exclude") == 0) {
        mode = FILTER_EXCLUDE;
        pat_start = 3;
    } else if (strcmp(argv[2], "--include") == 0) {
        mode = FILTER_INCLUDE;
        pat_start = 3;
    }

    if (pat_start >= argc) usage(argv[0]);

    PathFilter *pf = path_filter_create(mode);
    if (!pf) { fprintf(stderr, "failed to create filter\n"); return 1; }

    for (int i = pat_start; i < argc; i++) {
        if (path_filter_add(pf, argv[i]) != 0) {
            fprintf(stderr, "failed to add pattern: %s\n", argv[i]);
            path_filter_free(pf);
            return 1;
        }
    }

    printf("filter mode : %s\n", mode == FILTER_INCLUDE ? "include" : "exclude");
    printf("patterns    : %d\n", path_filter_count(pf));
    printf("root        : %s\n\n", root);

    /* placeholder: in full integration this would walk root via fs_walker */
    printf("(filesystem walk integration pending)\n");

    path_filter_free(pf);
    return 0;
}
