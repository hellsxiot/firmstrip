/*
 * permission_checker.h - Check and report file permission anomalies
 * in extracted firmware filesystems.
 */

#ifndef PERMISSION_CHECKER_H
#define PERMISSION_CHECKER_H

#include <sys/stat.h>
#include <stdint.h>

/* Permission anomaly flags */
#define PERM_WORLD_WRITABLE   (1 << 0)  /* file is world-writable */
#define PERM_SUID_SET         (1 << 1)  /* setuid bit is set */
#define PERM_SGID_SET         (1 << 2)  /* setgid bit is set */
#define PERM_STICKY_SET       (1 << 3)  /* sticky bit set on non-dir */
#define PERM_NO_OWNER_READ    (1 << 4)  /* owner cannot read own file */
#define PERM_EXEC_SCRIPT      (1 << 5)  /* script file is executable */

typedef struct {
    char path[512];
    mode_t mode;
    uint32_t flags;   /* bitmask of PERM_* anomalies found */
} perm_result_t;

typedef struct {
    perm_result_t *entries;
    size_t count;
    size_t capacity;
} perm_report_t;

/*
 * Analyze a single file's permissions.
 * Returns a bitmask of PERM_* flags indicating anomalies, or 0 if clean.
 */
uint32_t perm_check_file(const char *path, mode_t mode);

/*
 * Initialize a permission report.
 */
void perm_report_init(perm_report_t *report);

/*
 * Add a result entry to the report.
 */
int perm_report_add(perm_report_t *report, const char *path, mode_t mode, uint32_t flags);

/*
 * Print a human-readable summary of the permission report to stdout.
 */
void perm_report_print(const perm_report_t *report);

/*
 * Free resources held by the report.
 */
void perm_report_free(perm_report_t *report);

#endif /* PERMISSION_CHECKER_H */
