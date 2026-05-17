#ifndef SUID_SCANNER_H
#define SUID_SCANNER_H

#include <sys/stat.h>
#include <stddef.h>

/* Flags for suid/sgid detection */
#define SUID_FLAG_SUID   (1 << 0)
#define SUID_FLAG_SGID   (1 << 1)
#define SUID_FLAG_STICKY (1 << 2)

typedef struct {
    char path[512];
    uid_t uid;
    gid_t gid;
    mode_t mode;
    int flags; /* combination of SUID_FLAG_* */
} SuidEntry;

typedef struct {
    SuidEntry *entries;
    size_t count;
    size_t capacity;
} SuidResult;

/*
 * Initialize a SuidResult structure.
 * Returns 0 on success, -1 on failure.
 */
int suid_result_init(SuidResult *result);

/*
 * Scan a single file's stat for suid/sgid/sticky bits.
 * If any are found, appends to result.
 * Returns 0 on success, -1 on failure.
 */
int suid_scan_file(SuidResult *result, const char *path, const struct stat *st);

/*
 * Returns a human-readable string describing the flags.
 * Caller must not free the returned pointer.
 */
const char *suid_flags_str(int flags);

/*
 * Free all memory associated with a SuidResult.
 */
void suid_result_free(SuidResult *result);

#endif /* SUID_SCANNER_H */
