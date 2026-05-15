#ifndef FS_WALKER_H
#define FS_WALKER_H

#include <stdint.h>
#include <stddef.h>

#define FS_WALKER_MAX_PATH 512
#define FS_WALKER_MAX_DEPTH 32

typedef enum {
    FS_ENTRY_FILE,
    FS_ENTRY_DIR,
    FS_ENTRY_SYMLINK,
    FS_ENTRY_OTHER
} fs_entry_type_t;

typedef struct {
    char path[FS_WALKER_MAX_PATH];
    char link_target[FS_WALKER_MAX_PATH]; /* only for symlinks */
    fs_entry_type_t type;
    uint64_t size;
    uint32_t mode;
    uint32_t uid;
    uint32_t gid;
} fs_entry_t;

/* callback invoked for each entry found during walk */
typedef int (*fs_walk_cb)(const fs_entry_t *entry, void *userdata);

typedef struct {
    char root[FS_WALKER_MAX_PATH];
    fs_walk_cb callback;
    void *userdata;
    int max_depth;       /* -1 for unlimited */
    int follow_symlinks;
    uint64_t total_files;
    uint64_t total_dirs;
    uint64_t total_bytes;
} fs_walker_t;

/* initialize walker with a root directory */
int fs_walker_init(fs_walker_t *walker, const char *root, fs_walk_cb cb, void *userdata);

/* start the walk */
int fs_walker_run(fs_walker_t *walker);

/* print a summary after walk completes */
void fs_walker_summary(const fs_walker_t *walker);

#endif /* FS_WALKER_H */
