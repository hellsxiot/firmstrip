#include "path_filter.h"
#include <string.h>
#include <stdlib.h>
#include <fnmatch.h>

struct PathFilter {
    char **patterns;
    int count;
    int capacity;
    PathFilterMode mode;
};

PathFilter *path_filter_create(PathFilterMode mode) {
    PathFilter *pf = calloc(1, sizeof(PathFilter));
    if (!pf) return NULL;
    pf->mode = mode;
    pf->capacity = 8;
    pf->patterns = calloc(pf->capacity, sizeof(char *));
    if (!pf->patterns) { free(pf); return NULL; }
    return pf;
}

void path_filter_free(PathFilter *pf) {
    if (!pf) return;
    for (int i = 0; i < pf->count; i++)
        free(pf->patterns[i]);
    free(pf->patterns);
    free(pf);
}

int path_filter_add(PathFilter *pf, const char *pattern) {
    if (!pf || !pattern) return -1;
    if (pf->count >= pf->capacity) {
        int new_cap = pf->capacity * 2;
        char **tmp = realloc(pf->patterns, new_cap * sizeof(char *));
        if (!tmp) return -1;
        pf->patterns = tmp;
        pf->capacity = new_cap;
    }
    pf->patterns[pf->count] = strdup(pattern);
    if (!pf->patterns[pf->count]) return -1;
    pf->count++;
    return 0;
}

bool path_filter_match(const PathFilter *pf, const char *path) {
    if (!pf || !path) return false;
    if (pf->count == 0) return pf->mode == FILTER_EXCLUDE ? false : true;
    for (int i = 0; i < pf->count; i++) {
        if (fnmatch(pf->patterns[i], path, FNM_PATHNAME) == 0) {
            return pf->mode == FILTER_INCLUDE ? true : false;
        }
    }
    return pf->mode == FILTER_INCLUDE ? false : true;
}

int path_filter_count(const PathFilter *pf) {
    return pf ? pf->count : 0;
}
