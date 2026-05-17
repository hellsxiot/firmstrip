#include <stdlib.h>
#include <string.h>
#include "hardlink_map.h"

static unsigned int bucket_index(ino_t inode, dev_t device) {
    unsigned long h = (unsigned long)inode ^ ((unsigned long)device << 16);
    return (unsigned int)(h % HARDLINK_MAP_BUCKETS);
}

hardlink_map_t *hardlink_map_create(void) {
    hardlink_map_t *map = calloc(1, sizeof(hardlink_map_t));
    return map;
}

void hardlink_map_free(hardlink_map_t *map) {
    if (!map) return;
    for (int i = 0; i < HARDLINK_MAP_BUCKETS; i++) {
        hardlink_entry_t *e = map->buckets[i];
        while (e) {
            hardlink_entry_t *next = e->next;
            free(e->first_path);
            free(e);
            e = next;
        }
    }
    free(map);
}

const char *hardlink_map_register(hardlink_map_t *map, ino_t inode, dev_t device, const char *path) {
    if (!map || !path) return NULL;

    unsigned int idx = bucket_index(inode, device);
    hardlink_entry_t *e = map->buckets[idx];

    while (e) {
        if (e->inode == inode && e->device == device) {
            e->link_count++;
            if (e->link_count == 2) {
                map->total_hardlinks++;
            }
            return e->first_path;
        }
        e = e->next;
    }

    /* New inode */
    hardlink_entry_t *entry = calloc(1, sizeof(hardlink_entry_t));
    if (!entry) return NULL;
    entry->inode = inode;
    entry->device = device;
    entry->first_path = strdup(path);
    entry->link_count = 1;
    entry->next = map->buckets[idx];
    map->buckets[idx] = entry;
    map->total_inodes++;
    return NULL;
}

const hardlink_entry_t *hardlink_map_lookup(const hardlink_map_t *map, ino_t inode, dev_t device) {
    if (!map) return NULL;
    unsigned int idx = bucket_index(inode, device);
    const hardlink_entry_t *e = map->buckets[idx];
    while (e) {
        if (e->inode == inode && e->device == device) return e;
        e = e->next;
    }
    return NULL;
}

void hardlink_map_foreach_dup(const hardlink_map_t *map,
                              void (*cb)(const hardlink_entry_t *entry, void *ctx),
                              void *ctx) {
    if (!map || !cb) return;
    for (int i = 0; i < HARDLINK_MAP_BUCKETS; i++) {
        const hardlink_entry_t *e = map->buckets[i];
        while (e) {
            if (e->link_count > 1) cb(e, ctx);
            e = e->next;
        }
    }
}

int hardlink_map_dup_count(const hardlink_map_t *map) {
    return map ? map->total_hardlinks : 0;
}
