#include <stdio.h>
#include "hardlink_map.h"

static void print_entry(const hardlink_entry_t *entry, void *ctx) {
    FILE *out = ctx ? (FILE *)ctx : stdout;
    fprintf(out, "  inode %-8lu  links: %d  first seen: %s\n",
            (unsigned long)entry->inode,
            entry->link_count,
            entry->first_path ? entry->first_path : "(unknown)");
}

void hardlink_report_print(const hardlink_map_t *map, FILE *out) {
    if (!map) return;
    if (!out) out = stdout;

    int dups = hardlink_map_dup_count(map);

    fprintf(out, "\n=== Hard Link Report ===\n");
    fprintf(out, "Unique inodes tracked : %d\n", map->total_inodes);
    fprintf(out, "Inodes with hardlinks : %d\n", dups);

    if (dups == 0) {
        fprintf(out, "No hard links detected.\n");
        return;
    }

    fprintf(out, "\nHard-linked inodes:\n");
    hardlink_map_foreach_dup(map, print_entry, out);
    fprintf(out, "========================\n");
}

#ifdef HARDLINK_REPORT_MAIN
int main(void) {
    hardlink_map_t *m = hardlink_map_create();
    hardlink_map_register(m, 1001, 1, "/etc/passwd");
    hardlink_map_register(m, 1001, 1, "/etc/passwd.bak");
    hardlink_map_register(m, 1002, 1, "/bin/sh");
    hardlink_map_register(m, 1002, 1, "/bin/bash");
    hardlink_map_register(m, 1002, 1, "/usr/bin/sh");
    hardlink_map_register(m, 2000, 1, "/usr/lib/libssl.so");
    hardlink_report_print(m, stdout);
    hardlink_map_free(m);
    return 0;
}
#endif
