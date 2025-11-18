#define _XOPEN_SOURCE 700
#include "scanner.h"

#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>

static off_t dir_size_recursive(const char *path);
static int compare_entries_desc(const void *a, const void *b);

DirView *scan_directory(const char *path)
{
    DIR *dir;
    struct dirent *ent;
    DirEntry *entries = NULL;
    size_t capacity = 0;
    size_t count = 0;
    off_t total = 0;

    dir = opendir(path);
    if (!dir) {
        return NULL;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        char full[PATH_MAX];
        int n = snprintf(full, sizeof(full), "%s/%s", path, ent->d_name);
        if (n < 0 || (size_t)n >= sizeof(full)) {
            continue; /* path too long, skip */
        }

        struct stat st;
        if (lstat(full, &st) == -1) {
            continue;
        }

        /* Skip symlinks entirely to avoid loops */
        if (S_ISLNK(st.st_mode)) {
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            off_t sz = dir_size_recursive(full);
            total += sz;

            if (count == capacity) {
                size_t newcap = capacity == 0 ? 16 : capacity * 2;
                DirEntry *tmp = realloc(entries, newcap * sizeof(DirEntry));
                if (!tmp) {
                    /* out of memory: clean up and abort */
                    closedir(dir);
                    for (size_t i = 0; i < count; ++i) {
                        free(entries[i].name);
                        free(entries[i].path);
                    }
                    free(entries);
                    return NULL;
                }
                entries = tmp;
                capacity = newcap;
            }

            entries[count].name = strdup(ent->d_name);
            entries[count].path = strdup(full);
            entries[count].size = sz;
            if (!entries[count].name || !entries[count].path) {
                closedir(dir);
                for (size_t i = 0; i <= count; ++i) {
                    free(entries[i].name);
                    free(entries[i].path);
                }
                free(entries);
                return NULL;
            }
            count++;
        } else if (S_ISREG(st.st_mode)) {
            total += st.st_size;
        }
        /* ignore other special file types */
    }

    closedir(dir);

    /* sort subdirs by size descending */
    if (entries && count > 0) {
        qsort(entries, count, sizeof(DirEntry), compare_entries_desc);
    }

    DirView *view = malloc(sizeof(DirView));
    if (!view) {
        for (size_t i = 0; i < count; ++i) {
            free(entries[i].name);
            free(entries[i].path);
        }
        free(entries);
        return NULL;
    }

    view->path = strdup(path);
    view->entries = entries;
    view->count = count;
    view->total_size = total;

    return view;
}

void free_dirview(DirView *view)
{
    if (!view) return;
    if (view->entries) {
        for (size_t i = 0; i < view->count; ++i) {
            free(view->entries[i].name);
            free(view->entries[i].path);
        }
        free(view->entries);
    }
    free(view->path);
    free(view);
}

const char *human_size(off_t bytes, char *buf, size_t buflen)
{
    const char *units[] = { "B", "KiB", "MiB", "GiB", "TiB" };
    int unit_index = 0;
    double size = (double)bytes;

    while (size >= 1024.0 && unit_index < 4) {
        size /= 1024.0;
        unit_index++;
    }

    snprintf(buf, buflen, "%.2f %s", size, units[unit_index]);
    return buf;
}

/* --- internal helpers --- */

static off_t dir_size_recursive(const char *path)
{
    DIR *dir;
    struct dirent *ent;
    off_t total = 0;

    dir = opendir(path);
    if (!dir) {
        return 0; /* no permission or error -> count as 0 */
    }

    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }

        char full[PATH_MAX];
        int n = snprintf(full, sizeof(full), "%s/%s", path, ent->d_name);
        if (n < 0 || (size_t)n >= sizeof(full)) {
            continue;
        }

        struct stat st;
        if (lstat(full, &st) == -1) {
            continue;
        }

        if (S_ISLNK(st.st_mode)) {
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            total += dir_size_recursive(full);
        } else if (S_ISREG(st.st_mode)) {
            total += st.st_size;
        }
    }

    closedir(dir);
    return total;
}

static int compare_entries_desc(const void *a, const void *b)
{
    const DirEntry *ea = (const DirEntry *)a;
    const DirEntry *eb = (const DirEntry *)b;

    if (ea->size < eb->size) return 1;
    if (ea->size > eb->size) return -1;
    return strcmp(ea->name, eb->name);
}