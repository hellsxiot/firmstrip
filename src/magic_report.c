#include "magic_bytes.h"
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_PATH 512

static void scan_file(const char *path) {
    MagicResult r = magic_bytes_identify_file(path);
    if (r.type != MAGIC_UNKNOWN) {
        printf("  [%s] %s (confidence: %d%%)
",
               r.name, path, r.confidence);
    }
}

static void scan_dir(const char *dir_path, int depth) {
    if (depth > 8)
        return;

    DIR *d = opendir(dir_path);
    if (!d)
        return;

    struct dirent *entry;
    while ((entry = readdir(d)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0)
            continue;

        char full[MAX_PATH];
        snprintf(full, sizeof(full), "%s/%s", dir_path, entry->d_name);

        struct stat st;
        if (lstat(full, &st) != 0)
            continue;

        if (S_ISREG(st.st_mode))
            scan_file(full);
        else if (S_ISDIR(st.st_mode))
            scan_dir(full, depth + 1);
    }
    closedir(d);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: magic_report <path>\n");
        return 1;
    }

    magic_bytes_init();
    printf("Magic byte scan: %s\n", argv[1]);
    printf("%-20s %s\n", "Type", "Path");
    printf("%-20s %s\n", "----", "----");

    struct stat st;
    if (stat(argv[1], &st) != 0) {
        perror("stat");
        return 1;
    }

    if (S_ISDIR(st.st_mode))
        scan_dir(argv[1], 0);
    else
        scan_file(argv[1]);

    return 0;
}
