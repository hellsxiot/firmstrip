/*
 * inode_stat.h - Collect and report inode-level stat info for files
 */

#ifndef INODE_STAT_H
#define INODE_STAT_H

#include <sys/stat.h>
#include <stdint.h>

/* Inode stat record */
typedef struct {
    char        path[512];
    ino_t       inode;
    mode_t      mode;
    uid_t       uid;
    gid_t       gid;
    off_t       size;
    nlink_t     nlink;
    time_t      mtime;
    dev_t       dev;
} InodeStat;

/* Collection of inode stat records */
typedef struct {
    InodeStat  *entries;
    size_t      count;
    size_t      capacity;
} InodeStatTable;

/*
 * Initialize an InodeStatTable.
 * Returns 0 on success, -1 on failure.
 */
int inode_stat_table_init(InodeStatTable *table, size_t initial_capacity);

/*
 * Stat a file and append its record to the table.
 * Returns 0 on success, -1 on failure.
 */
int inode_stat_collect(InodeStatTable *table, const char *path);

/*
 * Print a summary of all collected inode stats to stdout.
 */
void inode_stat_report(const InodeStatTable *table);

/*
 * Free resources held by the table.
 */
void inode_stat_table_free(InodeStatTable *table);

#endif /* INODE_STAT_H */
