#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "../src/xattr_reader.h"

/* --- helpers --- */

static void fill_fake_list(XAttrList *list) {
    memset(list, 0, sizeof(*list));
    /* entry 0 */
    strncpy(list->entries[0].name, "user.comment", XATTR_NAME_MAX - 1);
    strncpy(list->entries[0].value, "hello", XATTR_VALUE_MAX - 1);
    list->entries[0].value_len = 5;
    /* entry 1 */
    strncpy(list->entries[1].name, "security.selinux", XATTR_NAME_MAX - 1);
    strncpy(list->entries[1].value, "system_u:object_r:etc_t:s0", XATTR_VALUE_MAX - 1);
    list->entries[1].value_len = 26;
    list->count = 2;
}

/* --- tests --- */

static void test_xattr_find_existing(void) {
    XAttrList list;
    fill_fake_list(&list);
    const XAttr *xa = xattr_find(&list, "user.comment");
    assert(xa != NULL);
    assert(strcmp(xa->value, "hello") == 0);
    printf("PASS test_xattr_find_existing\n");
}

static void test_xattr_find_missing(void) {
    XAttrList list;
    fill_fake_list(&list);
    const XAttr *xa = xattr_find(&list, "user.nonexistent");
    assert(xa == NULL);
    printf("PASS test_xattr_find_missing\n");
}

static void test_xattr_has_security_true(void) {
    XAttrList list;
    fill_fake_list(&list);
    assert(xattr_has_security(&list) == 1);
    printf("PASS test_xattr_has_security_true\n");
}

static void test_xattr_has_security_false(void) {
    XAttrList list;
    memset(&list, 0, sizeof(list));
    strncpy(list.entries[0].name, "user.mime_type", XATTR_NAME_MAX - 1);
    list.count = 1;
    assert(xattr_has_security(&list) == 0);
    printf("PASS test_xattr_has_security_false\n");
}

static void test_xattr_null_safety(void) {
    assert(xattr_find(NULL, "foo") == NULL);
    assert(xattr_has_security(NULL) == 0);
    printf("PASS test_xattr_null_safety\n");
}

static void test_xattr_read_nonexistent(void) {
    XAttrList list;
    /* reading xattrs from a nonexistent path should not crash */
    int rc = xattr_read("/tmp/firmstrip_no_such_file_xyz", &list);
    /* rc may be 0 (empty) or -1 depending on platform; just no crash */
    (void)rc;
    printf("PASS test_xattr_read_nonexistent\n");
}

int main(void) {
    test_xattr_find_existing();
    test_xattr_find_missing();
    test_xattr_has_security_true();
    test_xattr_has_security_false();
    test_xattr_null_safety();
    test_xattr_read_nonexistent();
    printf("All xattr_reader tests passed.\n");
    return 0;
}
