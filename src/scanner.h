#ifndef SCANNER_H
#define SCANNER_H

#include <sys/types.h>
#include <stddef.h>

typedef struct {
    char *name;   /* directory name (not full path) */
    char *path;   /* full path */
    off_t size;   /* total size in bytes (recursive) */
} DirEntry;

typedef struct {
    char *path;       /* full path of current directory */
    DirEntry *entries;
    size_t count;
    off_t total_size; /* total size of this directory (files + subdirs) */
} DirView;

/* Scan a directory, compute recursive sizes of its subdirectories.
 * Returns NULL on error.
 */
DirView *scan_directory(const char *path);

/* Free everything allocated inside a DirView. */
void free_dirview(DirView *view);

/* Convert bytes to human-readable string like "12.3 MiB". */
const char *human_size(off_t bytes, char *buf, size_t buflen);

#endif /* SCANNER_H */