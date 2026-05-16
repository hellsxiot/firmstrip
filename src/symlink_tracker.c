#include "symlink_tracker.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct SymlinkTracker {
    SymlinkEntry *entries;
    size_t count;
    size_t capacity;
};

SymlinkTracker *symlink_tracker_create(void) {
    SymlinkTracker *t = calloc(1, sizeof(SymlinkTracker));
    if (!t) return NULL;
    t->capacity = 16;
    t->entries = calloc(t->capacity, sizeof(SymlinkEntry));
    if (!t->entries) {
        free(t);
        return NULL;
    }
    return t;
}

void symlink_tracker_free(SymlinkTracker *t) {
    if (!t) return;
    for (size_t i = 0; i < t->count; i++) {
        free(t->entries[i].path);
        free(t->entries[i].target);
    }
    free(t->entries);
    free(t);
}

int symlink_tracker_add(SymlinkTracker *t, const char *path, const char *target) {
    if (!t || !path || !target) return -1;
    if (t->count >= t->capacity) {
        size_t new_cap = t->capacity * 2;
        SymlinkEntry *tmp = realloc(t->entries, new_cap * sizeof(SymlinkEntry));
        if (!tmp) return -1;
        t->entries = tmp;
        t->capacity = new_cap;
    }
    t->entries[t->count].path = strdup(path);
    t->entries[t->count].target = strdup(target);
    if (!t->entries[t->count].path || !t->entries[t->count].target) return -1;
    t->count++;
    return 0;
}

const char *symlink_tracker_resolve(const SymlinkTracker *t, const char *path) {
    if (!t || !path) return NULL;
    for (size_t i = 0; i < t->count; i++) {
        if (strcmp(t->entries[i].path, path) == 0)
            return t->entries[i].target;
    }
    return NULL;
}

size_t symlink_tracker_count(const SymlinkTracker *t) {
    return t ? t->count : 0;
}

const SymlinkEntry *symlink_tracker_get_all(const SymlinkTracker *t, size_t *out_count) {
    if (!t || !out_count) return NULL;
    *out_count = t->count;
    return t->entries;
}

int symlink_tracker_has_cycle(const SymlinkTracker *t, const char *start) {
    if (!t || !start) return 0;
    const char *cur = start;
    int steps = 0;
    while (cur && steps < (int)t->count + 1) {
        cur = symlink_tracker_resolve(t, cur);
        if (cur && strcmp(cur, start) == 0) return 1;
        steps++;
    }
    return 0;
}
