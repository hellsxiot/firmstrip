#include "file_classifier.h"
#include "fs_walker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READ_MAGIC_BYTES 16

static void print_classification(const char *path) {
    uint8_t buf[READ_MAGIC_BYTES] = {0};
    size_t nread = 0;
    FILE *f = fopen(path, "rb");
    if (f) {
        nread = fread(buf, 1, READ_MAGIC_BYTES, f);
        fclose(f);
    }
    FileType t = classify_file(path, buf, nread);
    printf("%-12s  %s\n", file_type_name(t), path);
}

static int walk_cb(const char *path, const FileEntry *entry, void *user) {
    (void)user;
    if (entry->is_dir) return 0;
    print_classification(path);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: classify_report <root_dir>\n");
        return 1;
    }
    printf("%-12s  %s\n", "TYPE", "PATH");
    printf("%-12s  %s\n", "------------", "----");
    return fs_walk(argv[1], walk_cb, NULL);
}
