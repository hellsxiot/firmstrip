#include "xattr_reader.h"
#include <string.h>
#include <stdio.h>

#ifdef __linux__
#include <sys/xattr.h>
#define HAVE_XATTR 1
#else
#define HAVE_XATTR 0
#endif

int xattr_read(const char *path, XAttrList *out) {
    if (!path || !out) return -1;
    memset(out, 0, sizeof(*out));

#if HAVE_XATTR
    char namebuf[65536];
    ssize_t listlen = llistxattr(path, namebuf, sizeof(namebuf));
    if (listlen < 0) {
        /* no xattrs or not supported — treat as empty */
        return 0;
    }

    const char *p = namebuf;
    while (p < namebuf + listlen && out->count < XATTR_MAX_COUNT) {
        XAttr *xa = &out->entries[out->count];
        strncpy(xa->name, p, XATTR_NAME_MAX - 1);
        xa->name[XATTR_NAME_MAX - 1] = '\0';

        ssize_t vlen = lgetxattr(path, p, xa->value, XATTR_VALUE_MAX - 1);
        if (vlen >= 0) {
            xa->value_len = (size_t)vlen;
            xa->value[vlen] = '\0';
            out->count++;
        }
        p += strlen(p) + 1;
    }
    return 0;
#else
    (void)path;
    return 0;
#endif
}

void xattr_list_free(XAttrList *list) {
    (void)list;
    /* static storage, nothing to free */
}

int xattr_has_security(const XAttrList *list) {
    if (!list) return 0;
    for (int i = 0; i < list->count; i++) {
        const char *n = list->entries[i].name;
        if (strncmp(n, "security.", 9) == 0 ||
            strncmp(n, "system.posix_acl_", 17) == 0) {
            return 1;
        }
    }
    return 0;
}

const XAttr *xattr_find(const XAttrList *list, const char *name) {
    if (!list || !name) return NULL;
    for (int i = 0; i < list->count; i++) {
        if (strcmp(list->entries[i].name, name) == 0)
            return &list->entries[i];
    }
    return NULL;
}
