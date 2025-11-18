#include "ui.h"
#include <stdio.h>

int main(int argc, char **argv)
{
    const char *start = ".";
    if (argc > 1) {
        start = argv[1];
    }

    int rc = ui_run(start);
    if (rc != 0) {
        fprintf(stderr, "Exiting with error code %d\n", rc);
    }
    return rc;
}