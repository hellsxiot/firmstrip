#ifndef XATTR_READER_H
#define XATTR_READER_H

#include <stddef.h>

/* Maximum number of xattrs per file */
#define XATTR_MAX_COUNT 32
/* Maximum length of xattr name */
#define XATTR_NAME_MAX 256
/* Maximum length of xattr value */
#define XATTR_VALUE_MAX 1024

typedef struct {
    char name[XATTR_NAME_MAX];
    char value[XATTR_VALUE_MAX];
    size_t value_len;
} XAttr;

typedef struct {
    XAttr entries[XATTR_MAX_COUNT];
    int count;
} XAttrList;

/*
 * Read all extended attributes from the file at the given path.
 * Returns 0 on success, -1 on error.
 * Populates out with found xattrs.
 */
int xattr_read(const char *path, XAttrList *out);

/*
 * Free any resources associated with an XAttrList.
 * (Currently a no-op since storage is static, but good practice.)
 */
void xattr_list_free(XAttrList *list);

/*
 * Check whether the given xattr list contains a security-relevant
 * attribute (e.g. security.*, system.posix_acl_*).
 * Returns 1 if found, 0 otherwise.
 */
int xattr_has_security(const XAttrList *list);

/*
 * Find an xattr by name. Returns pointer to XAttr or NULL.
 */
const XAttr *xattr_find(const XAttrList *list, const char *name);

#endif /* XATTR_READER_H */
