#define _XOPEN_SOURCE 700
#include "ui.h"
#include "scanner.h"

#include <ncurses.h>
#include <limits.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

static void draw_view(const DirView *view, int selected, int scroll_offset);
static void parent_path(const char *current, char *out, size_t out_size);

int ui_run(const char *start_path)
{
    char current[PATH_MAX];

    if (!realpath(start_path, current)) {
        /* fallback: just copy as-is */
        strncpy(current, start_path, sizeof(current) - 1);
        current[sizeof(current) - 1] = '\0';
    }

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    DirView *view = scan_directory(current);
    if (!view) {
        endwin();
        fprintf(stderr, "Error: could not scan directory: %s\n", current);
        return 1;
    }

    int selected = 0;
    int scroll_offset = 0;
    int ch;

    while (1) {
        clear();
        draw_view(view, selected, scroll_offset);
        refresh();

        ch = getch();
        if (ch == 'q' || ch == 'Q') {
            break;
        } else if (ch == KEY_UP) {
            if (selected > 0) {
                selected--;
                if (selected < scroll_offset) {
                    scroll_offset = selected;
                }
            }
        } else if (ch == KEY_DOWN) {
            if ((size_t)selected + 1 < view->count) {
                selected++;
                int visible_rows = rows - 5; /* header + footer lines */
                if (selected >= scroll_offset + visible_rows) {
                    scroll_offset = selected - visible_rows + 1;
                }
            }
        } else if (ch == '\n' || ch == KEY_ENTER) {
            if (view->count == 0) continue;

            const char *subpath = view->entries[selected].path;
            char resolved[PATH_MAX];
            if (!realpath(subpath, resolved)) {
                /* could not enter – ignore */
                continue;
            }

            free_dirview(view);
            view = scan_directory(resolved);
            if (!view) {
                /* failed to scan, go back to previous path? */
                /* reload current */
                view = scan_directory(current);
                continue;
            }

            strncpy(current, resolved, sizeof(current) - 1);
            current[sizeof(current) - 1] = '\0';
            selected = 0;
            scroll_offset = 0;
        } else if (ch == KEY_BACKSPACE || ch == 127 || ch == KEY_LEFT || ch == 'h') {
            /* go up one directory */
            char parent[PATH_MAX];
            parent_path(current, parent, sizeof(parent));
            if (strcmp(parent, current) == 0) {
                /* already at root */
                continue;
            }

            DirView *new_view = scan_directory(parent);
            if (!new_view) {
                continue;
            }

            free_dirview(view);
            view = new_view;
            strncpy(current, parent, sizeof(current) - 1);
            current[sizeof(current) - 1] = '\0';
            selected = 0;
            scroll_offset = 0;
        } else if (ch == 'r' || ch == 'R') {
            /* rescan current directory */
            DirView *new_view = scan_directory(current);
            if (!new_view) continue;
            free_dirview(view);
            view = new_view;
            selected = 0;
            scroll_offset = 0;
        }
        /* You can add more keybindings if you want */
    }

    free_dirview(view);
    endwin();
    return 0;
}

/* Draw header, list of dirs with bars, and footer. */
static void draw_view(const DirView *view, int selected, int scroll_offset)
{
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    char sizebuf[64];

    /* Header */
    mvprintw(0, 0, "File System Usage Analyzer (du on steroids)  -  q: quit, UP/DOWN: move, ENTER: enter dir, BACKSPACE: up, r: rescan");
    mvhline(1, 0, '-', cols);

    mvprintw(2, 0, "Path: %s", view->path);
    mvprintw(3, 0, "Total size: %s", human_size(view->total_size, sizebuf, sizeof(sizebuf)));
    mvhline(4, 0, '=', cols);

    if (view->count == 0) {
        mvprintw(6, 0, "(No subdirectories or permission denied.)");
        return;
    }

    /* find max size for scaling the bar graph */
    off_t max_size = 0;
    for (size_t i = 0; i < view->count; ++i) {
        if (view->entries[i].size > max_size) {
            max_size = view->entries[i].size;
        }
    }
    if (max_size == 0) max_size = 1;

    int y_start = 5;
    int visible_rows = rows - y_start - 2;   /* leave space for footer */
    if (visible_rows < 1) visible_rows = 1;

    /* Footer / help line */
    mvhline(rows - 2, 0, '-', cols);
    mvprintw(rows - 1, 0, "Showing %d of %zu subdirectories | sorted by size (largest first)",
             visible_rows, view->count);

    for (int i = 0; i < visible_rows; ++i) {
        size_t idx = (size_t)(scroll_offset + i);
        if (idx >= view->count) break;

        int y = y_start + i;

        DirEntry *e = &view->entries[idx];
        const double ratio = (double)e->size / (double)max_size;

        /* Leave some space for name + size text, bars fill the rest */
        int text_cols = 40;
        if (text_cols > cols - 10) text_cols = cols - 10;
        int bar_cols = cols - text_cols - 4;
        if (bar_cols < 10) bar_cols = 10;

        int bar_width = (int)(ratio * bar_cols);
        if (bar_width < 1 && e->size > 0) bar_width = 1;

        /* Selected line highlight */
        if ((int)idx == selected) {
            attron(A_REVERSE);
        }

        /* Clear line first */
        mvhline(y, 0, ' ', cols);

        /* Name + size text */
        char sizebuf2[64];
        snprintf(sizebuf2, sizeof(sizebuf2), "%s", human_size(e->size, sizebuf, sizeof(sizebuf)));
        mvprintw(y, 0, "%-*s %12s", text_cols - 1, e->name, sizebuf2);

        /* Draw bar */
        int bar_start = text_cols + 2;
        for (int x = 0; x < bar_width && bar_start + x < cols; ++x) {
            mvaddch(y, bar_start + x, '#');
        }

        if ((int)idx == selected) {
            attroff(A_REVERSE);
        }
    }
}

/* Compute parent directory path; stays at "/" when already root. */
static void parent_path(const char *current, char *out, size_t out_size)
{
    if (strcmp(current, "/") == 0) {
        strncpy(out, "/", out_size - 1);
        out[out_size - 1] = '\0';
        return;
    }

    char tmp[PATH_MAX];
    strncpy(tmp, current, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';

    char *dir = dirname(tmp);
    if (!dir || dir[0] == '\0') {
        strncpy(out, "/", out_size - 1);
        out[out_size - 1] = '\0';
        return;
    }

    strncpy(out, dir, out_size - 1);
    out[out_size - 1] = '\0';
}