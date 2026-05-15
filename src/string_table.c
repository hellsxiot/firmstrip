#include "string_table.h"

#include <stdlib.h>
#include <string.h>

int string_table_init(StringTable *tbl) {
    if (!tbl) return -1;
    tbl->entries = calloc(STRING_TABLE_INITIAL_CAP, sizeof(char *));
    if (!tbl->entries) return -1;
    tbl->count = 0;
    tbl->capacity = STRING_TABLE_INITIAL_CAP;
    return 0;
}

const char *string_table_lookup(const StringTable *tbl, const char *str) {
    if (!tbl || !str) return NULL;
    for (size_t i = 0; i < tbl->count; i++) {
        if (strcmp(tbl->entries[i], str) == 0)
            return tbl->entries[i];
    }
    return NULL;
}

const char *string_table_intern(StringTable *tbl, const char *str) {
    if (!tbl || !str) return NULL;

    const char *existing = string_table_lookup(tbl, str);
    if (existing) return existing;

    /* Grow if needed */
    if (tbl->count >= tbl->capacity) {
        size_t new_cap = tbl->capacity * 2;
        char **new_entries = realloc(tbl->entries, new_cap * sizeof(char *));
        if (!new_entries) return NULL;
        tbl->entries = new_entries;
        tbl->capacity = new_cap;
    }

    char *copy = strdup(str);
    if (!copy) return NULL;

    tbl->entries[tbl->count++] = copy;
    return copy;
}

size_t string_table_count(const StringTable *tbl) {
    return tbl ? tbl->count : 0;
}

void string_table_free(StringTable *tbl) {
    if (!tbl) return;
    for (size_t i = 0; i < tbl->count; i++) {
        free(tbl->entries[i]);
    }
    free(tbl->entries);
    tbl->entries = NULL;
    tbl->count = 0;
    tbl->capacity = 0;
}
