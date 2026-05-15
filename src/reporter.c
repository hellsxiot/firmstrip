#include "reporter.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL_CAPACITY 64

const char *severity_str(Severity s) {
    switch (s) {
        case SEVERITY_INFO:   return "INFO";
        case SEVERITY_LOW:    return "LOW";
        case SEVERITY_MEDIUM: return "MEDIUM";
        case SEVERITY_HIGH:   return "HIGH";
        default:              return "UNKNOWN";
    }
}

int report_init(Report *r, ReportFormat fmt, FILE *out) {
    if (!r) return -1;
    r->findings = calloc(INITIAL_CAPACITY, sizeof(Finding));
    if (!r->findings) return -1;
    r->count    = 0;
    r->capacity = INITIAL_CAPACITY;
    r->format   = fmt;
    r->out      = out ? out : stdout;
    return 0;
}

int report_add(Report *r, const char *path, const char *desc,
               Severity sev, uint64_t size, uint32_t perms) {
    if (!r || !path || !desc) return -1;

    if (r->count >= r->capacity) {
        size_t new_cap = r->capacity * 2;
        Finding *tmp = realloc(r->findings, new_cap * sizeof(Finding));
        if (!tmp) return -1;
        r->findings = tmp;
        r->capacity = new_cap;
    }

    Finding *f = &r->findings[r->count++];
    strncpy(f->path,        path, sizeof(f->path) - 1);
    strncpy(f->description, desc, sizeof(f->description) - 1);
    f->severity   = sev;
    f->file_size  = size;
    f->permissions = perms;
    return 0;
}

static void write_text(const Report *r) {
    fprintf(r->out, "=== firmstrip report (%zu findings) ===\n", r->count);
    for (size_t i = 0; i < r->count; i++) {
        const Finding *f = &r->findings[i];
        fprintf(r->out, "[%s] %s\n  %s\n  size=%llu perms=%04o\n",
                severity_str(f->severity), f->path, f->description,
                (unsigned long long)f->file_size, f->permissions);
    }
}

static void write_json(const Report *r) {
    fprintf(r->out, "{\"findings\":[\n");
    for (size_t i = 0; i < r->count; i++) {
        const Finding *f = &r->findings[i];
        fprintf(r->out,
            "  {\"path\":\"%s\",\"description\":\"%s\","
            "\"severity\":\"%s\",\"size\":%llu,\"perms\":%u}%s\n",
            f->path, f->description, severity_str(f->severity),
            (unsigned long long)f->file_size, f->permissions,
            (i + 1 < r->count) ? "," : "");
    }
    fprintf(r->out, "]}\n");
}

static void write_csv(const Report *r) {
    fprintf(r->out, "severity,path,description,size,permissions\n");
    for (size_t i = 0; i < r->count; i++) {
        const Finding *f = &r->findings[i];
        fprintf(r->out, "%s,\"%s\",\"%s\",%llu,%04o\n",
                severity_str(f->severity), f->path, f->description,
                (unsigned long long)f->file_size, f->permissions);
    }
}

int report_write(const Report *r) {
    if (!r) return -1;
    switch (r->format) {
        case REPORT_FORMAT_JSON: write_json(r); break;
        case REPORT_FORMAT_CSV:  write_csv(r);  break;
        default:                 write_text(r); break;
    }
    return 0;
}

void report_free(Report *r) {
    if (!r) return;
    free(r->findings);
    r->findings = NULL;
    r->count    = 0;
    r->capacity = 0;
}
