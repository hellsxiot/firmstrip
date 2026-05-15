#include "signature.h"

#include <stdio.h>
#include <string.h>

/* Magic byte sequences for common firmware/fs formats */
static const uint8_t magic_squashfs_le[] = { 0x68, 0x73, 0x71, 0x73 }; /* hsqs */
static const uint8_t magic_squashfs_be[] = { 0x73, 0x71, 0x73, 0x68 }; /* sqsh */
static const uint8_t magic_ext2[]        = { 0x53, 0xEF };              /* ext2/3/4 superblock */
static const uint8_t magic_jffs2[]       = { 0x19, 0x85 };
static const uint8_t magic_cramfs[]      = { 0x45, 0x3D, 0xCD, 0x28 };
static const uint8_t magic_ubifs[]       = { 0x31, 0x18, 0x10, 0x06 };
static const uint8_t magic_gzip[]        = { 0x1F, 0x8B };
static const uint8_t magic_lzma[]        = { 0x5D, 0x00, 0x00 };
static const uint8_t magic_uimage[]      = { 0x27, 0x05, 0x19, 0x56 };

static const fs_signature_t known_sigs[] = {
    { magic_squashfs_le, 4, 0,    "squashfs-le", "SquashFS little-endian" },
    { magic_squashfs_be, 4, 0,    "squashfs-be", "SquashFS big-endian"    },
    { magic_ext2,        2, 0x38, "ext2/3/4",   "ext2/ext3/ext4 filesystem" },
    { magic_jffs2,       2, 0,    "jffs2",      "JFFS2 filesystem"        },
    { magic_cramfs,      4, 0,    "cramfs",     "CramFS filesystem"       },
    { magic_ubifs,       4, 0,    "ubifs",      "UBIFS filesystem"        },
    { magic_gzip,        2, 0,    "gzip",       "gzip compressed data"    },
    { magic_lzma,        3, 0,    "lzma",       "LZMA compressed data"    },
    { magic_uimage,      4, 0,    "uimage",     "U-Boot firmware image"   },
};

#define NUM_SIGS (sizeof(known_sigs) / sizeof(known_sigs[0]))

sig_match_t scan_signature(const uint8_t *data, size_t scan_len)
{
    sig_match_t result = { NULL, -1 };

    for (size_t i = 0; i < NUM_SIGS; i++) {
        const fs_signature_t *s = &known_sigs[i];
        size_t check_at = s->offset;

        if (check_at + s->magic_len > scan_len)
            continue;

        if (memcmp(data + check_at, s->magic, s->magic_len) == 0) {
            result.sig          = s;
            result.match_offset = (long)check_at;
            return result;
        }
    }

    return result;
}

const char *signature_name(const sig_match_t *match)
{
    if (!match || !match->sig)
        return "unknown";
    return match->sig->name;
}

void list_signatures(void)
{
    printf("Known firmware/filesystem signatures:\n");
    printf("  %-16s  %s\n", "Name", "Description");
    printf("  %-16s  %s\n", "----", "-----------");
    for (size_t i = 0; i < NUM_SIGS; i++) {
        printf("  %-16s  %s\n",
               known_sigs[i].name,
               known_sigs[i].description);
    }
}
