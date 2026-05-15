#ifndef STRING_TABLE_H
#define STRING_TABLE_H

#include <stddef.h>

/* Simple string interning table for deduplicating path/name strings */

#define STRING_TABLE_INITIAL_CAP 64

typedef struct {
    char **entries;
    size_t count;
    size_t capacity;
} StringTable;

/*
 * Initialize a new string table.
 * Returns 0 on success, -1 on allocation failure.
 */
int string_table_init(StringTable *tbl);

/*
 * Intern a string into the table.
 * Returns a pointer to the stored copy, or NULL on failure.
 * If the string already exists, returns the existing pointer.
 */
const char *string_table_intern(StringTable *tbl, const char *str);

/*
 * Check if a string is already in the table.
 * Returns the stored pointer or NULL if not found.
 */
const char *string_table_lookup(const StringTable *tbl, const char *str);

/*
 * Return the number of unique strings stored.
 */
size_t string_table_count(const StringTable *tbl);

/*
 * Free all memory associated with the table.
 */
void string_table_free(StringTable *tbl);

#endif /* STRING_TABLE_H */
