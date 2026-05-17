#include "permission_checker.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

static const PermissionRule default_rules[] = {
    { S_ISUID, PERM_SUID,   "setuid bit set" },
    { S_ISGID, PERM_SGID,   "setgid bit set" },
    { S_ISVTX, PERM_STICKY, "sticky bit set" },
    { 0002,    PERM_WORLD_WRITE, "world-writable" },
    { 0,       PERM_OK,     NULL }
};

PermissionResult permission_check(const char *path, mode_t mode) {
    PermissionResult result;
    memset(&result, 0, sizeof(result));

    if (!path) {
        result.flags = PERM_ERROR;
        snprintf(result.description, sizeof(result.description),
                 "null path provided");
        return result;
    }

    strncpy(result.path, path, sizeof(result.path) - 1);
    result.mode = mode;
    result.flags = PERM_OK;

    for (int i = 0; default_rules[i].mask != 0; i++) {
        if (mode & default_rules[i].mask) {
            result.flags |= default_rules[i].flag;
            if (result.description[0] != '\0')
                strncat(result.description, ", ",
                        sizeof(result.description) - strlen(result.description) - 1);
            strncat(result.description, default_rules[i].label,
                    sizeof(result.description) - strlen(result.description) - 1);
        }
    }

    if (result.flags == PERM_OK)
        snprintf(result.description, sizeof(result.description), "ok");

    return result;
}

int permission_is_suspicious(const PermissionResult *result) {
    if (!result) return 0;
    return (result->flags & (PERM_SUID | PERM_SGID | PERM_WORLD_WRITE)) != 0;
}

void permission_result_print(const PermissionResult *result, FILE *out) {
    if (!result || !out) return;
    fprintf(out, "[perm] %s mode=%04o flags=0x%x desc=%s suspicious=%s\n",
            result->path,
            (unsigned)result->mode,
            result->flags,
            result->description,
            permission_is_suspicious(result) ? "yes" : "no");
}
