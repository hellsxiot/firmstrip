/*
 * hardlink_map.h - Track hard links by inode number across filesystem walk
 */

#ifndef HARDLINK_MAP_H
#define HARDLINK_MAP_H

#include <stdint.h>
#include <sys/types.h>

#define HARDLINK_MAP_BUCKETS 256

typedef struct hardlink_entry {
    ino_t inode;
    dev_t device;
    char *first_path;           /* first path seen for this inode */
    int link_count;             /* how many paths share this inode */
    struct hardlink_entry *next;
} hardlink_entry_t;

typedef struct {
    hardlink_entry_t *buckets[HARDLINK_MAP_BUCKETS];
    int total_inodes;           /* unique inodes tracked */
    int total_hardlinks;        /* inodes with link_count > 1 */
} hardlink_map_t;

/* Initialize a new hardlink map */
hardlink_map_t *hardlink_map_create(void);

/* Free all resources */
void hardlink_map_free(hardlink_map_t *map);

/*
 * Register a path with its inode/device.
 * Returns NULL if this is the first time we see this inode (not a hardlink).
 * Returns the first_path if this inode was already seen (is a hardlink).
 */
const char *hardlink_map_register(hardlink_map_t *map, ino_t inode, dev_t device, const char *path);

/* Look up an entry by inode+device, returns NULL if not found */
const hardlink_entry_t *hardlink_map_lookup(const hardlink_map_t *map, ino_t inode, dev_t device);

/* Iterate over all entries that have more than one link */
void hardlink_map_foreach_dup(const hardlink_map_t *map,
                              void (*cb)(const hardlink_entry_t *entry, void *ctx),
                              void *ctx);

/* Return number of inodes with link_count > 1 */
int hardlink_map_dup_count(const hardlink_map_t *map);

#endif /* HARDLINK_MAP_H */
