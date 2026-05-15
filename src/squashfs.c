#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include "squashfs.h"

/* Magic bytes for SquashFS (little-endian and big-endian variants) */
#define SQUASHFS_MAGIC_LE 0x73717368
#define SQUASHFS_MAGIC_BE 0x68737173

int squashfs_probe(FILE *fp, long offset, struct squashfs_info *out) {
    if (!fp || !out) return -1;

    if (fseek(fp, offset, SEEK_SET) != 0) {
        fprintf(stderr, "[squashfs] seek failed: %s\n", strerror(errno));
        return -1;
    }

    struct squashfs_super_block sb;
    if (fread(&sb, sizeof(sb), 1, fp) != 1) {
        return -1;
    }

    int is_le = (sb.magic == SQUASHFS_MAGIC_LE);
    int is_be = (sb.magic == SQUASHFS_MAGIC_BE);

    if (!is_le && !is_be) {
        return -1; /* not a squashfs image */
    }

    out->offset      = offset;
    out->endian      = is_le ? SQUASHFS_ENDIAN_LE : SQUASHFS_ENDIAN_BE;
    out->version_maj = is_le ? sb.s_major : __builtin_bswap16(sb.s_major);
    out->version_min = is_le ? sb.s_minor : __builtin_bswap16(sb.s_minor);
    out->inode_count = is_le ? sb.inodes  : __builtin_bswap32(sb.inodes);
    out->block_size  = is_le ? sb.block_size : __builtin_bswap32(sb.block_size);
    out->bytes_used  = is_le ? sb.bytes_used : __builtin_bswap64(sb.bytes_used);

    return 0;
}

void squashfs_print_info(const struct squashfs_info *info) {
    if (!info) return;
    printf("[squashfs] found at offset 0x%lx\n", info->offset);
    printf("  version    : %u.%u\n", info->version_maj, info->version_min);
    printf("  endian     : %s\n", info->endian == SQUASHFS_ENDIAN_LE ? "little" : "big");
    printf("  inodes     : %u\n", info->inode_count);
    printf("  block size : %u bytes\n", info->block_size);
    printf("  bytes used : %lu bytes\n", info->bytes_used);
}
