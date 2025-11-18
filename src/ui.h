#ifndef UI_H
#define UI_H

/* Start the ncurses UI at the given path.
 * Returns 0 on success, non-zero on error.
 */
int ui_run(const char *start_path);

#endif /* UI_H */