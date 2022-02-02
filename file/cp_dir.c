#include "file.h"
#include <sys/stat.h>
#include <stdio.h>

int cp_dir(const char* path_from, const char* path_to) {
    struct stat from_dir_stats;
    if (stat(path_from, &from_dir_stats) < 0) {
        fprintf(stderr, "Can't copy %s\n", path_from);
        perror("Error reading stats in 'cp_dir'");
        return -1;
    }

    int from_dir_mode =  from_dir_stats.st_mode & 0777;

    if (mkdir(path_to, from_dir_mode) < 0) {
        fprintf(stderr, "Can't copy %s\n", path_from);
        perror("Error creating directory");
        return -1;
    }

    return 0;
}
