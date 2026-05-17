#include "inode_stat.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

static const char *inode_type_str(InodeType t) {
    switch (t) {
        case INODE_REG:     return "regular";
        case INODE_DIR:     return "directory";
        case INODE_LNK:     return "symlink";
        case INODE_BLK:     return "block";
        case INODE_CHR:     return "char";
        case INODE_FIFO:    return "fifo";
        case INODE_SOCK:    return "socket";
        default:            return "unknown";
    }
}

static InodeType mode_to_inode_type(mode_t mode) {
    if (S_ISREG(mode))  return INODE_REG;
    if (S_ISDIR(mode))  return INODE_DIR;
    if (S_ISLNK(mode))  return INODE_LNK;
    if (S_ISBLK(mode))  return INODE_BLK;
    if (S_ISCHR(mode))  return INODE_CHR;
    if (S_ISFIFO(mode)) return INODE_FIFO;
    if (S_ISSOCK(mode)) return INODE_SOCK;
    return INODE_UNKNOWN;
}

int inode_stat_fill(const char *path, InodeStat *out) {
    if (!path || !out) return -1;

    struct stat st;
    if (lstat(path, &st) != 0) return -1;

    memset(out, 0, sizeof(*out));
    strncpy(out->path, path, INODE_PATH_MAX - 1);
    out->inode_num  = (uint64_t)st.st_ino;
    out->nlink      = (uint32_t)st.st_nlink;
    out->uid        = st.st_uid;
    out->gid        = st.st_gid;
    out->mode       = st.st_mode;
    out->size       = (uint64_t)st.st_size;
    out->mtime      = st.st_mtime;
    out->type       = mode_to_inode_type(st.st_mode);
    return 0;
}

void inode_stat_print(const InodeStat *s, FILE *out) {
    if (!s || !out) return;
    char tbuf[32];
    struct tm *tm_info = localtime(&s->mtime);
    strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(out, "path=%-40s  ino=%-8" PRIu64 "  type=%-9s  mode=%04o  "
                 "uid=%-5u  gid=%-5u  nlink=%-3u  size=%-10" PRIu64 "  mtime=%s\n",
            s->path, s->inode_num, inode_type_str(s->type),
            s->mode & 07777, s->uid, s->gid, s->nlink, s->size, tbuf);
}
