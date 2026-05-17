#include "xattr_reader.h"
#include <stdio.h>
#include <string.h>

static void print_printable(const char *val, size_t len) {
    for (size_t i = 0; i < len; i++) {
        unsigned char c = (unsigned char)val[i];
        if (c >= 0x20 && c < 0x7f)
            putchar(c);
        else
            printf("\\x%02x", c);
    }
}

void xattr_report(const char *path, const XAttrList *list) {
    if (!list || list->count == 0) return;

    printf("[xattr] %s\n", path);
    for (int i = 0; i < list->count; i++) {
        const XAttr *xa = &list->entries[i];
        printf("  %s = \"", xa->name);
        print_printable(xa->value, xa->value_len);
        printf("\"\n");
    }

    if (xattr_has_security(list))
        printf("  [!] security-related xattr present\n");
}

/* Standalone entry point for quick xattr reporting */
#ifdef XATTR_REPORT_MAIN
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage: xattr_report <file>...\n");
        return 1;
    }
    for (int i = 1; i < argc; i++) {
        XAttrList list;
        if (xattr_read(argv[i], &list) == 0)
            xattr_report(argv[i], &list);
        else
            fprintf(stderr, "failed to read xattrs for %s\n", argv[i]);
    }
    return 0;
}
#endif
