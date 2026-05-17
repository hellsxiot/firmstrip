#ifndef OWNER_INFO_H
#define OWNER_INFO_H

#include <sys/types.h>

#define OWNER_NAME_MAX 64

/**
 * Holds ownership and permission metadata for a single filesystem entry.
 */
typedef struct {
    uid_t uid;
    gid_t gid;
    char  username[OWNER_NAME_MAX];
    char  groupname[OWNER_NAME_MAX];
    int   is_root_owned;
    int   is_world_writable;
    int   is_setuid;
    int   is_setgid;
} OwnerInfo;

/**
 * Populate an OwnerInfo struct by stat-ing the given path.
 * Returns 0 on success, -1 on error.
 */
int owner_info_get(const char *path, OwnerInfo *out);

/**
 * Returns 1 if the entry has any suspicious flags
 * (world-writable, setuid, or setgid), 0 otherwise.
 */
int owner_info_is_suspicious(const OwnerInfo *info);

/**
 * Print a single-line summary of the owner info for the given path.
 */
void owner_info_print(const OwnerInfo *info, const char *path);

#endif /* OWNER_INFO_H */
