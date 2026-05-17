#include "suid_scanner.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_CAPACITY 16

int suid_result_init(SuidResult *result) {
    if (!result) return -1;
    result->entries = malloc(sizeof(SuidEntry) * INITIAL_CAPACITY);
    if (!result->entries) return -1;
    result->count = 0;
    result->capacity = INITIAL_CAPACITY;
    return 0;
}

static int suid_result_grow(SuidResult *result) {
    size_t new_cap = result->capacity * 2;
    SuidEntry *tmp = realloc(result->entries, sizeof(SuidEntry) * new_cap);
    if (!tmp) return -1;
    result->entries = tmp;
    result->capacity = new_cap;
    return 0;
}

int suid_scan_file(SuidResult *result, const char *path, const struct stat *st) {
    if (!result || !path || !st) return -1;

    int flags = 0;
    if (st->st_mode & S_ISUID) flags |= SUID_FLAG_SUID;
    if (st->st_mode & S_ISGID) flags |= SUID_FLAG_SGID;
    if (st->st_mode & S_ISVTX) flags |= SUID_FLAG_STICKY;

    if (flags == 0) return 0;

    if (result->count >= result->capacity) {
        if (suid_result_grow(result) != 0) return -1;
    }

    SuidEntry *e = &result->entries[result->count];
    strncpy(e->path, path, sizeof(e->path) - 1);
    e->path[sizeof(e->path) - 1] = '\0';
    e->uid = st->st_uid;
    e->gid = st->st_gid;
    e->mode = st->st_mode;
    e->flags = flags;
    result->count++;

    return 0;
}

const char *suid_flags_str(int flags) {
    if ((flags & SUID_FLAG_SUID) && (flags & SUID_FLAG_SGID)) return "suid+sgid";
    if (flags & SUID_FLAG_SUID)   return "suid";
    if (flags & SUID_FLAG_SGID)   return "sgid";
    if (flags & SUID_FLAG_STICKY) return "sticky";
    return "none";
}

void suid_result_free(SuidResult *result) {
    if (!result) return;
    free(result->entries);
    result->entries = NULL;
    result->count = 0;
    result->capacity = 0;
}
