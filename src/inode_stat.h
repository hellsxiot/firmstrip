#ifndef INODE_STAT_H
#define INODE_STAT_H

#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/types.h>
#include <time.h>

#define INODE_PATH_MAX 512

typedef enum {
    INODE_UNKNOWN = 0,
    INODE_REG,
    INODE_DIR,
    INODE_LNK,
    INODE_BLK,
    INODE_CHR,
    INODE_FIFO,
    INODE_SOCK
} InodeType;

typedef struct {
    char     path[INODE_PATH_MAX];
    uint64_t inode_num;
    uint32_t nlink;
    uid_t    uid;
    gid_t    gid;
    mode_t   mode;
    uint64_t size;
    time_t   mtime;
    InodeType type;
} InodeStat;

/* Fill an InodeStat by calling lstat() on path.
 * Returns 0 on success, -1 on error. */
int inode_stat_fill(const char *path, InodeStat *out);

/* Print a human-readable one-liner for the inode to the given stream. */
void inode_stat_print(const InodeStat *s, FILE *out);

#endif /* INODE_STAT_H */
