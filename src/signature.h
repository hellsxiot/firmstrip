#ifndef SIGNATURE_H
#define SIGNATURE_H

#include <stdint.h>
#include <stddef.h>

/* Known filesystem/firmware magic bytes and their descriptions */
typedef struct {
    const uint8_t *magic;
    size_t         magic_len;
    size_t         offset;      /* offset within file where magic appears */
    const char    *name;
    const char    *description;
} fs_signature_t;

/* Result of a signature scan */
typedef struct {
    const fs_signature_t *sig;  /* matched signature, NULL if none */
    long                  match_offset; /* byte offset in file where found */
} sig_match_t;

/*
 * scan_signature
 * Scan the first `scan_len` bytes of `data` for known firmware signatures.
 * Returns a sig_match_t; sig field is NULL if no match found.
 */
sig_match_t scan_signature(const uint8_t *data, size_t scan_len);

/*
 * signature_name
 * Return a human-readable name for a matched signature, or "unknown".
 */
const char *signature_name(const sig_match_t *match);

/*
 * list_signatures
 * Print all known signatures to stdout (for --list-types style output).
 */
void list_signatures(void);

#endif /* SIGNATURE_H */
