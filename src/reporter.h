#ifndef REPORTER_H
#define REPORTER_H

#include <stdint.h>
#include <stdio.h>

/* Output formats supported by the reporter */
typedef enum {
    REPORT_FORMAT_TEXT = 0,
    REPORT_FORMAT_JSON,
    REPORT_FORMAT_CSV
} ReportFormat;

/* Severity levels for findings */
typedef enum {
    SEVERITY_INFO = 0,
    SEVERITY_LOW,
    SEVERITY_MEDIUM,
    SEVERITY_HIGH
} Severity;

/* A single finding entry */
typedef struct {
    char        path[512];
    char        description[256];
    Severity    severity;
    uint64_t    file_size;
    uint32_t    permissions;
} Finding;

/* Report context */
typedef struct {
    Finding    *findings;
    size_t      count;
    size_t      capacity;
    ReportFormat format;
    FILE       *out;
} Report;

/* Initialize a report; out may be NULL to use stdout */
int  report_init(Report *r, ReportFormat fmt, FILE *out);

/* Add a finding to the report */
int  report_add(Report *r, const char *path, const char *desc,
                Severity sev, uint64_t size, uint32_t perms);

/* Write the full report to the output stream */
int  report_write(const Report *r);

/* Free resources held by the report */
void report_free(Report *r);

/* Helper: severity as a short string */
const char *severity_str(Severity s);

#endif /* REPORTER_H */
