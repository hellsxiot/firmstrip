#ifndef FIRMSTRIP_SQUASHFS_H
#define FIRMSTRIP_SQUASHFS_H

#include <stdio.h>
#include <stdint.h>

/* Minimal SquashFS superblock layout (v4, little-endian fields stored raw) */
struct squashfs_super_block {
    uint32_t magic;
    uint32_t inodes;
    uint32_t mkfs_time;
    uint32_t block_size;
    uint32_t fragments;
    uint16_t compression;
    uint16_t block_log;
    uint16_t flags;
    uint16_t no_ids;
    uint16_t s_major;
    uint16_t s_minor;
    uint64_t root_inode;
    uint64_t bytes_used;
    uint64_t id_table_start;
    uint64_t xattr_id_table_start;
    uint64_t inode_table_start;
    uint64_t directory_table_start;
    uint64_t fragment_table_start;
    uint64_t lookup_table_start;
} __attribute__((packed));

typedef enum {
    SQUASHFS_ENDIAN_LE = 0,
    SQUASHFS_ENDIAN_BE = 1
} squashfs_endian_t;

struct squashfs_info {
    long             offset;
    squashfs_endian_t endian;
    uint16_t         version_maj;
    uint16_t         version_min;
    uint32_t         inode_count;
    uint32_t         block_size;
    uint64_t         bytes_used;
};

/**
 * Probe an open file for a SquashFS superblock at the given offset.
 * Returns 0 on success, -1 if not found or on error.
 * Fills *out with parsed metadata on success.
 */
int  squashfs_probe(FILE *fp, long offset, struct squashfs_info *out);

/** Pretty-print parsed SquashFS metadata. */
void squashfs_print_info(const struct squashfs_info *info);

#endif /* FIRMSTRIP_SQUASHFS_H */
