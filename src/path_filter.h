#ifndef PATH_FILTER_H
#define PATH_FILTER_H

#include <stddef.h>

/* Maximum number of filter patterns supported */
#define PATH_FILTER_MAX_PATTERNS 64

/* Filter modes */
typedef enum {
    FILTER_MODE_INCLUDE,  /* only include matching paths */
    FILTER_MODE_EXCLUDE   /* exclude matching paths */
} filter_mode_t;

typedef struct {
    char *patterns[PATH_FILTER_MAX_PATTERNS];
    size_t count;
    filter_mode_t mode;
} path_filter_t;

/*
 * Initialize a path filter with the given mode.
 * Returns 0 on success, -1 on failure.
 */
int path_filter_init(path_filter_t *filter, filter_mode_t mode);

/*
 * Add a glob-style pattern to the filter.
 * Returns 0 on success, -1 if pattern list is full or allocation fails.
 */
int path_filter_add(path_filter_t *filter, const char *pattern);

/*
 * Test whether the given path passes the filter.
 * In FILTER_MODE_INCLUDE: returns 1 if any pattern matches, 0 otherwise.
 * In FILTER_MODE_EXCLUDE: returns 0 if any pattern matches, 1 otherwise.
 * If no patterns have been added, all paths pass (returns 1).
 */
int path_filter_match(const path_filter_t *filter, const char *path);

/*
 * Remove all patterns from the filter, resetting it to an empty state.
 * The filter mode is preserved. Frees memory for all stored patterns.
 */
void path_filter_clear(path_filter_t *filter);

/*
 * Free all resources held by the filter.
 */
void path_filter_free(path_filter_t *filter);

#endif /* PATH_FILTER_H */
