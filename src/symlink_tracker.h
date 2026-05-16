/*
 * symlink_tracker.h - Track and resolve symbolic links found in firmware
 * filesystems to detect loops, dangling refs, and suspicious targets.
 */

#ifndef SYMLINK_TRACKER_H
#define SYMLINK_TRACKER_H

#include <stddef.h>

#define SYMLINK_MAX_ENTRIES  512
#define SYMLINK_PATH_MAX     256
#define SYMLINK_MAX_DEPTH    16

typedef enum {
    SYMLINK_OK = 0,
    SYMLINK_DANGLING,      /* target does not exist */
    SYMLINK_LOOP,          /* circular reference detected */
    SYMLINK_EXTERNAL,      /* points outside filesystem root */
    SYMLINK_SUSPICIOUS,    /* points to sensitive path */
    SYMLINK_DEPTH_EXCEEDED /* too many levels of indirection */
} symlink_status_t;

typedef struct {
    char path[SYMLINK_PATH_MAX];      /* path of the symlink itself */
    char target[SYMLINK_PATH_MAX];    /* raw link target */
    char resolved[SYMLINK_PATH_MAX];  /* resolved absolute path */
    symlink_status_t status;
} symlink_entry_t;

typedef struct {
    symlink_entry_t entries[SYMLINK_MAX_ENTRIES];
    size_t count;
    size_t dangling;
    size_t loops;
    size_t external;
    size_t suspicious;
} symlink_tracker_t;

void symlink_tracker_init(symlink_tracker_t *tracker);

int symlink_tracker_add(symlink_tracker_t *tracker,
                        const char *path,
                        const char *target,
                        const char *fs_root);

symlink_status_t symlink_tracker_classify(const char *path,
                                          const char *target,
                                          const char *fs_root,
                                          char *resolved_out,
                                          size_t resolved_len);

void symlink_tracker_print_summary(const symlink_tracker_t *tracker);
void symlink_tracker_dump(const symlink_tracker_t *tracker);

#endif /* SYMLINK_TRACKER_H */
