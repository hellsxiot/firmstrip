#include "extractor.h"
#include "squashfs.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

/* Magic bytes for quick filesystem detection */
static const uint8_t SQUASHFS_MAGIC[4] = { 0x73, 0x71, 0x73, 0x68 }; /* 'sqsh' */
static const uint8_t CRAMFS_MAGIC[4]   = { 0x45, 0x3d, 0xcd, 0x28 };
static const uint8_t JFFS2_MAGIC[2]    = { 0x19, 0x85 };

int extractor_init(extractor_ctx_t *ctx, const char *image_path,
                   const char *output_dir, int verbose)
{
    if (!ctx || !image_path || !output_dir)
        return -1;

    memset(ctx, 0, sizeof(*ctx));
    ctx->image_path = image_path;
    ctx->output_dir = output_dir;
    ctx->verbose    = verbose;
    return 0;
}

int extractor_scan(extractor_ctx_t *ctx)
{
    FILE *fp;
    uint8_t buf[4];
    uint64_t offset = 0;
    int found = 0;

    fp = fopen(ctx->image_path, "rb");
    if (!fp) {
        fprintf(stderr, "extractor: cannot open %s: %s\n",
                ctx->image_path, strerror(errno));
        return -1;
    }

    /* Slide through the image in 512-byte steps looking for signatures */
    while (fread(buf, 1, sizeof(buf), fp) == sizeof(buf)) {
        fs_type_t type = FS_UNKNOWN;

        if (memcmp(buf, SQUASHFS_MAGIC, 4) == 0)
            type = FS_SQUASHFS;
        else if (memcmp(buf, CRAMFS_MAGIC, 4) == 0)
            type = FS_CRAMFS;
        else if (memcmp(buf, JFFS2_MAGIC, 2) == 0)
            type = FS_JFFS2;

        if (type != FS_UNKNOWN && found < 16) {
            fs_region_t *r = &ctx->regions[found++];
            r->type   = type;
            r->offset = offset;
            r->size   = 0; /* determined during extraction */
            r->name   = (type == FS_SQUASHFS) ? "squashfs" :
                        (type == FS_CRAMFS)   ? "cramfs"   : "jffs2";
            if (ctx->verbose)
                printf("[scan] found %s at offset 0x%llx\n",
                       r->name, (unsigned long long)r->offset);
        }

        offset += 512;
        fseek(fp, (long)offset, SEEK_SET);
    }

    fclose(fp);
    ctx->region_count = (size_t)found;
    return found;
}

int extractor_run(extractor_ctx_t *ctx)
{
    if (mkdir(ctx->output_dir, 0755) != 0 && errno != EEXIST) {
        fprintf(stderr, "extractor: cannot create output dir %s: %s\n",
                ctx->output_dir, strerror(errno));
        return -1;
    }

    for (size_t i = 0; i < ctx->region_count; i++) {
        fs_region_t *r = &ctx->regions[i];
        if (r->type == FS_SQUASHFS) {
            if (squashfs_extract(ctx->image_path, r->offset,
                                 ctx->output_dir, ctx->verbose) != 0) {
                fprintf(stderr, "extractor: squashfs extraction failed at 0x%llx\n",
                        (unsigned long long)r->offset);
                return -1;
            }
        } else {
            if (ctx->verbose)
                printf("[extract] skipping unsupported fs '%s' at 0x%llx\n",
                       r->name, (unsigned long long)r->offset);
        }
    }
    return 0;
}

void extractor_free(extractor_ctx_t *ctx)
{
    if (ctx)
        memset(ctx, 0, sizeof(*ctx));
}
