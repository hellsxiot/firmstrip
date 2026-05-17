#include "owner_info.h"
#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>

int owner_info_get(const char *path, OwnerInfo *out) {
    if (!path || !out) return -1;

    struct stat st;
    if (lstat(path, &st) != 0) return -1;

    out->uid = st.st_uid;
    out->gid = st.st_gid;

    struct passwd *pw = getpwuid(st.st_uid);
    if (pw) {
        strncpy(out->username, pw->pw_name, OWNER_NAME_MAX - 1);
        out->username[OWNER_NAME_MAX - 1] = '\0';
    } else {
        snprintf(out->username, OWNER_NAME_MAX, "%u", st.st_uid);
    }

    struct group *gr = getgrgid(st.st_gid);
    if (gr) {
        strncpy(out->groupname, gr->gr_name, OWNER_NAME_MAX - 1);
        out->groupname[OWNER_NAME_MAX - 1] = '\0';
    } else {
        snprintf(out->groupname, OWNER_NAME_MAX, "%u", st.st_gid);
    }

    out->is_root_owned = (st.st_uid == 0);
    out->is_world_writable = (st.st_mode & S_IWOTH) != 0;
    out->is_setuid = (st.st_mode & S_ISUID) != 0;
    out->is_setgid = (st.st_mode & S_ISGID) != 0;

    return 0;
}

int owner_info_is_suspicious(const OwnerInfo *info) {
    if (!info) return 0;
    return info->is_world_writable || info->is_setuid || info->is_setgid;
}

void owner_info_print(const OwnerInfo *info, const char *path) {
    if (!info || !path) return;
    printf("%-40s uid=%-6u(%s) gid=%-6u(%s)%s%s%s\n",
        path,
        info->uid, info->username,
        info->gid, info->groupname,
        info->is_setuid  ? " [SETUID]"  : "",
        info->is_setgid  ? " [SETGID]"  : "",
        info->is_world_writable ? " [WORLD-WRITABLE]" : ""
    );
}
