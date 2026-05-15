#include "fs_walker.h"

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

static int walk_dir(fs_walker_t *walker, const char *dirpath, int depth)
{
    if (walker->max_depth >= 0 && depth > walker->max_depth)
        return 0;

    DIR *dp = opendir(dirpath);
    if (!dp) {
        fprintf(stderr, "[fs_walker] cannot open dir: %s (%s)\n", dirpath, strerror(errno));
        return -1;
    }

    struct dirent *ent;
    while ((ent = readdir(dp)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;

        fs_entry_t entry;
        memset(&entry, 0, sizeof(entry));

        snprintf(entry.path, FS_WALKER_MAX_PATH, "%s/%s", dirpath, ent->d_name);

        struct stat st;
        int stat_ret = walker->follow_symlinks
            ? stat(entry.path, &st)
            : lstat(entry.path, &st);

        if (stat_ret != 0) {
            fprintf(stderr, "[fs_walker] stat failed: %s\n", entry.path);
            continue;
        }

        entry.size = (uint64_t)st.st_size;
        entry.mode = (uint32_t)st.st_mode;
        entry.uid  = (uint32_t)st.st_uid;
        entry.gid  = (uint32_t)st.st_gid;

        if (S_ISREG(st.st_mode)) {
            entry.type = FS_ENTRY_FILE;
            walker->total_files++;
            walker->total_bytes += entry.size;
        } else if (S_ISDIR(st.st_mode)) {
            entry.type = FS_ENTRY_DIR;
            walker->total_dirs++;
        } else if (S_ISLNK(st.st_mode)) {
            entry.type = FS_ENTRY_SYMLINK;
            ssize_t len = readlink(entry.path, entry.link_target, FS_WALKER_MAX_PATH - 1);
            if (len > 0) entry.link_target[len] = '\0';
        } else {
            entry.type = FS_ENTRY_OTHER;
        }

        if (walker->callback) {
            int ret = walker->callback(&entry, walker->userdata);
            if (ret != 0) {
                closedir(dp);
                return ret;
            }
        }

        if (entry.type == FS_ENTRY_DIR)
            walk_dir(walker, entry.path, depth + 1);
    }

    closedir(dp);
    return 0;
}

int fs_walker_init(fs_walker_t *walker, const char *root, fs_walk_cb cb, void *userdata)
{
    if (!walker || !root) return -1;
    memset(walker, 0, sizeof(*walker));
    strncpy(walker->root, root, FS_WALKER_MAX_PATH - 1);
    walker->callback = cb;
    walker->userdata = userdata;
    walker->max_depth = -1;
    walker->follow_symlinks = 0;
    return 0;
}

int fs_walker_run(fs_walker_t *walker)
{
    if (!walker) return -1;
    return walk_dir(walker, walker->root, 0);
}

void fs_walker_summary(const fs_walker_t *walker)
{
    if (!walker) return;
    printf("[fs_walker] walk complete: root=%s\n", walker->root);
    printf("  files : %llu\n", (unsigned long long)walker->total_files);
    printf("  dirs  : %llu\n", (unsigned long long)walker->total_dirs);
    printf("  bytes : %llu\n", (unsigned long long)walker->total_bytes);
}
