#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include <stdint.h>
#include <stddef.h>

/* Supported filesystem types */
typedef enum {
    FS_UNKNOWN = 0,
    FS_SQUASHFS,
    FS_CRAMFS,
    FS_JFFS2,
    FS_EXT2,
} fs_type_t;

/* Result of a filesystem probe */
typedef struct {
    fs_type_t   type;
    const char *name;
    uint64_t    offset;   /* byte offset within firmware image */
    uint64_t    size;     /* size in bytes, 0 if unknown */
} fs_region_t;

/* Extraction context */
typedef struct {
    const char  *image_path;
    const char  *output_dir;
    int          verbose;
    fs_region_t  regions[16];
    size_t       region_count;
} extractor_ctx_t;

/*
 * Initialize an extractor context.
 * Returns 0 on success, -1 on error.
 */
int extractor_init(extractor_ctx_t *ctx, const char *image_path,
                   const char *output_dir, int verbose);

/*
 * Scan the firmware image for known filesystem signatures.
 * Populates ctx->regions and ctx->region_count.
 * Returns number of regions found, or -1 on error.
 */
int extractor_scan(extractor_ctx_t *ctx);

/*
 * Extract all detected filesystem regions to ctx->output_dir.
 * Returns 0 on success, -1 on error.
 */
int extractor_run(extractor_ctx_t *ctx);

/* Free any resources held by the context */
void extractor_free(extractor_ctx_t *ctx);

#endif /* EXTRACTOR_H */
