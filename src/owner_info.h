/*
 * owner_info.h - UID/GID ownership tracking for firmware files
 */

#ifndef OWNER_INFO_H
#define OWNER_INFO_H

#include <sys/types.h>
#include <stdint.h>

/* Entry representing a single file's ownership */
typedef struct {
    char   *path;
    uid_t   uid;
    gid_t   gid;
} OwnerEntry;

/* Collection of ownership entries */
typedef struct {
    OwnerEntry *entries;
    size_t      count;
    size_t      capacity;
} OwnerInfo;

/*
 * Allocate a new OwnerInfo collector.
 * Returns NULL on allocation failure.
 */
OwnerInfo *owner_info_create(void);

/*
 * Add a file path with its uid/gid to the collector.
 * Returns 0 on success, -1 on failure.
 */
int owner_info_add(OwnerInfo *oi, const char *path, uid_t uid, gid_t gid);

/*
 * Find entries matching a specific uid.
 * Caller must free the returned array (not the entries inside).
 * Returns NULL if none found; sets *out_count.
 */
OwnerEntry **owner_info_find_by_uid(const OwnerInfo *oi, uid_t uid, size_t *out_count);

/*
 * Find entries matching a specific gid.
 * Caller must free the returned array (not the entries inside).
 * Returns NULL if none found; sets *out_count.
 */
OwnerEntry **owner_info_find_by_gid(const OwnerInfo *oi, gid_t gid, size_t *out_count);

/*
 * Print a summary of ownership to stdout.
 */
void owner_info_print(const OwnerInfo *oi);

/*
 * Free all resources held by the collector.
 */
void owner_info_free(OwnerInfo *oi);

#endif /* OWNER_INFO_H */
