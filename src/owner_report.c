#include "owner_info.h"
#include "fs_walker.h"
#include <stdio.h>
#include <string.h>

typedef struct {
    int total;
    int suspicious;
    int setuid_count;
    int setgid_count;
    int world_writable_count;
} OwnerStats;

static int owner_visit(const char *path, const struct stat *st, void *userdata) {
    (void)st;
    OwnerStats *stats = (OwnerStats *)userdata;
    OwnerInfo info;

    if (owner_info_get(path, &info) != 0) return 0;

    stats->total++;
    if (info.is_setuid)        stats->setuid_count++;
    if (info.is_setgid)        stats->setgid_count++;
    if (info.is_world_writable) stats->world_writable_count++;
    if (owner_info_is_suspicious(&info)) {
        stats->suspicious++;
        owner_info_print(&info, path);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: owner_report <root_path>\n");
        return 1;
    }

    OwnerStats stats = {0};
    printf("=== Owner / Permission Report ===\n");
    printf("Suspicious entries (setuid/setgid/world-writable):\n\n");

    fs_walk(argv[1], owner_visit, &stats);

    printf("\nSummary:\n");
    printf("  Total files scanned : %d\n", stats.total);
    printf("  Suspicious entries  : %d\n", stats.suspicious);
    printf("  SETUID files        : %d\n", stats.setuid_count);
    printf("  SETGID files        : %d\n", stats.setgid_count);
    printf("  World-writable      : %d\n", stats.world_writable_count);
    return 0;
}
