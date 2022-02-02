#include <stdio.h>
#include "file/file.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Program expected 2 args, got %d\n", argc - 1);
        return 1;
    }

    const char* path_from = argv[1];
    const char* path_to = argv[2];

    if (!cp_r(path_from, path_to)) {
        return 1;
    }

    return 0;
}
